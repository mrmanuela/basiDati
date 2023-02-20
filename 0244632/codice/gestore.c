#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defines.h"



void visualizzaCategoria(MYSQL* conn) {
	MYSQL_STMT* prepared_stmt;


	if (!setup_prepared_stmt(&prepared_stmt, "call visualizzaCategoria()", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize categories list statement\n", false);
	}

	if (mysql_stmt_execute(prepared_stmt) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not retrieve categories list\n", true);
	}

	dump_result_set(conn, prepared_stmt, "\n***Lista categoria***");
	mysql_stmt_close(prepared_stmt);
	getchar();
}


static void riscuoti_percentuale(MYSQL* conn) {
	MYSQL_STMT* prepared_stmt;
	MYSQL_BIND param;

	char username[51];

	if (!setup_prepared_stmt(&prepared_stmt, "call visualizzaReport()", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize categories list statement\n", false);
	}

	if (mysql_stmt_execute(prepared_stmt) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not retrieve percentages list\n", true);
	}

	dump_result_set(conn, prepared_stmt, "\n***Report***");
	mysql_stmt_close(prepared_stmt);

	printf("\nUsername dell'utente per riscuotere la percentuale: ");
	getInput(50, username, false);

	if (!setup_prepared_stmt(&prepared_stmt, "call riscuotiPercentuale(?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize collect percentage statement\n", false);
	}

	memset(&param, 0, sizeof(param));

	param.buffer_type = MYSQL_TYPE_VAR_STRING;
	param.buffer = username;
	param.buffer_length = strlen(username);

	if (mysql_stmt_bind_param(prepared_stmt, &param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for collect percentage\n", true);
	}

	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error(prepared_stmt, "An error occurred while collecting percentage.");
	}
	else {
		printf("Riscossione avvenuta con successo...\n");
	}
	getchar();
	mysql_stmt_close(prepared_stmt);
}


static void crea_categoria(MYSQL* conn) {
	MYSQL_STMT* prepared_stmt;
	MYSQL_BIND param[2];

	char nome[51];
	char categoriapadre[51];
	bool categoriapadrenull;
	bool categorianull;

    visualizzaCategoria(conn);
		
	again:
	printf("\nNome della categoria: ");
	getInput(50, nome, false);

	printf("Nome categoria padre: ");
	getInput(50, categoriapadre, false);

	if (nome[0] == '\0')
		categorianull = true;
	else
		categorianull = false;

	if (categoriapadre[0] == '\0')
		categoriapadrenull = true;
	else
		categoriapadrenull = false;

	switch (validateParams()) {
	case EXIT_SUCCESS:
		break;
	case EXIT_FAILURE:
		goto again;
	}

	if (!setup_prepared_stmt(&prepared_stmt, "call creaCategoria(?, ?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize category list statement\n", false);
	}

	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = nome;
	param[0].buffer_length = strlen(nome);
	param[0].is_null = &categorianull;

	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[1].buffer = categoriapadre;
	param[1].buffer_length = strlen(categoriapadre);
	param[1].is_null = &categoriapadrenull;

	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for category list\n", true);
	}

	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error(prepared_stmt, "\nAn error occurred while registering category.");
	}
	else {
		printf("Categoria creata correttamente...\n");
	}
	getchar();
	mysql_stmt_close(prepared_stmt);
}


void gestorePage(MYSQL* conn)
{
	char options[4] = { '1','2', '3','4'};
	char op;
	while (true) {
	start:
		printf("\033[2J\033[H");
		printf("Hai effetuato l'accesso come Gestore\n\n");
		printf("Cosa vuoi fare?\n");
		printf("1) Visualizza categoria\n");
		printf("2) Crea categoria\n");
		printf("3) Riscuoti percentuale\n");
		printf("4) Esci\n");

		op = multiChoice("Seleziona un'opzione", options, 4);

		switch (op) {
		case'1':
			visualizzaCategoria(conn);
			goto start;
		case '2':
			crea_categoria(conn);
			goto start;
		case '3':
			riscuoti_percentuale(conn);
			goto start;
		case '4':
			return;

		default:
			fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
			abort();
		}
		getchar();
	}
}


void run_as_gestore(MYSQL* conn) {
	printf("Switching to administrative role...\n");

	if (!parse_config("users/gestore.json", &conf)) {
		fprintf(stderr, "Unable to load administrator configuration\n");
		exit(EXIT_FAILURE);
	}

	if (mysql_change_user(conn, conf.db_username, conf.db_password, conf.database)) {
		fprintf(stderr, "mysql_change_user() failed\n");
		exit(EXIT_FAILURE);
	}

	gestorePage(conn);

	if (!parse_config("users/login.json", &conf)) {
		fprintf(stderr, "Unable to load login configuration\n");
		exit(EXIT_FAILURE);
	}

	if (mysql_change_user(conn, conf.db_username, conf.db_password, conf.database)) {
		fprintf(stderr, "mysql_change_user() needed for the logout has failed\n");
		exit(EXIT_FAILURE);
	}
}