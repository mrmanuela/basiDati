#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "defines.h"

char* getInput(unsigned int lung, char* stringa) {
	char c;
	unsigned int i;

	// Acquisisce da tastiera al più lung - 1 caratteri
	for (i = 0; i < lung; i++) {
		fread(&c, sizeof(char), 1, stdin);
		if (c == '\n') {
			stringa[i] = '\0';
			break;
		}
		else
			stringa[i] = c;
	}
	// Controlla che il terminatore di stringa sia stato inserito
	if (i == lung - 1)
		stringa[i] = '\0';
	// Se sono stati digitati più caratteri, svuota il buffer della tastiera
	if (strlen(stringa) >= lung) {
		// Svuota il buffer della tastiera
		do {
			c = getchar();
		} while (c != '\n');
	}
	return stringa;
}

char multiChoice(char* domanda, char choices[], int num) {

	// Genera la stringa delle possibilità
	char* possib = malloc(2 * num * sizeof(char));
	int i, j = 0;
	for (i = 0; i < num; i++) {
		possib[j++] = choices[i];
		possib[j++] = '/';
	}
	possib[j - 1] = '\0'; // Per eliminare l'ultima '/'

	// Chiede la risposta
	while (true) {
		// Mostra la domanda
		printf("%s [%s]: ", domanda, possib);

		char c;
		getInput(1, &c);

		// Controlla se è un carattere valido
		for (i = 0; i < num; i++) {
			if (c == choices[i])
				return c;
		}
	}
}

// Accepts the inputted parameters
int validateParams() {
	char choices[2] = { 's', 'n' };
	char res;
	res = multiChoice("\nSei sicuro di voler confermare questi parametri?", choices, 2);
	switch (res) {
	case 's':
		return EXIT_SUCCESS;
	case 'n':
		return EXIT_FAILURE;
	}
}

// Write the birth date from a string in the MYSQL_TIME struct
int getBirthDate(char* dateText, MYSQL_TIME* date) {

	char* end;
	unsigned int d[3];

	// Extracts the day, month and year
	for (int i = 0; i < 3; i++) {
		d[i] = strtoul(dateText, &end, 10);
		dateText = end + 1;
		if (d[i] == 0)
			return EXIT_FAILURE;
	}

	// Check for out of bounds values
	if (d[0] >= 32 || d[0] == 0)
		return EXIT_FAILURE;
	if (d[1] >= 13 || d[1] == 0)
		return EXIT_FAILURE;
	if (d[2] >= 10000 || d[2] == 0)
		return EXIT_FAILURE;

	date->day = d[0];
	date->month = d[1];
	date->year = d[2];

	return EXIT_SUCCESS;
}

// Write the expiration date of a credit card from a string in the MYSQL_TIME struct
int getExpirationDate(char* dateText, MYSQL_TIME* date) {

	char* end;
	unsigned int d[2];

	// Extracts the day, month and year
	for (int i = 0; i < 2; i++) {
		d[i] = strtoul(dateText, &end, 10);
		dateText = end + 1;
		if (d[i] == 0)
			return EXIT_FAILURE;
	}

	if (d[0] >= 13 || d[0] == 0)
		return EXIT_FAILURE;
	if (d[1] >= 10000 || d[1] == 0)
		return EXIT_FAILURE;

	date->day = 31;
	date->month = d[0];
	if (d[1] >= 2000)
		date->year = d[1];
	else
		date->year = d[1] + 2000;

	// Sets the maximum number of days based on the month
	switch (d[0]) {
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
		date->day = 31;
	case 4:
	case 6:
	case 9:
	case 11:
		date->day = 31;
	case 2:
		if (date->year % 4 == 0)
			date->day = 29;
		else
			date->day = 28;
	}

	return EXIT_SUCCESS;
}