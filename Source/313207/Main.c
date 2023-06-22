#define _CRT_SECURE_NO_DEPRECATE

#define BUF 1050
#define TOT 80
#define ORDERS_LENGTH 18
#define DATA_ADDRESS_REGISTER_NUMBER 15
#define ORDERS_ADDRESS_REGISTER_NUMBER 14

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

int total = 64;

char delimit[] = " \r\n\t,";
char input[TOT][BUF];
char orders[TOT][BUF] = { "AR", "A",  "SR", "S",  "MR", "M",  "DR", "D",  "CR", "C" ,  "J" ,  "JZ" , "JP" , "JN" , "L",  "LR" , "LA" , "ST" };
char ordersHEX[TOT][BUF] = { "10", "D1", "12", "D3", "14", "D5", "16", "D7", "18", "D9" , "E0" , "E1" , "E2" , "E3" , "F0", "31" , "F2" , "F3" };

char dataSection[TOT][BUF];
char ordersSection[TOT][BUF];

char dataSectionHEX[TOT][BUF];
char ordersSectionHEX[TOT][BUF];

int ordersLength;
int dataLength;

char dataOutput[BUF] = "";
char ordersOutput[BUF] = "";

int currentDataSectionOffset = 0;
int currentOrdersSectionOffset = 0;
int currentLODidx = 0;
int currentLOOidx = 0;

char labelsData[TOT][BUF];
char labelsOrders[TOT][BUF];

int offsetsData[TOT];
int offsetsOrders[TOT];

char input_name[BUF];

int dataLabelsLength = 0;
int ordersLabelsLength = 0;

void writeToOutputFile(char* s, int times);
void addOffsetFromData();
void addOffsetFromOrders();
void addLabelFromData(char str[BUF]);
void addLabelFromOrders(char str[BUF]);
void append_shift(char* str, char* shiftToHex);
void decodeData(char* str);
void decodeOrder(char* str, int idx);
void addAllLabels(char dataSection[TOT][BUF], char ordersSection[TOT][BUF], int d_length, int o_length);
void addAllOffsets(char dataSection[TOT][BUF], char ordersSection[TOT][BUF], int d_length, int o_length);
void refreshSections();
void readInput();
void getInput(char ifn[TOT]);

int findIndex(char a[TOT][BUF], int size, char value[BUF]);
int idx_of_char_in_str(char str[BUF], char c);

char* reverseString(char str[BUF]);
char* readUntil(char str[BUF], char c);
char* readInRange(char str[BUF], int p, int q);

bool isBlank(char* str);
bool contains(char str[BUF], char c);
bool contains_T(static char strings[TOT][BUF], char str[BUF], int len);

int main(int ac, char** av) {


	char ifn[BUF];

	bool canProceed = true;

	if (ac < 2) canProceed = false;

	if (canProceed) {
		strcpy(ifn, av[1]);

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

	strcpy(input_name, ifn);

	getInput(ifn);
	readInput();


	strcat(dataOutput, "\n");
	strcat(dataOutput, ordersOutput);

	writeToOutputFile(dataOutput, 1);

	return 0;

}



void writeToOutputFile(char* s, int times) { //Wypisuje ciag znakow do pliku output.txt

	FILE* output_file;

	char temp[TOT][BUF];
	char* token = strtok(input_name, "."); // Podzial linii na wyrazy wzgledem znakow bialych
	int i = 0;

	while (token != NULL) {
		strcpy(temp[i], token);
		token = strtok(NULL, delimit);
		i += 1;
	}

	char output_name[BUF];
	
	strcat(temp, "_out.txt");
	strcpy(output_name, temp);

	output_file = fopen(output_name, "w");

	
	for (i = 0; i < times;i++) {
		fprintf(output_file, "%s\n", s);
	}

	fclose(output_file);

}

bool isBlank(char* str) { //Sprawdza czy linia nie jest pusta (czy nie jest pelna spacji)
	while (*str) {
		if (!isspace(*str++)) {
			return false;
		}
	}
	return true;
}

void addOffsetFromData() { //Dodaje przesuniecie dla etykiety danej

	offsetsData[currentLODidx] = currentDataSectionOffset;
	currentLODidx += 1;
}

void addOffsetFromOrders() { //Dodaje przesuniecie dla etykiety rozkazu

	offsetsOrders[currentLOOidx] = currentOrdersSectionOffset;
	currentLOOidx += 1;
}

void addLabelFromData(char str[BUF]) { //Dodaje etykiete danej do listy etykiet danych

	strcpy(labelsData[currentLODidx], str);
	dataLabelsLength += 1;
	currentLODidx += 1;
}

void addLabelFromOrders(char str[BUF]) { //Dodaje etykiete rozkazu do listy etykiet rozkazow
	strcpy(labelsOrders[currentLOOidx], str);
	ordersLabelsLength += 1;
	currentLOOidx += 1;
}


int findIndex(char a[TOT][BUF], int size, char value[BUF]) { // Szuka indeksu ciagu znakow w tablicy ciagow znakowych

	int j = 0;
	while (j < size) {

		if (strcmp(a[j], value) == 0) return j;
		j += 1;
	}

	return -1;

}

int idx_of_char_in_str(char str[BUF], char c) { // Szuka indeksu znaku w ciagu znakow

	const char* ptr = strchr(str, c);
	if (ptr) {
		int index = ptr - str;
		return index;
	}

	return -1;
}

bool contains(char str[BUF], char c) { // Sprawdza czy ciag znakow zawiera okreslony znak
	size_t len = strlen(str);

	int i;
	for (i = 0; i < len;i++) {
		if (str[i] == c) return true;
	}

	return false;
}

bool contains_T(static char strings[TOT][BUF], char str[BUF], int len) { // Sprawdza czy tablica ciagow znakowych zawiera okreslony ciag znakow
	size_t i;
	for (i = 0; i < len; i++)
	{
		if (strcmp(strings[i], str) == 0) return true;
	}

	return false;
}

char* reverseString(char str[BUF]) { // Odwraca kolejnosc znakow w ciagu znakowym
	static char rstr[BUF];

	int i;
	for (i = 0;i < strlen(str);i++) {
		rstr[i] = str[strlen(str) - i - 1];
	}

	return rstr;

}

char* readUntil(char str[BUF], char c) { // Zwraca ciag znakow przeczytany do pierwszego napotkania okreslonego znaku
	static char r[BUF] = "";
	strcpy(r, "");

	int i;
	for (i = 0;i < strlen(str);i++) {
		if (str[i] == c) break;
		strncat(r, &str[i], 1);

	}

	return r;
}

char* readInRange(char str[BUF], int p, int q) { // Zwraca ciag znakow ze znakami z okreslonego przedzialu indeksow

	char temp[BUF];
	static char r[BUF] = "";
	strcpy(r, "");
	strcpy(temp, str);

	int i;
	for (i = p;i < q;i++) {
		strncat(r, &temp[i], 1);
	}


	return r;
}

void append_shift(char* str, char* shiftToHex) { // Dodaje przesuniecie do zdekodowanej linii

	switch (strlen(shiftToHex)) {	// Switch zalezny od dlugosci znakow przesuniecia w notacji szesnastkowej
	case 4:
		strcat(str, readInRange(shiftToHex, 1, 3));
		strcat(str, " ");
		strcat(str, readInRange(shiftToHex, 3, 5));
		break;
	case 3:
		strcat(str, "0");
		strcat(str, readInRange(shiftToHex, 1, 2));
		strcat(str, " ");
		strcat(str, readInRange(shiftToHex, 2, 4));
		break;
	case 2:
		strcat(str, "00");
		strcat(str, " ");
		strcat(str, shiftToHex);
		break;
	case 1:
		strcat(str, "00");
		strcat(str, " ");
		strcat(str, "0");
		strcat(str, shiftToHex);
		break;
	default:
		;
	}

}

void decodeData(char* str) { // dekoduje jedna linie danych
	bool hasLabel = true;
	char dataElements[TOT][BUF];


	if (str[0] == ' ' || str[0] == '\t') hasLabel = false; // Czy ma etykiete

	char* token = strtok(str, delimit); // Podzial linii na wyrazy wzgledem znakow bialych
	int i = 0;

	while (token != NULL) {
		strcpy(dataElements[i], token);
		token = strtok(NULL, delimit);
		i += 1;
	}
	int dataLength = i;

	int j;	//Chodzi tu o uzyskanie dostepu do komendy danych
	if (hasLabel) j = 1;
	else j = 0;

	char temp[BUF] = "";
	char num[BUF] = "";

	int lastIdx = 0;

	if (strcmp(dataElements[j], "DC") == 0) { // dekodowanie w przypadku komendy DC

		j += 1;
		lastIdx = j;
		strcat(num, readInRange(dataElements[j], idx_of_char_in_str(dataElements[j], '(') + 1, idx_of_char_in_str(dataElements[j], ')')));

		char hex[BUF] = "";
		sprintf(hex, "%X", atoi(num));
		strcat(temp, hex);


		char decodedString[BUF] = "";

		int len = strlen(temp);
		int zeros = 8 - len;
		int j = 0;

		int i;
		for (i = 0;i < 11;i++) {

			if ((i + 1) % 3 == 0) {
				strcat(decodedString, " ");
				continue;
			}

			if (zeros > 0) {
				strcat(decodedString, "0");
				zeros -= 1;
				continue;
			}

			strncat(decodedString, &temp[j], 1);
			j += 1;
		}

		strcat(decodedString, "\n");

		if (contains(dataElements[lastIdx], '*')) {
			char multiplier[BUF] = "";
			strcat(multiplier, readUntil(dataElements[lastIdx], '*'));
			int n = atoi(multiplier);

			char temp[BUF];
			strcpy(temp, decodedString);
			for (int i = 0;i < n - 1;i++) {

				strcat(decodedString, temp);
			}

		}

		strcat(dataOutput, decodedString);

	}
	else if (strcmp(dataElements[j], "DS") == 0) { // dekodowanie w przypadku komendy DS
		j += 1;

		if (contains(dataElements[j], '*')) {

			char multiplier[BUF] = "";
			strcat(multiplier, readUntil(dataElements[j], '*'));
			int n = atoi(multiplier);

			char decodedString[BUF] = "";

			for (int i = 0;i < n;i++) {

				strcat(decodedString, "~~ ~~ ~~ ~~\n");
			}
			strcat(dataOutput, decodedString);
		}
		else {

			char decodedString[BUF] = "~~ ~~ ~~ ~~";
			strcat(decodedString, "\n");
			strcat(dataOutput, decodedString);
		}
	}
}

void decodeOrder(char* str, int idx) { // dekoduje jedna linie rozkazow


	bool hasLabel = true;
	char orderElements[TOT][BUF];

	if (str[0] == ' ' || str[0] == '\t') hasLabel = false; // Czy ma etykiete

	char tempo[BUF];
	strcpy(tempo, str);
	char* token = strtok(tempo, delimit); // Podzial linii na wyrazy wzgledem znakow bialych
	int i = 0;

	while (token != NULL) {
		strcpy(orderElements[i], token);
		token = strtok(NULL, delimit);
		i += 1;
	}

	int j;	//Chodzi tu o uzyskanie dostepu do komendy rozkazu
	if (hasLabel) j = 1;
	else j = 0;

	//if (idx < ordersLength - 1) strcpy(orderElements[i - 1], readInRange(orderElements[i - 1], 0, strlen(orderElements[i - 1]) - 1));
	int orderLength = i;



	char decodedString[BUF] = "";
	strcat(decodedString, ordersHEX[findIndex(orders, ORDERS_LENGTH, orderElements[j])]);

	if (strcmp(orderElements[j], "JZ") == 0 || strcmp(orderElements[j], "JP") == 0 || strcmp(orderElements[j], "JN") == 0 || strcmp(orderElements[j], "J") == 0) { // dekodowanie w przypadku komendy JZ/JP/JN/J

		j += 1;

		strcat(decodedString, " 0");

		if (contains(orderElements[j], ')')) {
			char* registerNum = readInRange(orderElements[j], idx_of_char_in_str(orderElements[j], '(') + 1, idx_of_char_in_str(orderElements[j], ')'));
			char hex[BUF] = "";

			sprintf(hex, "%X", atoi(registerNum));
			strcat(decodedString, hex);

			strcat(decodedString, " ");

			char* shift = readUntil(orderElements[j], '(');
			char shiftToHex[BUF];

			sprintf(shiftToHex, "%X", atoi(shift));

			append_shift(decodedString, shiftToHex);
		}
		else {

			char temp[BUF];
			sprintf(temp, "%X", ORDERS_ADDRESS_REGISTER_NUMBER);
			strcat(decodedString, temp);

			int shift = offsetsOrders[findIndex(labelsOrders, currentLOOidx, orderElements[j])];
			char shiftToHex[BUF];
			sprintf(shiftToHex, "%X", shift);

			strcat(decodedString, " ");

			append_shift(decodedString, shiftToHex);
		}
		strcat(decodedString, "\n");
		strcat(ordersOutput, decodedString);
	}
	else { // dekodowanie w przypadku komendy innej niz JZ/JP/JN/J

		j += 1;

		strcat(decodedString, " ");

		char hex[BUF] = "";
		char temp[BUF];

		strcpy(temp, orderElements[j]);

		sprintf(hex, "%X", atoi(temp));
		strcat(decodedString, hex);

		j += 1;

		if (contains(orderElements[j], ')')) { // Drugi argument rozkazu zawiera przesuniecie i rejestr z adresem wzgledem ktorego rozpatrujemy przesuniecie

			char* registerNum = readInRange(orderElements[j], idx_of_char_in_str(orderElements[j], '(') + 1, idx_of_char_in_str(orderElements[j], ')'));
			char temp[BUF];
			sprintf(temp, "%X", atoi(registerNum));
			strcat(decodedString, temp);


			char* shift = readUntil(orderElements[j], '(');
			char shiftToHex[BUF];

			sprintf(shiftToHex, "%X", atoi(shift));

			strcat(decodedString, " ");

			append_shift(decodedString, shiftToHex);
		}
		else { // Drugi argument rozkazu nie zawiera przesuniecia i rejestru z adresem wzgledem ktorego rozpatrujemy przesuniecie

			bool isSecondArgAnOrderLabel = false;
			bool isSecondArgADataLabel = false;

			int k;
			for (k = 0;k < currentLOOidx;k++) {
				if (strcmp(labelsData[k], readInRange(orderElements[j], 0, strlen(orderElements[j]) - 1)) == 0 && strlen(orderElements[j]) != 1) {

					isSecondArgAnOrderLabel = true;  // Drugi argument rozkazu to etykieta rozkazow
					break;
				}
			}


			for (k = 0;k < currentLODidx;k++) {


				if (strcmp(labelsData[k], orderElements[j]) == 0) {
					isSecondArgADataLabel = true;  // Drugi argument rozkazu to etykieta danych
					break;
				}
			}

			if (isSecondArgAnOrderLabel) {

				char temp[BUF];
				sprintf(temp, "%X", ORDERS_ADDRESS_REGISTER_NUMBER);
				strcat(decodedString, temp);

				int shift = offsetsOrders[findIndex(labelsOrders, currentLOOidx, orderElements[j])];
				char shiftToHex[BUF];
				sprintf(shiftToHex, "%X", shift);


				strcat(decodedString, " ");

				append_shift(decodedString, shiftToHex);

			}
			else if (isSecondArgADataLabel) {

				char temp[BUF];
				sprintf(temp, "%X", DATA_ADDRESS_REGISTER_NUMBER);
				strcat(decodedString, temp);

				int shift = offsetsData[findIndex(labelsData, currentLODidx, orderElements[j])];

				char shiftToHex[BUF];
				sprintf(shiftToHex, "%X", shift);

				strcat(decodedString, " ");

				append_shift(decodedString, shiftToHex);
			}
			else {  // Drugi argument rozkazu to numer rejestru

				char hex[BUF] = "";
				sprintf(hex, "%X", atoi(orderElements[j]));
				strcat(decodedString, hex);

			}
		}


		strcat(decodedString, "\n");
		strcat(ordersOutput, decodedString);
	}
}

void addAllLabels(char dataSection[TOT][BUF], char ordersSection[TOT][BUF], int d_length, int o_length) { // Tworzy liste wszystkich etykiet pojawiajacych sie w pliku wejsciowym


	int k;
	for (k = 0; k < d_length; k++) {

		bool hasLabel = true;
		char dataElements[TOT][BUF];
		char temp[BUF];
		strcpy(temp, dataSection[k]);

		char* token = strtok(temp, delimit);
		int i = 0;

		while (token != NULL) {
			strcpy(dataElements[i], token);
			token = strtok(NULL, delimit);
			i += 1;
		}

		int len = i;

		if (dataSection[k][0] == ' ' || dataSection[k][0] == '\t') hasLabel = false;

		if (hasLabel) addLabelFromData(dataElements[0]);

	}


	for (k = 0; k < o_length; k++) {

		bool hasLabel = true;
		char orderElements[TOT][BUF];


		char temp[BUF];
		strcpy(temp, ordersSection[k]);

		char* token = strtok(temp, delimit);
		int i = 0;

		while (token != NULL) {
			strcpy(orderElements[i], token);
			token = strtok(NULL, delimit);
			i += 1;
		}

		int len = i;

		if (ordersSection[k][0] == ' ' || ordersSection[k][0] == '\t') hasLabel = false;

		if (hasLabel) addLabelFromOrders(orderElements[0]);

	}

}

void addAllOffsets(char dataSection[TOT][BUF], char ordersSection[TOT][BUF], int d_length, int o_length) { // Tworzy liste przesuniec dla wszystkich etykiet pojawiajacych sie w pliku wejsciowym

	currentLOOidx = 0;
	currentLODidx = 0;

	//refreshSections();

	int k;
	for (k = 0; k < d_length; k++) {

		bool hasLabel = true;
		if (dataSection[k][0] == ' ' || dataSection[k][0] == '\t') hasLabel = false;

		char dataElements[TOT][BUF];


		char temp[BUF];
		strcpy(temp, dataSection[k]);

		char* token = strtok(temp, delimit);
		int i = 0;

		while (token != NULL) {
			strcpy(dataElements[i], token);
			token = strtok(NULL, delimit);
			i += 1;
		}

		int len = i;


		if (hasLabel) addOffsetFromData();

		int multiplier;
		if (contains(dataElements[len - 1], '*')) {
			multiplier = atoi(readUntil(dataElements[len - 1], '*'));
		}
		else {
			multiplier = 1;
		}
		currentDataSectionOffset += multiplier * 4;

	}

	//refreshSections();


	for (k = 0; k < o_length; k++) {

		bool hasLabel = true;
		if (ordersSection[k][0] == ' ') hasLabel = false;

		char orderElements[TOT][BUF];

		if (ordersSection[k][0] == ' ' || ordersSection[k][0] == '\t') hasLabel = false;

		char temp[BUF];
		strcpy(temp, ordersSection[k]);

		char* token = strtok(temp, delimit);
		int i = 0;
		while (token != NULL) {
			strcpy(orderElements[i], token);
			token = strtok(NULL, delimit);
			i += 1;
		}

		int len = i;


		if (hasLabel) addOffsetFromOrders();

		if (!contains(orderElements[len - 1], '(') && !contains_T(labelsData, readUntil(orderElements[len - 1], '\n'), dataLabelsLength) && !contains_T(labelsOrders, readUntil(orderElements[len - 1], '\n'), ordersLabelsLength)) {
			currentOrdersSectionOffset += 2;
		}
		else {
			currentOrdersSectionOffset += 4;
		}
	}

}



bool isTheBeginningOfOrdersSection(char str[BUF]) {
	char temp[BUF];
	bool hasLabel = false;

	if (str[0] == " " || str[0] == '\t') hasLabel = false;
	strcpy(temp, str);
	char* token = strtok(temp, delimit);
	char a[TOT][BUF];

	int i = 0;
	while (token != NULL) {
		strcpy(a[i], token);
		token = strtok(NULL, delimit);
		i += 1;
	}

	int j;
	if (hasLabel) j = 1;
	else j = 0;


	return contains_T(orders, a[j], ORDERS_LENGTH);
}


void readInput() { // interpretuje plik wejsciowy, uruchamia procesy dekodowania

	int i = 0;
	int n = 0;
	int m = 0;



	bool isTheEndOfDataSection = false;

	for (i = 0; i < total; ++i) {


		if (isBlank(input[i])) {

			if (i + 1 < total && !isTheEndOfDataSection && isTheBeginningOfOrdersSection(input[i + 1])) {
				isTheEndOfDataSection = true;
				continue;
			}
		}


		if (!isTheEndOfDataSection && !isBlank(input[i])) {

			strcpy(dataSection[n], input[i]);
			n += 1;
		}
		else if (!isBlank(input[i])) {
			strcpy(ordersSection[m], input[i]);

			m += 1;
		}

	}

	dataLength = n;
	ordersLength = m;



	addAllLabels(dataSection, ordersSection, dataLength, ordersLength);
	addAllOffsets(dataSection, ordersSection, dataLength, ordersLength);


	for (int k = 0; k < n; k++) {
		decodeData(dataSection[k]);
	}

	for (int k = 0; k < m; k++) {
		decodeOrder(ordersSection[k], k);
	}


}

void getInput(char ifn[TOT]) { // czyta plik wejsciowy


	FILE* input_file;

	int i = 0;

	input_file = fopen(ifn, "r");

	while (fgets(input[i], BUF, input_file)) {

		input[i][strlen(input[i])] = '\0';
		i++;
	}

	total = i;

	fclose(input_file);



}


