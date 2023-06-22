#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "Definitions.h"
#include "FileManagment.h"
#include "StringManagment.h"

void writeToOutputFile(char* s, Context* context) { //Wypisuje ciag znakow do pliku wyjsciowego

	FILE* output_file;

	char temp[TOT][BUF];
	split(temp, context->inputFileName, ".", context);

	char output_name[BUF];

	strcat(temp[0], "_out.txt");
	strcpy(output_name, temp[0]);

	output_file = fopen(output_name, "w");
	fprintf(output_file, "%s\n", s);

	fclose(output_file);

}

void getInput(char ifn[TOT], Context* context) { // Czyta plik wejsciowy

	FILE* input_file;

	int i = 0;

	input_file = fopen(ifn, "r");
	char input[TOT][BUF];

	while (fgets(input[i], BUF, input_file)) {

		input[i][strlen(input[i])] = '\0';
		i++;
	}

	for (int j = 0;j < i;j++) {
		int p = 0;
		for (int k = 0;input[j][k] != '\0';k++) {
			context->input[j][k] = input[j][k];
			p = k;
		}
		context->input[j][p + 1] = '\0';

	}
	context->total = i;

	fclose(input_file);

}


char* getInputFilePath(int ac, char** av, Context* context) { // Czyta sciezke wzgledna do pliku wejsciowego oraz parametry wejsciowe

	char ifn[BUF];

	bool canProceed = true;

	if (ac < 2) canProceed = false;

	if (canProceed) {
		strcpy(ifn, av[1]);

		switch (ac) {
		case 2:
			context->psa_code = 1;
			context->debug = 0;
			break;
		case 3:
			if (strcmp(av[2], "psa_code") == 0) context->psa_code = 1;
			else if (strcmp(av[2], "msck_code") == 0) context->psa_code = 0;
			else {
				printf("Niepoprawnie wpisany drugi argument");
				exit(0);
			}
			context->debug = 0;
			break;
		case 4:
			if (strcmp(av[2], "psa_code") == 0) {
				context->psa_code = 1;
				if (strcmp(av[3], "debug") == 0) {
					context->debug = 1;
				}
				else {
					printf("Niepoprawnie wpisany trzeci argument");
					exit(0);
				}
			}
			else if (strcmp(av[2], "msck_code") == 0) {
				context->psa_code = 0;
				context->debug = 0;

			}
			else {
				printf("Niepoprawnie wpisany drugi argument");
				exit(0);
			}

			break;

		}

		FILE* pf;
		int errnum;
		pf = fopen(ifn, "r");

		if (pf != NULL) {
			canProceed = true;
			fclose(pf);
		}
		else {
			printf("Niepoprawna nazwa pliku wejsciowego!\n");
			canProceed = false;
		}
	}
	else {
		context->psa_code = 1;
		context->debug = 0;
	}

	while (!canProceed) {
		printf("Wprowadz nazwe pliku wejsciowego:\n");
		scanf("%s", &ifn);

		FILE* pf;
		pf = fopen(ifn, "r");

		if (pf != NULL) {
			canProceed = true;
			fclose(pf);
		}
		else {
			printf("Niepoprawna nazwa pliku wejsciowego!\n");
		}
	}

	strcpy(context->inputFileName, ifn);

	return ifn;

}
