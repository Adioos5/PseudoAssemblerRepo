#include <stdio.h>
#include <stdbool.h>
#include "Definitions.h"
#include "ConsoleManagment.h"
#include "StringManagment.h"

void printDataSection(Context* context);
void printOrdersSection(int idx);
void printRegisters(Context* context);

void setConsoleColour(WORD* Attributes, DWORD Colour) { // Ustawia nowy kolor tekstu w konsoli
	CONSOLE_SCREEN_BUFFER_INFO Info;
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hStdout, &Info);
	*Attributes = Info.wAttributes;
	SetConsoleTextAttribute(hStdout, Colour);
}

void resetConsoleColour(WORD Attributes) { // Resetuje kolor tekstu w konsoli do koloru domyslnego
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), Attributes);
}

void clrscr() { // Wyczyszcza zawartosc konsoli
	system("@cls||clear");
}

void refreshScreen(int idx, Context* context) { // Odswieza zawartosc konsoli

	clrscr();

	WORD Attributes = 0;

	printDataSection(context);
	printOrdersSection(idx, context);

	setConsoleColour(&Attributes, (FOREGROUND_RED));
	printRegisters(context);

	resetConsoleColour(Attributes);

	setConsoleColour(&Attributes, (FOREGROUND_GREEN));
	printf("\nNastepny krok[p] | Wyjdz i zapisz aktualny stan symulacji[s] | Wyjdz bez zapisywania[q]\n");

	resetConsoleColour(Attributes);

}

void printDataSection(Context* context) { // Wypisuje na konsoli sekcje danych

	WORD Attributes = 0;

	printf("			    SEKCJA DANYCH:\n");
	printf("%16s", "Etykieta:");
	printf("%36s", "Zawartosc komorki pamieci (HEX):");
	printf("\n\n");

	int b = 0;
	for (int a = 0;a < context->d_idx;a++) {
		char tempStr[TOT][BUF];
		
		split(tempStr, context->dataSection[b], "\n", context);
		
		printf("%16s", context->labelsData[b]);

		if (a == context->dToChange) {
			setConsoleColour(&Attributes, (FOREGROUND_BLUE));
			printf("%36s\n", context->dataSectionHEX[a]);
			resetConsoleColour(Attributes);
			context->dToChange = -1;
		}
		else {
			printf("%36s\n", context->dataSectionHEX[a]);
		}
		

		if (strcmp(tempStr[1], "DS") == 0) {

			if (contains(tempStr[2], '*')) {
			
				char multiplier[BUF] = "";
				strcat(multiplier, readUntil(tempStr[2], '*'));
				int n = atoi(multiplier);

				for (int h = 1;h < n;h++) {
					a += 1;

					if (a == context->dToChange) {
						setConsoleColour(&Attributes, (FOREGROUND_BLUE));
						printf("%52s\n", context->dataSectionHEX[a]);
						resetConsoleColour(Attributes);
						context->dToChange = -1;
					}
					else {
						printf("%52s\n", context->dataSectionHEX[a]);
					}
				}

			}
		}

		b += 1;
	}
	printf("\n");
}

void printOrdersSection(int idx, Context* context) { // Wypisuje na konsoli sekcje rozkazow


	printf("			   SEKCJA ROZKAZOW:\n");
	printf("%16s", "Etykieta:");
	printf("%16s", "Kod rozkazu:");
	printf("%16s", "Pierwszy arg.:");
	printf("%16s", "Drugi arg.:");


	printf("\n\n");

	char str[TOT][BUF] = { "" };
	char line[BUF] = "";

	for (int j = 0;j < context->o_idx;j++) {

		strcpy(line, context->ordersSection[j]);
		split(str, line, "\n", context);

		if (context->lastSplitLength == 2) {
			printf("%16s", " ");
		}

		if (context->lastSplitLength == 3) {
			printf("%16s", " ");
		}

		int k;

		for (k = 0;k < context->lastSplitLength;k++) {
			printf("%16s", str[k]);
		}

		if (context->lastSplitLength == 2) {
			printf("%16s", " ");
		}

		if (j == idx) printf("%5s", "<");

		printf("\n");
	}
}

void printRegisters(Context* context) { // Wypisuje na konsoli zawartosc rejestrow

	WORD Attributes = 0;

	printf("\n			       ZAWARTOSC REJESTROW:\n");

	int w;
	for (w = 0;w < 15;w+=3) {
		if (w == context->rToChange) {
			setConsoleColour(&Attributes, (FOREGROUND_BLUE));
			printf("%2d.[%10d] ", w + 1, context->registers[w]);
			resetConsoleColour(Attributes);
			setConsoleColour(&Attributes, (FOREGROUND_RED));
			context->rToChange = -1;
		}
		else {
			printf("%2d.[%10d] ", w+1, context->registers[w]);

		}
	}
	printf("\n");
	for (w = 1;w < 15;w += 3) {
		if (w == context->rToChange) {
			setConsoleColour(&Attributes, (FOREGROUND_BLUE));
			printf("%2d.[%10d] ", w + 1, context->registers[w]);
			resetConsoleColour(Attributes);
			setConsoleColour(&Attributes, (FOREGROUND_RED));
			context->rToChange = -1;
		}
		else {
			printf("%2d.[%10d] ", w + 1, context->registers[w]);

		}
	}
	printf("\n");
	for (w = 2;w < 15;w += 3) {
		if (w == context->rToChange) {
			setConsoleColour(&Attributes, (FOREGROUND_BLUE));
			printf("%2d.[%10d] ", w + 1, context->registers[w]);
			resetConsoleColour(Attributes);
			setConsoleColour(&Attributes, (FOREGROUND_RED));
			context->rToChange = -1;
		}
		else {
			printf("%2d.[%10d] ", w + 1, context->registers[w]);

		}
	}
	printf("\n");
}
