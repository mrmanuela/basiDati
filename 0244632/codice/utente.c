#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defines.h"


 void aggiorna_utente_certificato(MYSQL* conn) {

	MYSQL_STMT* prepared_stmt;
	MYSQL_BIND param[5];

	char cartaCredito[17];
	char intestatario[50];
	char cvc_cvv[4];
	char data[11];
	bool cartanull;
	bool intestatarionull;
	bool cvc_cvvnull;
	bool datanull;
	MYSQL_TIME* date;

	printf("\nInserisci i seguenti dati per autenticare l'utente.\n");
utente:
	printf("\nNumero carta di credito: ");
	getInput(17, cartaCredito);
	printf("Intestatario carta: ");
	getInput(50, intestatario);
	printf("CVC/CVV: ");
	getInput(3, cvc_cvv);
	printf("Data scadenza carta (mm-aa): ");
	getInput(10, data);

	if (strlen(cartaCredito) != 16) {
		printf("\nCarta di credito deve avere 16 caratteri.\n");
		goto utente;
	}

	if (strlen(cvc_cvv) != 3) {
		printf("\nCVC_CVV deve avere 3 caratteri.\n");
		goto utente;
	}

	date = malloc(sizeof(MYSQL_TIME*));
	date->time_type = MYSQL_TIMESTAMP_DATE;


	switch (getExpirationDate(data, date)) {
	case EXIT_SUCCESS:
		break;
	case EXIT_FAILURE:
		goto utente;
	}

	if (cartaCredito[0] == '\0')
		cartanull = true;
	else
		cartanull = false;
	if (intestatario[0] == '\0')
		intestatarionull = true;
	else
		intestatarionull = false;
	if (cvc_cvv[0] == '\0')
		cvc_cvvnull = true;
	else
		cvc_cvvnull = false;
	if (data[0] == '\0')
		datanull = true;
	else
		datanull = false;

	switch (validateParams()) {
	case EXIT_SUCCESS:
		break;
	case EXIT_FAILURE:
		goto utente;
	}

	if (!setup_prepared_stmt(&prepared_stmt, "call certificaUtente(?, ?, ?, ?, ?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize exam list statement\n", false);
	}

	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = conf.username;
	param[0].buffer_length = strlen(conf.username);

	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[1].buffer = intestatario;
	param[1].buffer_length = strlen(intestatario);
	param[1].is_null = &intestatarionull;

	param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[2].buffer = cartaCredito;
	param[2].buffer_length = strlen(cartaCredito);
	param[2].is_null = &cartanull;

	param[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[3].buffer = cvc_cvv;
	param[3].buffer_length = strlen(cvc_cvv);
	param[3].is_null = &cvc_cvvnull;

	param[4].buffer_type = MYSQL_TYPE_DATE;
	param[4].buffer = date;
	param[4].buffer_length = sizeof(date);
	param[4].is_null = &datanull;

	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for upgrading user\n", true);
	}

	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error(prepared_stmt, "An error occurred while upgrading the user.");
	}
	else {
		printf("Utente certificato correttamente...\n");
	}
	getchar();
	mysql_stmt_close(prepared_stmt);
	return main();
}



void utentePage(MYSQL* conn) {

	char options[7] = { '1','2', '3','4','5','6','7'};
	char op;

	while (true) {
	start:
		printf("\033[2J\033[H");
		printf("Hai effetuato l'accesso come Utente\n");
		printf("\nCosa vuoi fare?\n");
		printf("1) Visualizza bacheca pubblica\n");
		printf("2) Visualizza annunci seguiti\n");
		printf("3) Visualizza chats\n");
		printf("4) Informazioni anagrafiche\n");
		printf("5) Contatti\n");
		printf("6) Autentica utente\n");
		printf("7) Esci\n");

		op = multiChoice("\nSeleziona un'opzione", options, 7);

		switch (op) {
		case '1':
			visualizza_bacheca_pubblica(conn);
			goto start;
		case '2':
			visualizza_annunci_seguiti(conn);
			goto start;
		case '3':
			visualizza_chat(conn);
			goto start;
		case '4':
			gestisci_informazioni_personali(conn);
			goto start;
		case'5':
			gestisci_contatti(conn);
			goto start;
		case '6':
			aggiorna_utente_certificato(conn);
			goto start;
		case '7':
			return;
		default:
			fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
			abort();
		}
		getchar();
	}
}


void run_as_utente(MYSQL* conn) {

	printf("Switching to administrative role...\n");

	if (!parse_config("users/utente.json", &conf)) {
		fprintf(stderr, "Unable to load administrator configuration\n");
		exit(EXIT_FAILURE);
	}

	if (mysql_change_user(conn, conf.db_username, conf.db_password, conf.database)) {
		fprintf(stderr, "mysql_change_user() failed\n");
		exit(EXIT_FAILURE);
	}

	utentePage(conn);

	if (!parse_config("users/login.json", &conf)) {
		fprintf(stderr, "Unable to load login configuration\n");
		exit(EXIT_FAILURE);
	}

	if (mysql_change_user(conn, conf.db_username, conf.db_password, conf.database)) {
		fprintf(stderr, "mysql_change_user() needed for the logout has failed\n");
		exit(EXIT_FAILURE);
	}
}