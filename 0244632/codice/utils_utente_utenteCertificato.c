#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"




void visualizza_bacheca_pubblica(MYSQL* conn) {
    MYSQL_STMT* prepared_stmt;
    char op;
    char options[5] = { '1','2','3','4','5' };

    if (!setup_prepared_stmt(&prepared_stmt, "call visualizzaBachecaPubblica()", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize public notice board statement\n", false);
    }

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not retrieve the public notice board\n", true);
    }

    dump_result_set(conn, prepared_stmt, "\n***Bacheca pubblica degli Annunci***");
    mysql_stmt_close(prepared_stmt);
   
        printf("\nCosa vuoi fare?\n");
        printf("1) Segui annuncio\n");
        printf("2) Visualizza contatti del Venditore\n");
        printf("3) Visualizza commenti\n");
        printf("4) Invia messaggio al venditore\n");
        printf("5) Indietro");

        op = multiChoice("\n\nSeleziona un' opzione", options, 5);

        switch (op) {
        case '1':
            segui_annuncio(conn);
            break;
        case '2':
            visualizza_contatto_venditore(conn);
            break;
        case '3':
            visualizza_commenti(conn);
            break;
        case'4':
            contatta_venditore(conn);
            break;
        case'5':
            return;
        } 
    }


void segui_annuncio(MYSQL* conn) {
    MYSQL_STMT* prepared_stmt;
    MYSQL_BIND param[2];

    char annuncio[8];
    int annuncio1;

    printf("\nInserisci id dell' annuncio da seguire: ");
    getInput(8, annuncio);
    annuncio1 = atoi(annuncio);

    if (!setup_prepared_stmt(&prepared_stmt, "call segueAnnuncio(?,?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize follow annuncement statement\n", false);
    }

    memset(param, 0, sizeof(param));

    param[0].buffer_type = MYSQL_TYPE_LONG;
    param[0].buffer = &annuncio1;
    param[0].buffer_length = sizeof(annuncio);

    param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[1].buffer = conf.username;
    param[1].buffer_length = strlen(conf.username);

    if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for follow annuncement\n", true);
    }

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        print_stmt_error(prepared_stmt, "An error occurred while follow annuncement.");
    }
    else {
        printf("Annuncio seguito correttamente...\n");
    }
    getchar();
    mysql_stmt_close(prepared_stmt);
}


void visualizza_contatto_venditore(MYSQL* conn) {

    MYSQL_STMT* prepared_stmt;
    MYSQL_BIND param[1];
    char venditore[51];

    printf("\nInserisci il nome del venditore per conoscere i suoi contatti: ");
    getInput(50, venditore);

    if (!setup_prepared_stmt(&prepared_stmt, "call visualizzaContattiVenditore(?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize contacts board statement\n", false);
    }
    memset(param, 0, sizeof(param));

    param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[0].buffer = venditore;
    param[0].buffer_length = strlen(venditore);

    if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for contacts list\n", true);
    }
    if (mysql_stmt_execute(prepared_stmt) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not retrieve the contacts board\n", true);
    }

    dump_result_set(conn, prepared_stmt, "\n***Lista dei contatti***");
    printf("\nPress to continue...");
    getchar();
    mysql_stmt_close(prepared_stmt);
}



void visualizza_commenti(MYSQL* conn) {
    MYSQL_STMT* prepared_stmt;
    MYSQL_BIND param[1];

    char options[2] = { '1','2' };
    char op;
    char annuncio[8];
    int annuncio1;

    printf("\nInserisci l'id dell'annuncio per visualizzare i suoi commenti: ");
    getInput(8, annuncio);
    annuncio1 = atoi(annuncio);


    if (!setup_prepared_stmt(&prepared_stmt, "call visualizzaCommenti(?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize show comments statement\n", false);
    }

    memset(param, 0, sizeof(param));

    param[0].buffer_type = MYSQL_TYPE_LONG;
    param[0].buffer = &annuncio1;
    param[0].buffer_length = sizeof(annuncio);

    if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for show comments\n", true);
    }

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not retrieve annuncement's comments\n", true);
    }

    dump_result_set(conn, prepared_stmt, "\n***Commenti dell' annuncio***");
    mysql_stmt_close(prepared_stmt);

    printf("\n1)Pubblica un commento\n2)Indietro\n");

    op = multiChoice("\nSeleziona un' opzione", options, 2);

    switch (op) {
    case '1':
        pubblica_commento(conn, annuncio1);
        break;
    case '2':
        return;
    }
}


void pubblica_commento(MYSQL* conn, int ad) {
    MYSQL_STMT* prepared_stmt;
    MYSQL_BIND param[3];

    char commento[101];
    bool commentonull;

    printf("\nContenuto del commento: ");
    getInput(100, commento);

    if (commento[0] == '\0')
        commentonull = true;
    else
        commentonull = false;

    if (!setup_prepared_stmt(&prepared_stmt, "call creaCommento(?, ?, ?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize post comment statement\n", false);
    }

    memset(param, 0, sizeof(param));

    param[0].buffer_type = MYSQL_TYPE_LONG;
    param[0].buffer = &ad;
    param[0].buffer_length = sizeof(ad);

    param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[1].buffer = conf.username;
    param[1].buffer_length = strlen(conf.username);

    param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[2].buffer = commento;
    param[2].buffer_length = strlen(commento);
    param[2].is_null = &commentonull;

    if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for posting comment\n", true);
    }

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        print_stmt_error(prepared_stmt, "An error occurred while posting the comment.");
    }
    else {
        printf("Commento correttamente pubblicato...\n");
    }
    getchar();
    mysql_stmt_close(prepared_stmt);
}


void contatta_venditore(MYSQL* conn) {
    MYSQL_STMT* prepared_stmt;
    MYSQL_BIND param[3];

    char content[101];
    char utente2[51];
    bool contentnull;

    printf("\nInserisci il nome dell'utente da contattare: ");
    getInput(50, utente2);
    printf("\nScrivi messaggio: ");
    getInput(100, content);

    if (content[0] == '\0')
        contentnull = true;
    else
        contentnull = false;

    if (!setup_prepared_stmt(&prepared_stmt, "call inviaMessaggio(?,?,?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize send message statement\n", false);
    }

    memset(param, 0, sizeof(param));

    param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[0].buffer = conf.username;
    param[0].buffer_length = strlen(conf.username);

    param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[1].buffer = utente2;
    param[1].buffer_length = strlen(utente2);

    param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[2].buffer = content;
    param[2].buffer_length = strlen(content);
    param[2].is_null = &contentnull;

    if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for sending message\n", true);
    }

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        print_stmt_error(prepared_stmt, "An error occurred while sending message.");
    }
    else {
        printf("Messaggio inviato correttamente...\n");
    }
    getchar();
    mysql_stmt_close(prepared_stmt);
}



void visualizza_annunci_seguiti(MYSQL* conn) {

    MYSQL_BIND param[1];
    char options[4] = { '1','2','3','4' };
    char op;
    MYSQL_STMT* prepared_stmt;


    if (!setup_prepared_stmt(&prepared_stmt, "call visualizzaAnnunciSeguiti(?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize  follow annuncement statement\n", false);
    }
    memset(param, 0, sizeof(param));

    param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[0].buffer = conf.username;
    param[0].buffer_length = strlen(conf.username);

    if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for follow annuncement\n", true);
    } 

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not retrieve the show follow annuncement board\n", true);
    }

    dump_result_set(conn, prepared_stmt, "\n***Annunci da te seguiti***");
    mysql_stmt_close(prepared_stmt);

        printf("\nCosa vuoi fare?\n");
        printf("1) Visualizza contatti del Venditore\n");
        printf("2) Visualizza commenti\n");
        printf("3) Invia messaggio al venditore\n");
        printf("4) Indietro\n");

        op = multiChoice("Seleziona un' opzione", options, 4);

        switch (op) {
        case '1':
            visualizza_contatto_venditore_seguiti(conn);
            break;
        case '2':
            visualizza_commenti_seguiti(conn);
            break;
        case '3':
            contatta_venditore_seguiti(conn);
            break;
        case '4':
            return;

        }
}


void visualizza_contatto_venditore_seguiti(MYSQL* conn) {

    MYSQL_STMT* prepared_stmt;
    MYSQL_BIND param[1];
    char venditore[51];

    printf("\nInserisci il nome del venditore per conoscere i suoi contatti: ");
    getInput(50, venditore);

    if (!setup_prepared_stmt(&prepared_stmt, "call visualizzaContattiVenditoreSeguiti(?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize contacts board statement\n", false);
    }
    memset(param, 0, sizeof(param));

    param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[0].buffer = venditore;
    param[0].buffer_length = strlen(venditore);

    if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for contacts list\n", true);
    }
    if (mysql_stmt_execute(prepared_stmt) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not retrieve the contacts board\n", true);
    }

    dump_result_set(conn, prepared_stmt, "\n***Lista dei contatti***");
    printf("\nPress to continue...");
    getchar();
    mysql_stmt_close(prepared_stmt);
}



void visualizza_commenti_seguiti(MYSQL* conn) {
    MYSQL_STMT* prepared_stmt;
    MYSQL_BIND param[2];

    char options[2] = { '1','2' };
    char op;
    char annuncio[8];
    int annuncio1;

    printf("\nInserisci l'id dell'annuncio per visualizzare i suoi commenti: ");
    getInput(8, annuncio);
    annuncio1 = atoi(annuncio);

    if (!setup_prepared_stmt(&prepared_stmt, "call visualizzaCommentiSeguiti(?,?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize show comments statement\n", false);
    }

    memset(param, 0, sizeof(param));

    param[0].buffer_type = MYSQL_TYPE_LONG;
    param[0].buffer = &annuncio1;
    param[0].buffer_length = sizeof(annuncio);

    param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[1].buffer = conf.username;
    param[1].buffer_length = strlen(conf.username);


    if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for show comments\n", true);
    }

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not retrieve annuncement's comments\n", true);
    }

    dump_result_set(conn, prepared_stmt, "\n***Commenti dell' annuncio***");
    mysql_stmt_close(prepared_stmt);

    printf("\n1)Pubblica un commento\n2)Indietro\n");

    op = multiChoice("\nSeleziona un' opzione", options, 2);

    switch (op) {
    case '1':
        pubblica_commento_seguiti(conn, annuncio1);
        break;
    case '2':
        return;

    }
}


void pubblica_commento_seguiti(MYSQL* conn, int ad) {
    MYSQL_STMT* prepared_stmt;
    MYSQL_BIND param[3];

    char commento[101];
    bool commentonull;

    printf("\nContenuto del commento: ");
    getInput(100, commento);

    if (commento[0] == '\0')
        commentonull = true;
    else
        commentonull = false;

    if (!setup_prepared_stmt(&prepared_stmt, "call creaCommentoSeguiti(?, ?, ?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize post comment statement\n", false);
    }

    memset(param, 0, sizeof(param));

    param[0].buffer_type = MYSQL_TYPE_LONG;
    param[0].buffer = &ad;
    param[0].buffer_length = sizeof(ad);

    param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[1].buffer = conf.username;
    param[1].buffer_length = strlen(conf.username);

    param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[2].buffer = commento;
    param[2].buffer_length = strlen(commento);
    param[2].is_null = &commentonull;

    if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for posting comment\n", true);
    }

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        print_stmt_error(prepared_stmt, "An error occurred while posting the comment.");
    }
    else {
        printf("Commento correttamente pubblicato...\n");
    }
    getchar();
    mysql_stmt_close(prepared_stmt);
}


void contatta_venditore_seguiti(MYSQL* conn) {
    MYSQL_STMT* prepared_stmt;
    MYSQL_BIND param[3];

    char content[101];
    char utente2[51];
    bool contentnull;

    printf("\nInserisci il nome dell'utente da contattare: ");
    getInput(50, utente2);
    printf("\nScrivi messaggio: ");
    getInput(100, content);

    if (content[0] == '\0')
        contentnull = true;
    else
        contentnull = false;

    if (!setup_prepared_stmt(&prepared_stmt, "call inviaMessaggioSegui(?,?,?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize send message statement\n", false);
    }

    memset(param, 0, sizeof(param));

    param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[0].buffer = conf.username;
    param[0].buffer_length = strlen(conf.username);

    param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[1].buffer = utente2;
    param[1].buffer_length = strlen(utente2);

    param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[2].buffer = content;
    param[2].buffer_length = strlen(content);
    param[2].is_null = &contentnull;

    if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for sending message\n", true);
    }

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        print_stmt_error(prepared_stmt, "An error occurred while sending message.");
    }
    else {
        printf("Messaggio inviato correttamente...\n");
    }
    getchar();
    mysql_stmt_close(prepared_stmt);
}


void visualizza_chat(MYSQL* conn) {
    MYSQL_STMT* prepared_stmt;
    MYSQL_BIND param[2];

    char options[2] = { '1','2' };
    char op;
    int conversazione1 = 0;

    if (!setup_prepared_stmt(&prepared_stmt, "call visualizzaChatInCorso(?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize show chats statement\n", false);
    }

    memset(param, 0, sizeof(param));

    param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[0].buffer = conf.username;
    param[0].buffer_length = strlen(conf.username);

    if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for chats list\n", true);
    }

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not retrieve chats list\n", true);
    }

    dump_result_set(conn, prepared_stmt, "\n***Lista delle conversazioni***");
    mysql_stmt_close(prepared_stmt);

    printf("\nCosa vuoi fare?\n");
    printf("1) Visualizza conversazione\n");
    printf("2) Indietro");

    op = multiChoice("\n\nSeleziona un' opzione", options, 2);

    switch (op) {
    case '1':
        start_mex(conn);
        break;
    case '2':
        return;
    }
}


void start_mex(MYSQL* conn) {
    MYSQL_STMT* prepared_stmt;
    MYSQL_BIND param[2];

    char options[2] = { 'y','n' };
    char username[51];
    char op;
    bool nomenull;

    printf("\nInserisci il nome dell'utente per visualizzare l'intera conversazione: ");
    getInput(50, username);


    if (!setup_prepared_stmt(&prepared_stmt, "call visualizzaChatIntera(?,?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize show chats statement\n", false);
    }

    memset(param, 0, sizeof(param));

    param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[0].buffer = username;
    param[0].buffer_length = strlen(username);
    
    param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[1].buffer = conf.username;
    param[1].buffer_length = strlen(conf.username);

    if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for messages list\n", true);
    }

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not retrieve messages\n", true);
    }

    dump_result_set(conn, prepared_stmt, "\n***La conversazione***");
    mysql_stmt_close(prepared_stmt);

    op = multiChoice("\n\nVuoi rispondere alla conversazione?", options, 2);

    switch (op) {
    case 'y':
        rispondi_conversazione(conn, username);
        break;
    case 'n':
        return;
    }
}


void rispondi_conversazione(MYSQL* conn, char* user) {
    MYSQL_STMT* prepared_stmt;
    MYSQL_BIND param[3];

    char content[101];
    bool contentnull;

    printf("\nScrivi messaggio: ");
    getInput(100, content);

    if (content[0] == '\0')
        contentnull = true;
    else
        contentnull = false;

    if (!setup_prepared_stmt(&prepared_stmt, "call rispondiConversazione(?,?,?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize send message statement\n", false);
    }

    memset(param, 0, sizeof(param));

    param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[0].buffer = conf.username;
    param[0].buffer_length = strlen(conf.username);

    param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[1].buffer = user;
    param[1].buffer_length = strlen(user);

    param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[2].buffer = content;
    param[2].buffer_length = strlen(content);
    param[2].is_null = &contentnull;

    if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for sending message\n", true);
    }

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        print_stmt_error(prepared_stmt, "An error occurred while sending message.");
    }
    else {
        printf("Messaggio inviato correttamente...\n");
    }
    getchar();
    mysql_stmt_close(prepared_stmt);
}


void gestisci_informazioni_personali(MYSQL* conn) {
    char options[3] = { '1','2','3' };
    char op;

    while (true) {
        printf("\033[2J\033[H");
        printf("Cosa vuoi fare?\n");
        printf("1) Visualizza informazioni anagrafiche\n");
        printf("2) Modifica informazioni anagrafiche\n");
        printf("3) Indietro\n");

        op = multiChoice("\nSeleziona un' opzione", options, 3);

        switch (op) {
        case'1':
            visualizza_info_anagrafiche(conn);
            break;
        case '2':
            visualizza_info_anagrafiche(conn);
            modifica_info_anagrafiche(conn);
            break;
        case '3':
            return;
            getchar();
        }
    }
}


void visualizza_info_anagrafiche(MYSQL* conn) {

    MYSQL_STMT* prepared_stmt;
    MYSQL_BIND param[1];

    if (!setup_prepared_stmt(&prepared_stmt, "call visualizzaInfoAnagrafiche(?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize show personal information statement\n", false);
    }

    memset(param, 0, sizeof(param));

    param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[0].buffer = conf.username;
    param[0].buffer_length = strlen(conf.username);

    if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for personal information list\n", true);
    }

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not retrieve personal information list\n", true);
    }

    dump_result_set(conn, prepared_stmt, "\n***Informazioni anagrafiche***");
    getchar();
    mysql_stmt_close(prepared_stmt);
}


void modifica_info_anagrafiche(MYSQL* conn) {
    MYSQL_STMT* prepared_stmt;
    MYSQL_BIND param[7];

    char codFisc[17];
    char nome[51];
    char cognome[51];
    char nascita[11];
    char residenza[51];
    char fatturazione[51];
    MYSQL_TIME* date = NULL;
    char datanull;


modifica:
    printf("\nNuovo nome: ");
    getInput(50, nome);
    printf("Nuovo cognome: ");
    getInput(50, cognome);
    printf("Nuovo codice fiscale: ");
    getInput(17, codFisc);
    printf("Nuova data di nascita (gg-mm-aaaa): ");
    getInput(10, nascita);
    printf("Nuovo indirizzo di residenza: ");
    getInput(50, residenza);
    printf("Nuovo indirizzo di fatturazione: ");
    getInput(50, fatturazione);

    switch (validateParams()) {
    case EXIT_SUCCESS:
        break;
    case EXIT_FAILURE:
        goto modifica;
    }

    if (nascita[0] == '\0')
        datanull = true;
    else {
        datanull = false;
        date = malloc(sizeof(MYSQL_TIME*));
        date->time_type = MYSQL_TIMESTAMP_DATE;


        switch (getBirthDate(nascita, date)) {
        case EXIT_SUCCESS:
            break;
        case EXIT_FAILURE:
            goto modifica;
        }
    }

    if (!setup_prepared_stmt(&prepared_stmt, "call aggiornaInfoAnagrafiche(?, ?, ?, ?, ?, ?, ?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize changing personal information statement\n", false);
    }

    memset(param, 0, sizeof(param));

    param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[0].buffer = nome;
    param[0].buffer_length = strlen(nome);

    param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[1].buffer = cognome;
    param[1].buffer_length = strlen(cognome);

    param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[2].buffer = codFisc;
    param[2].buffer_length = strlen(codFisc);

    param[3].buffer_type = MYSQL_TYPE_DATE;
    param[3].buffer = date;
    param[3].buffer_length = sizeof(date);
    param[3].is_null = &datanull;

    param[4].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[4].buffer = residenza;
    param[4].buffer_length = strlen(residenza);

    param[5].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[5].buffer = fatturazione;
    param[5].buffer_length = strlen(fatturazione);

    param[6].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[6].buffer = conf.username;
    param[6].buffer_length = strlen(conf.username);

    if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for changing information\n", true);
    }

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        print_stmt_error(prepared_stmt, "An error occurred while changing the informations.");
    }
    else {
        printf("Informazioni personali modificate correttamente...\n");
    }
    mysql_stmt_close(prepared_stmt);
}


void gestisci_contatti(MYSQL* conn) {
    char options[5] = { '1','2','3','4','5' };
    char op;

    while (true) {
        printf("\033[2J\033[H");
        printf("Cosa vuoi fare?\n");
        printf("1) Visualizza contatti\n");
        printf("2) Crea contatto\n");
        printf("3) Aggiorna contatto a preferito\n");
        printf("4) Cancella contatto\n");
        printf("5) Indietro\n");

        op = multiChoice("\nSeleziona un' opzione", options, 5);

        switch (op) {
        case '1':
            visualizza_contatti(conn);
            break;
        case '2':
        {
            visualizza_contatti(conn);
            aggiungi_contatto(conn);
        }
        break;
        case '3':
        {
            visualizza_contatti(conn);
            aggiorna_preferito(conn);
        }
        break;
        case '4':
        {
            visualizza_contatti(conn);
            elimina_contatto(conn);
        }
        case '5':
            return;
        }
        getchar();
    }
}


void visualizza_contatti(MYSQL* conn) {
    MYSQL_STMT* prepared_stmt;
    MYSQL_BIND param[1];

    if (!setup_prepared_stmt(&prepared_stmt, "call visualizzaContatti(?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize show contacts list statement\n", false);
    }

    memset(param, 0, sizeof(param));

    param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[0].buffer = conf.username;
    param[0].buffer_length = strlen(conf.username);

    if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for contacts list\n", true);
    }

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not retrieve contact list\n", true);
    }

    dump_result_set(conn, prepared_stmt, "\n***Lista contatti***");
    mysql_stmt_close(prepared_stmt);
}


void aggiungi_contatto(MYSQL* conn) {
    MYSQL_STMT* prepared_stmt;
    MYSQL_BIND param[4];

    char options[3] = { '1','2','3' };
    char optionsf[2] = { 'y','n' };
    char op;
    char contatto[51];
    bool contattonull;
    char tipo[10];
    char preferito;

    printf("\n\nInserisci le informazioni del tuo contatto\n");
    printf("\nRecapito: ");
    getInput(51, contatto);

    if (contatto[0] == '\0')
        contattonull = true;
    else
        contattonull = false;

    if (strcmp(contatto, "") == 0)return;

    printf("\nChe tipologia di contatto stai aggiungendo?\n1)Telefono fisso\n2)Cellulare\n3)Email\n");

    op = multiChoice("Seleziona un' opzione", options, 3);

    switch (op) {
    case '1':
        strcpy(tipo, "Fisso");
        break;
    case '2':
        strcpy(tipo, "Cellulare");
        break;
    case '3':
        strcpy(tipo, "Email");
        break;
    }

    op = multiChoice("\nVuoi aggiungere il contatto tra i preferiti?", optionsf, 2);

    switch (op) {
    case 'y':
        preferito = 1;
        break;
    case 'n':
        preferito = 0;
        break;
    }

    if (!setup_prepared_stmt(&prepared_stmt, "call creaContatto(?, ?, ?, ?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize contact list statement\n", false);
    }

    memset(param, 0, sizeof(param));

    param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[0].buffer = conf.username;
    param[0].buffer_length = strlen(conf.username);

    param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[1].buffer = contatto;
    param[1].buffer_length = strlen(contatto);
    param[1].is_null = &contattonull;

    param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[2].buffer = tipo;
    param[2].buffer_length = strlen(tipo);

    param[3].buffer_type = MYSQL_TYPE_TINY;
    param[3].buffer = &preferito;
    param[3].buffer_length = sizeof(preferito);

    if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for contact insertion\n", true);
    }
    if (mysql_stmt_execute(prepared_stmt) != 0) {
        print_stmt_error(prepared_stmt, "An error occurred while adding the contact."); 
    }
    else {
        printf("Contatto correttamente aggiunto...\n");
    }
    mysql_stmt_close(prepared_stmt);
}


void elimina_contatto(MYSQL* conn) {
    MYSQL_STMT* prepared_stmt;
    MYSQL_BIND param[2];

    char toDelete[8];

    printf("\nScrivi il contatto da eliminare: ");
    getInput(8, toDelete);

    if (!setup_prepared_stmt(&prepared_stmt, "call eliminaContatto(?, ?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize delete contacts statement\n", false);
    }

    memset(param, 0, sizeof(param));

    param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[0].buffer = conf.username;
    param[0].buffer_length = strlen(conf.username);

    param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[1].buffer = toDelete;
    param[1].buffer_length = strlen(toDelete);

    if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for delete contacts.\n", true);
    }

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        print_stmt_error(prepared_stmt, "An error occurred while  delete contacts.");
    }
    else {
        printf("Contatto eliminato correttamente...\n");
    }
    getchar();
    mysql_stmt_close(prepared_stmt);
}


void aggiorna_preferito(MYSQL* conn) {
    MYSQL_STMT* prepared_stmt;
    MYSQL_BIND param[2];

    char newFav[51];

    aggiorna:
    printf("\nInserisci il contatto da rendere preferito: ");
    getInput(50, newFav);

    switch (validateParams()) {
    case EXIT_SUCCESS:
        break;
    case EXIT_FAILURE:
        goto aggiorna;
    }

    if (!setup_prepared_stmt(&prepared_stmt, "call aggiornaContattoPreferito(?, ?)", conn)) {
        finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize update favourite contacts statement\n", false);
    }

    memset(param, 0, sizeof(param));

    param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[0].buffer = newFav;
    param[0].buffer_length = strlen(newFav);
    
    param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
    param[1].buffer = conf.username;
    param[1].buffer_length = strlen(conf.username);

    if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
        finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for update favourite contacts\n", true);
    }

    if (mysql_stmt_execute(prepared_stmt) != 0) {
        print_stmt_error(prepared_stmt, "An error occurred while updating the favourite contact.");
    }
    else {
        printf("Contatto aggiornato correttamente a preferito...\n");
    }
    mysql_stmt_close(prepared_stmt);
}



