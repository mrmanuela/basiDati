#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>

#include "defines.h"

typedef enum {
	UTENTE = 1,
	UTENTECERTIFICATO,
	GESTORE,
	FAILED_LOGIN
} role_t;



struct configuration conf;
static MYSQL* conn;


static role_t attempt_login(MYSQL* conn, char* username, char* password) {
	MYSQL_STMT* login_procedure;
	MYSQL_BIND param[3]; 
	int role = 0;

	if (!setup_prepared_stmt(&login_procedure, "CALL login(?, ?, ? )", conn)) {
		print_stmt_error(login_procedure, "Unable to initialize login statement\n");
		goto err2;
	}

	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = username;
	param[0].buffer_length = strlen(username);

	param[1].buffer_type = MYSQL_TYPE_VAR_STRING; 
	param[1].buffer = password;
	param[1].buffer_length = strlen(password);

	param[2].buffer_type = MYSQL_TYPE_LONG; 
	param[2].buffer = &role;
	param[2].buffer_length = sizeof(role);

	if (mysql_stmt_bind_param(login_procedure, param) != 0) { 
		print_stmt_error(login_procedure, "Could not bind parameters for login");
		goto err;
	}

	if (mysql_stmt_execute(login_procedure) != 0) {
		print_stmt_error(login_procedure, "Could not execute login procedure");
		goto err;
	}

	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_LONG; 
	param[0].buffer = &role;
	param[0].buffer_length = sizeof(role);


	if (mysql_stmt_bind_result(login_procedure, param)) {
		print_stmt_error(login_procedure, "Could not retrieve output parameter");
		goto err;
	}

	if (mysql_stmt_fetch(login_procedure)) {
		print_stmt_error(login_procedure, "Could not buffer results");
		goto err;
	}
	mysql_stmt_close(login_procedure);
	return role;

err:
	mysql_stmt_close(login_procedure);
err2:
	return FAILED_LOGIN;
}



static int attempt_registration(MYSQL* conn) {
	MYSQL_STMT* registration_procedure;
	MYSQL_BIND param[8];

	char codFisc[17];
	char nome[51];
	char cognome[51];
	char nascita[11];
	char residenza[51];
	char fatturazione[51];
	MYSQL_TIME* date;

	bool usernamenull;
	bool passwordnull;
	bool nomenull;
	bool cognomenull;
	bool cfnull;
	bool datanull;
	bool indResidenzanull;
	

	utente:
	printf("\nUsername: ");
	getInput(50, conf.username);
	printf("Password: ");
	getInput(50, conf.password);
	printf("Nome: ");
	getInput(50, nome);
	printf("Cognome: ");
	getInput(50, cognome);
	printf("Codice fiscale: ");
	getInput(16, codFisc);
	printf("Data compleanno (gg-mm-aaaa): ");
	getInput(10, nascita);
	printf("Indirizzo residenza: ");
	getInput(50, residenza);
	printf("Indirizzo fatturazione: ");
	getInput(50, fatturazione);
	
	if (strlen(codFisc) != 16) {
		printf("\nIl cf deve avere 16 caratteri.\n");
		goto utente;
	}

	date = malloc(sizeof(MYSQL_TIME*));
	date->time_type = MYSQL_TIMESTAMP_DATE;

	switch (getBirthDate(nascita, date)) {
	case EXIT_SUCCESS:
		break;
	case EXIT_FAILURE:
		goto utente;
	}

	if (conf.username[0] == '\0')
		usernamenull = true;
	else
		usernamenull = false;
	if (conf.password[0] == '\0')
		passwordnull = true;
	else
		passwordnull = false;
	if (nome[0] == '\0')
		nomenull = true;
	else
		nomenull = false;
	if (cognome[0] == '\0')
		cognomenull = true;
	else
		cognomenull = false;
	if (codFisc[0] == '\0' )
		cfnull = true;
	else
		cfnull = false;
	if (nascita[0] == '\0')
		datanull = true;
	else {
		datanull = false;
		}
	if (residenza[0] == '\0')
		indResidenzanull = true;
	else
		indResidenzanull = false;


	switch (validateParams()) {
	case EXIT_SUCCESS:
		break;
	case EXIT_FAILURE:
		goto utente;
	}

	if (!setup_prepared_stmt(&registration_procedure, "CALL creaUtente(?,?,?,?,?,?,?,?)", conn)) {
		print_stmt_error(registration_procedure, "Unable to initialize registration statement\n");
		goto err2;
	}

	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING; 
	param[0].buffer = conf.username;
	param[0].buffer_length = strlen(conf.username);
	param[0].is_null = &usernamenull;

	param[1].buffer_type = MYSQL_TYPE_VAR_STRING; 
	param[1].buffer = conf.password;
	param[1].buffer_length = strlen(conf.password);
	param[1].is_null = &passwordnull;

	param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[2].buffer = nome;
	param[2].buffer_length = strlen(nome);
	param[2].is_null = &nomenull;

	param[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[3].buffer = cognome;
	param[3].buffer_length = strlen(cognome);
	param[3].is_null = &cognomenull;

	param[4].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[4].buffer = codFisc;
	param[4].is_null = &cfnull;

	param[5].buffer_type = MYSQL_TYPE_DATE;
	param[5].buffer = date;
	param[5].buffer_length = sizeof(date);
	param[5].is_null = &datanull;

	param[6].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[6].buffer = residenza;
	param[6].buffer_length = strlen(residenza);
	param[6].is_null = &indResidenzanull;

	param[7].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[7].buffer = fatturazione;
	param[7].buffer_length = strlen(fatturazione);

	if (mysql_stmt_bind_param(registration_procedure, param) != 0) {
		print_stmt_error(registration_procedure, "Could not bind parameters for registration");
		goto err;
	}

	if (mysql_stmt_execute(registration_procedure) != 0) {
		print_stmt_error(registration_procedure, "Could not execute registration procedure");
		goto err;
	}

	mysql_stmt_close(registration_procedure);
	return EXIT_SUCCESS;
err:
	mysql_stmt_close(registration_procedure);
err2:
	return EXIT_FAILURE;
}


static int attempt_registration_gestore(MYSQL* conn) {

	MYSQL_STMT* registration_procedure;
	MYSQL_BIND param[2];

	bool usernamenull;
	bool passwordnull;

utente:
	printf("\nUsername: ");
	getInput(50, conf.username);
	printf("Password: ");
	getInput(50, conf.password);


	if (conf.username[0] == '\0')
		usernamenull = true;
	else
		usernamenull = false;
	if (conf.password[0] == '\0')
		passwordnull = true;
	else
		passwordnull = false;
	

	switch (validateParams()) {
	case EXIT_SUCCESS:
		break;
	case EXIT_FAILURE:
		goto utente;
	}


	if (!setup_prepared_stmt(&registration_procedure, "CALL creaGestore(?,?)", conn)) {
		print_stmt_error(registration_procedure, "Unable to initialize registration statement\n");
		goto err2;
	}

	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = conf.username;
	param[0].buffer_length = strlen(conf.username);
	param[0].is_null = &usernamenull;

	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[1].buffer = conf.password;
	param[1].buffer_length = strlen(conf.password);
	param[1].is_null = &passwordnull;

	
	if (mysql_stmt_bind_param(registration_procedure, param) != 0) {
		print_stmt_error(registration_procedure, "Could not bind parameters for registration");
		goto err;
	}

	if (mysql_stmt_execute(registration_procedure) != 0) {
		print_stmt_error(registration_procedure, "Could not execute registration procedure");
		goto err;
	}

	mysql_stmt_close(registration_procedure);
	return EXIT_SUCCESS;
err:
	mysql_stmt_close(registration_procedure);
err2:
	return EXIT_FAILURE;
}



int main(void) {

	char choicesAccess[4] = { '1','2','3','4'};
	char resAccess;
	role_t role;

	if (!parse_config("users/login.json", &conf)) {
		fprintf(stderr, "Unable to load login configuration\n");
		exit(EXIT_FAILURE);
	}

	conn = mysql_init(NULL);
	if (conn == NULL) {
		fprintf(stderr, "mysql_init() failed (probably out of memory)\n");
		exit(EXIT_FAILURE);
	}

	if (mysql_real_connect(conn, conf.host, conf.db_username, conf.db_password, conf.database, conf.port, NULL, CLIENT_MULTI_STATEMENTS | CLIENT_MULTI_RESULTS) == NULL) {
		fprintf(stderr, "mysql_real_connect() failed\n");
		mysql_close(conn);
		exit(EXIT_FAILURE);
	}

accessPage:
	printf("\033[2J\033[H");
	printf("Benvenuto a BachecaAnnunci!\n");
	printf("\nCosa vuoi fare?\n");
	printf("1) Accedi\n");
	printf("2) Registra Utente\n");
	printf("3) Registra Gestore\n");
	printf("4) Esci\n");
	resAccess = multiChoice("\nSeleziona un'opzione", choicesAccess, 4);
	switch (resAccess) {
	case '1':
		goto loginPage;
	case '2':
		goto registerPageUtente;
	case '3':
		goto registerPageGestore;
	case '4':
		goto exitPage;
	}

loginPage:
	printf("\nUsername: ");
	getInput(50, conf.username);
	printf("Password: ");
	getInput(50, conf.password);

	role = attempt_login(conn, conf.username, conf.password);

	switch (role) {
	case UTENTE:
		run_as_utente(conn);
		goto accessPage;

	case UTENTECERTIFICATO:
		run_as_utenteCertificato(conn);
		goto accessPage;

	case GESTORE:
		run_as_gestore(conn);
		goto accessPage;

	case FAILED_LOGIN:
		fprintf(stderr, "Username o Password sono sbagliati\n");
		getchar();
		goto accessPage;

	default:
		fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
		goto exitPage;
	}

registerPageUtente:

	switch (attempt_registration(conn)) {
	case EXIT_FAILURE:
		printf("\nRiprova a registrarti.\n");
		getchar();
		goto accessPage;
	case EXIT_SUCCESS:
		printf("\nRegistrazione avvenuta con sucesso, effettua il login per iniziare a usare i nostri servizi.\n");
		getchar();
		goto accessPage;
	}

registerPageGestore:
	switch (attempt_registration_gestore(conn)) {
	case EXIT_FAILURE:
		printf("\nRiprova a registrarti.\n");
		getchar();
		goto accessPage;
	case EXIT_SUCCESS:
		printf("\nRegistrazione avvenuta con sucesso, effettua il login.\n");
		getchar();
		goto accessPage;
	}

exitPage:
	printf("\033[2J\033[H");
	printf("Arrivederci!\n");
	mysql_close(conn);
	return EXIT_SUCCESS;
}


