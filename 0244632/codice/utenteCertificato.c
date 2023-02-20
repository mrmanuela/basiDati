#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"

static void creaAnnuncio(MYSQL* conn) {

    MYSQL_STMT* prepared_stmt;
    MYSQL_BIND param[6];

    char titolo[51];
    char descrizione[101];
    char prezzo[9];
    char categoria[51];
    char foto[51];
    bool titolonull;
    bool descrizionenull;
    bool prezzonull;
    bool categorianull;


    if (!setup_prepared_stmt(&prepared_stmt, "call visualizzaCategoria()", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize categories list statement\n", false);
    }

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not retrieve categories list\n", true);
    }

    dump_result_set(conn, prepared_stmt, "\n***Lista categoria***");
    mysql_stmt_close(prepared_stmt);

    start:
    printf("\nTitolo: ");
    getInput(50, titolo);
    printf("Descrizione: ");
    getInput(100, descrizione);
    printf("Categoria: ");
    getInput(50, categoria);
    printf("Foto: ");
    getInput(50, foto);
    printf("Prezzo: ");
    getInput(9, prezzo);

    switch (validateParams()) {
    case EXIT_SUCCESS:
        break;
    case EXIT_FAILURE:
        goto start;
    }

    if (titolo[0] == '\0')
        titolonull = true;
    else
        titolonull = false;
    if (descrizione[0] == '\0')
        descrizionenull = true;
    else
        descrizionenull = false;
    if (prezzo[0] == '\0')
        prezzonull = true;
    else
        prezzonull = false;
    if (categoria[0] == '\0')
        categorianull = true;
    else
        categorianull = false;

 
    if (!setup_prepared_stmt(&prepared_stmt, "call creaAnnuncio(?, ?, ?, ?, ?, ?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize annuncement statement\n", false);
    }

    memset(param, 0, sizeof(param));

    param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[0].buffer = titolo;
    param[0].buffer_length = strlen(titolo);
    param[0].is_null = &titolonull;

    param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[1].buffer = descrizione;
    param[1].buffer_length = strlen(descrizione);
    param[1].is_null = &descrizionenull;

    param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[2].buffer = prezzo;
    param[2].buffer_length = strlen(prezzo);
    param[2].is_null = &prezzonull;

    param[3].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[3].buffer = foto;
    param[3].buffer_length = strlen(foto);

    param[4].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[4].buffer = categoria;
    param[4].buffer_length = strlen(categoria);
    param[4].is_null = &categorianull;

    param[5].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[5].buffer = conf.username;
    param[5].buffer_length = strlen(conf.username);

    if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for creating annuncement\n", true);
    }

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        print_stmt_error(prepared_stmt, "An error occurred while creating the annuncement");
    }
    else {
        printf("Annuncio creato correttamente...\n");
    }
    getchar();
    mysql_stmt_close(prepared_stmt);
}


void visualizza_miei_annunci(MYSQL* conn) {

    MYSQL_STMT* prepared_stmt;
    MYSQL_BIND param;

    if (!setup_prepared_stmt(&prepared_stmt, "call visualizzaMieiAnnunci(?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize my ads statement\n", false);
    }

    memset(&param, 0, sizeof(param));

    param.buffer_type = MYSQL_TYPE_VAR_STRING;
    param.buffer = conf.username;
    param.buffer_length = strlen(conf.username);

    if (mysql_stmt_bind_param(prepared_stmt, &param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for your ads\n", true);
    }

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not retrieve the list of your ads\n", true);
    }

    dump_result_set(conn, prepared_stmt, "\n***Lista degli annunci***");
    mysql_stmt_close(prepared_stmt);
}

void visualizza_annunci(MYSQL* conn) {

    MYSQL_STMT* prepared_stmt;
    MYSQL_BIND param;

    if (!setup_prepared_stmt(&prepared_stmt, "call visualizzaAnnunci(?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize my ads statement\n", false);
    }

    memset(&param, 0, sizeof(param));

    param.buffer_type = MYSQL_TYPE_VAR_STRING;
    param.buffer = conf.username;
    param.buffer_length = strlen(conf.username);

    if (mysql_stmt_bind_param(prepared_stmt, &param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for your ads\n", true);
    }

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not retrieve the list of your ads\n", true);
    }

    dump_result_set(conn, prepared_stmt, "\n***Lista degli annunci***");
    mysql_stmt_close(prepared_stmt);
}


void utenteCertificatoPage(MYSQL* conn) {

    char options[9] = { '1','2','3','4','5','6','7','8','9' };
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
        printf("6) Crea annuncio\n");
        printf("7) Segna annuncio come venduto\n");
        printf("8) Rimuovi annuncio\n");
        printf("9) Esci\n");

        op = multiChoice("\nSeleziona un' opzione", options, 9);

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
        case '5':
            gestisci_contatti(conn);
            goto start;
        case '6':
        {
            creaAnnuncio(conn);
            goto start;
        }
        case '7':
        {
            visualizza_miei_annunci(conn);
            segna_venduto(conn);
            goto start;
        }
        case '8':
        {
            visualizza_annunci(conn);
            rimuovi_annuncio(conn);
            goto start;
        }
        case '9':
            return;
            return;
            return;
        }
        getchar();
    }
}


void run_as_utenteCertificato(MYSQL* conn) {
    printf("Switching to administrative role...\n");

    if (!parse_config("users/utentecertificato.json", &conf)) {
        fprintf(stderr, "Unable to load auth user configuration\n");
        exit(EXIT_FAILURE);
    }

    if (mysql_change_user(conn, conf.db_username, conf.db_password, conf.database)) {
        fprintf(stderr, "mysql_change_user() failed\n");
        exit(EXIT_FAILURE);
    }

    utenteCertificatoPage(conn);

    if (!parse_config("users/login.json", &conf)) {
        fprintf(stderr, "Unable to load login configuration\n");
        exit(EXIT_FAILURE);
    }

    if (mysql_change_user(conn, conf.db_username, conf.db_password, conf.database)) {
        fprintf(stderr, "mysql_change_user() needed for the logout has failed\n");
        exit(EXIT_FAILURE);
    }
}


void rimuovi_annuncio(MYSQL* conn) {
    MYSQL_STMT* prepared_stmt;
    MYSQL_BIND param[2];

    int articolo;
    char articoloC[8];

    printf("\nInserisci l'id dell'annuncio da rimuovere: ");
    getInput(8, articoloC);

    articolo = atoi(articoloC);

    if (!setup_prepared_stmt(&prepared_stmt, "call eliminaAnnuncio(? , ?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize remove announcement statement\n", false);
    }

    memset(param, 0, sizeof(param));

    param[0].buffer_type = MYSQL_TYPE_LONG;
    param[0].buffer = &articolo;
    param[0].buffer_length = sizeof(articolo);

    param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[1].buffer = conf.username;
    param[1].buffer_length = strlen(conf.username);

    if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for remove announcement\n", true);
    }

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        print_stmt_error(prepared_stmt, "An error occurred while removing the announcement.");
    }
    else {
        printf("Annuncio rimosso correttamente...\n");
    }
    getchar();
    mysql_stmt_close(prepared_stmt);
}



static void segna_venduto(MYSQL* conn) {
    MYSQL_STMT* prepared_stmt;
    MYSQL_BIND param[2];

    char idAd[8];
    int idAnnuncio;
  
    printf("\nInserisci id annuncio: ");
    getInput(8, idAd);

    idAnnuncio = atoi(idAd);

    if (!setup_prepared_stmt(&prepared_stmt, "call segnaVenduto(?, ?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize mark as sold statement\n", false);
    }

    memset(param, 0, sizeof(param));

    param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[0].buffer = conf.username;
    param[0].buffer_length = strlen(conf.username);

    param[1].buffer_type = MYSQL_TYPE_LONG;
    param[1].buffer = &idAnnuncio;
    param[1].buffer_length = sizeof(idAnnuncio);

    if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for mark as sold\n", true);
    }

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        print_stmt_error(prepared_stmt, "An error occurred while marking the annuncement as sold.");
    }
    else {
        printf("Annuncio venduto correttamente...\n");
    }
    getchar();
    mysql_stmt_close(prepared_stmt);
}