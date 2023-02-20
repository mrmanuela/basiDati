#pragma once

#include <mysql.h>

struct configuration {
	char* host;
	char* db_username;
	char* db_password;
	unsigned int port;
	char* database;

	char username[128];
	char password[128];
};


extern struct configuration conf;

/*---------------------------------UTENTE----------------------------------*/
extern void run_as_utente(MYSQL* conn);
extern void utentePage(MYSQL* conn);
extern void aggiorna_utente_certificato(MYSQL* conn);

/*-----------------------UTILS UTENTE / UTENTE CERTIFICATO---------------*/
extern void visualizza_bacheca_pubblica(MYSQL* conn);
extern void segui_annuncio(MYSQL* conn);
extern void visualizza_contatto_venditore(MYSQL* conn);
extern void visualizza_commenti(MYSQL* conn);
extern void pubblica_commento(MYSQL* conn, int ad);
extern void contatta_venditore(MYSQL* conn);
extern void visualizza_annunci_seguiti(MYSQL* conn);
extern void visualizza_contatto_venditore_seguiti(MYSQL* conn);
extern void visualizza_commenti_seguiti(MYSQL* conn);
extern void pubblica_commento_seguiti(MYSQL* conn, int ad);
extern void contatta_venditore_seguiti(MYSQL* conn);
extern void visualizza_chat(MYSQL* conn);
extern void start_mex(MYSQL* conn);
extern void rispondi_conversazione(MYSQL* conn, char* user);
extern void gestisci_informazioni_personali(MYSQL* conn);
extern void visualizza_info_anagrafiche(MYSQL* conn);
extern void modifica_info_anagrafiche(MYSQL* conn);
extern void gestisci_contatti(MYSQL* conn);
extern void visualizza_contatti(MYSQL* conn);
extern void aggiungi_contatto(MYSQL* conn);
extern void elimina_contatto(MYSQL* conn);
extern void aggiorna_preferito(MYSQL* conn);


/*--------------------UTENTE CERTIFICATO----------------------*/
extern void creaAnnuncio(MYSQL* conn);
extern void rimuovi_annuncio(MYSQL* conn);
extern void visualizza_miei_annunci(MYSQL* conn);
extern void utenteCertificatoPage(MYSQL* conn);
extern void run_as_utenteCertificato(MYSQL* conn);
extern void segna_venduto(MYSQL* conn);
extern void visualizza_annunci(MYSQL* conn);

/*-------------------GESTORE-------------------------------*/
extern void visualizzaCategoria(MYSQL* conn);
extern void riscuoti_percentuale(MYSQL* conn);
extern void run_as_gestore(MYSQL* conn);
extern void gestorePage(MYSQL* conn);
extern void crea_categoria(MYSQL* conn);

/*----------------------INOUT----------------------------------*/
extern char* getInput(unsigned int lung, char* stringa);
extern char multiChoice(char* domanda, char choices[], int num);
extern int validateParams();
extern int getBirthDate(char* dateText, MYSQL_TIME* date);
extern int getExpirationDate(char* dateText, MYSQL_TIME* date);

/*-------------------PARSE-----------------------*/
extern char* strndup(const char* s, size_t n);
extern int parse_config(char* path, struct configuration* conf);

/*----------------------------UTILS----------------------------*/
extern void print_error(MYSQL* conn, char* message);
extern void print_stmt_error(MYSQL_STMT* stmt, char* message);
extern void finish_with_error(MYSQL* conn, char* message);
extern void finish_with_stmt_error(MYSQL* conn, MYSQL_STMT* stmt, char* message, bool close_stmt);
extern bool setup_prepared_stmt(MYSQL_STMT** stmt, char* statement, MYSQL* conn);
extern void dump_result_set(MYSQL* conn, MYSQL_STMT* stmt, char* title);
