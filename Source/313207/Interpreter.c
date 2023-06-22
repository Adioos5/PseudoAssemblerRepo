#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <Windows.h>
#include "Definitions.h"
#include "StringManagment.h"
#include "InterpreterTools.h"

void interpreteInput(Context* context) { // Interpretuje plik wejsciowy, uruchamia procesy dekodowania

	int i = 0;
	int n = 0;
	int m = 0;

	bool isTheEndOfDataSection = false;


	for (i = 0; i < context->total; ++i) {

		if (isBlank(context->input[i])) {

			if (i + 1 < context->total && !isTheEndOfDataSection && isTheBeginningOfOrdersSection(context->input[i + 1], context)) {
				isTheEndOfDataSection = true;
				continue;
			}
		}


		if (!isTheEndOfDataSection && !isBlank(context->input[i])) {
			if (context->input[i][0] != '/' && context->input[i][1] != '/') {
				strcpy(context->dataSection[n], context->input[i]);
				n += 1;
			}
		}
		else if (!isBlank(context->input[i])) {
			if (context->input[i][0] != '/' && context->input[i][1] != '/') {
				strcpy(context->ordersSection[m], context->input[i]);
				m += 1;
			}
		}

	}

	context->dataLength = n;
	context->ordersLength = m;



	addAllLabels(context->dataSection, context->ordersSection, context->dataLength, context->ordersLength, context);
	addAllOffsets(context->dataSection, context->ordersSection, context->dataLength, context->ordersLength, context);


	for (int k = 0; k < n; k++) {
		decodeData(context->dataSection[k], context);
	}

	for (int k = 0; k < m; k++) {
		decodeOrder(context->ordersSection[k], k, context);
	}
}

void addOffsetFromData(Context* context) { //Dodaje przesuniecie dla etykiety danej

	context->offsetsData[context->currentLODidx] = context->currentDataSectionOffset;
	context->currentLODidx += 1;
}

void addOffsetFromOrders(Context* context) { //Dodaje przesuniecie dla etykiety rozkazu

	context->offsetsOrders[context->currentLOOidx] = context->currentOrdersSectionOffset;
	context->currentLOOidx += 1;
}

void addLabelFromData(char str[BUF], Context* context) { //Dodaje etykiete danej do listy etykiet danych

	strcpy(context->labelsData[context->currentLODidx], str);
	context->dataLabelsLength += 1;
	context->currentLODidx += 1;
}

void addLabelFromOrders(char str[BUF], Context* context) { //Dodaje etykiete rozkazu do listy etykiet rozkazow
	strcpy(context->labelsOrders[context->currentLOOidx], str);
	context->ordersLabelsLength += 1;
	context->currentLOOidx += 1;
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

void decodeData(char* str, Context* context) { // Dekoduje jedna linie danych
	bool hasLabel = true;
	char dataElements[TOT][BUF];


	if (str[0] == ' ' || str[0] == '\t') hasLabel = false; // Czy ma etykiete

	split(dataElements, str, "", context);

	int dataLength = context->lastSplitLength;

	int j;	//Chodzi tu o uzyskanie dostepu do komendy danych
	if (hasLabel) j = 1;
	else j = 0;

	char temp[BUF] = "";
	char num[BUF] = "";

	int lastIdx = 0;

	if (strcmp(dataElements[j], "DC") == 0) { // Dekodowanie w przypadku komendy DC

		j += 1;
		lastIdx = j;
		strcat(num, readInRange(dataElements[j], idxOfCharInStr(dataElements[j], '(') + 1, idxOfCharInStr(dataElements[j], ')')));

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

		strcat(context->dataOutput, decodedString);

	}
	else if (strcmp(dataElements[j], "DS") == 0) { // Dekodowanie w przypadku komendy DS
		j += 1;

		if (contains(dataElements[j], '*')) {

			char multiplier[BUF] = "";
			strcat(multiplier, readUntil(dataElements[j], '*'));
			int n = atoi(multiplier);

			char decodedString[BUF] = "";

			for (int i = 0;i < n;i++) {

				strcat(decodedString, "~~ ~~ ~~ ~~\n");
			}
			strcat(context->dataOutput, decodedString);
		}
		else {

			char decodedString[BUF] = "~~ ~~ ~~ ~~";
			strcat(decodedString, "\n");
			strcat(context->dataOutput, decodedString);
		}
	}
}

void decodeOrder(char* str, int idx, Context* context) { // Dekoduje jedna linie rozkazow


	bool hasLabel = true;
	char orderElements[TOT][BUF];

	if (str[0] == ' ' || str[0] == '\t') hasLabel = false; // Czy ma etykiete

	char tempo[BUF];
	strcpy(tempo, str);

	split(orderElements, tempo, "", context);

	int j;	//Chodzi tu o uzyskanie dostepu do komendy rozkazu
	if (hasLabel) j = 1;
	else j = 0;

	int orderLength = context->lastSplitLength;

	char decodedString[BUF] = "";
	strcat(decodedString, context->ordersHEX[findIndex(context->orders, ORDERS_LENGTH, orderElements[j])]);

	if (strcmp(orderElements[j], "JZ") == 0 || strcmp(orderElements[j], "JP") == 0 || strcmp(orderElements[j], "JN") == 0 || strcmp(orderElements[j], "J") == 0) { // Dekodowanie w przypadku komendy JZ/JP/JN/J

		j += 1;

		strcat(decodedString, " 0");

		if (contains(orderElements[j], ')')) {
			char* registerNum = readInRange(orderElements[j], idxOfCharInStr(orderElements[j], '(') + 1, idxOfCharInStr(orderElements[j], ')'));
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

			int shift = context->offsetsOrders[findIndex(context->labelsOrders, context->currentLOOidx, orderElements[j])];
			char shiftToHex[BUF];
			sprintf(shiftToHex, "%X", shift);

			strcat(decodedString, " ");

			append_shift(decodedString, shiftToHex);
		}
		strcat(decodedString, "\n");
		strcat(context->ordersOutput, decodedString);
	}
	else { // Dekodowanie w przypadku komendy innej niz JZ/JP/JN/J

		j += 1;

		strcat(decodedString, " ");

		char hex[BUF] = "";
		char temp[BUF];

		strcpy(temp, orderElements[j]);

		sprintf(hex, "%X", atoi(temp));
		strcat(decodedString, hex);

		j += 1;

		if (contains(orderElements[j], ')')) { // Drugi argument rozkazu zawiera przesuniecie i rejestr z adresem wzgledem ktorego rozpatrujemy przesuniecie

			char* registerNum = readInRange(orderElements[j], idxOfCharInStr(orderElements[j], '(') + 1, idxOfCharInStr(orderElements[j], ')'));
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
			for (k = 0;k < context->currentLOOidx;k++) {
				if (strcmp(context->labelsData[k], readInRange(orderElements[j], 0, strlen(orderElements[j]) - 1)) == 0 && strlen(orderElements[j]) != 1) {

					isSecondArgAnOrderLabel = true;  // Drugi argument rozkazu to etykieta rozkazow
					break;
				}
			}


			for (k = 0;k < context->currentLODidx;k++) {


				if (strcmp(context->labelsData[k], orderElements[j]) == 0) {
					isSecondArgADataLabel = true;  // Drugi argument rozkazu to etykieta danych
					break;
				}
			}

			if (isSecondArgAnOrderLabel) {

				char temp[BUF];
				sprintf(temp, "%X", ORDERS_ADDRESS_REGISTER_NUMBER);
				strcat(decodedString, temp);

				int shift = context->offsetsOrders[findIndex(context->labelsOrders, context->currentLOOidx, orderElements[j])];
				char shiftToHex[BUF];
				sprintf(shiftToHex, "%X", shift);


				strcat(decodedString, " ");

				append_shift(decodedString, shiftToHex);

			}
			else if (isSecondArgADataLabel) {

				char temp[BUF];
				sprintf(temp, "%X", DATA_ADDRESS_REGISTER_NUMBER);
				strcat(decodedString, temp);

				int shift = context->offsetsData[findIndex(context->labelsData, context->currentLODidx, orderElements[j])];

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
		strcat(context->ordersOutput, decodedString);
	}
}

void addAllLabels(char dataSection[TOT][BUF], char ordersSection[TOT][BUF], int d_length, int o_length, Context* context) { // Tworzy liste wszystkich etykiet pojawiajacych sie w pliku wejsciowym


	int k;
	for (k = 0; k < d_length; k++) {

		bool hasLabel = true;
		char dataElements[TOT][BUF];
		char temp[BUF];
		strcpy(temp, dataSection[k]);

		split(dataElements, temp, "", context);

		int len = context->lastSplitLength;

		if (dataSection[k][0] == ' ' || dataSection[k][0] == '\t') hasLabel = false;

		if (hasLabel) addLabelFromData(dataElements[0], context);

	}


	for (k = 0; k < o_length; k++) {

		bool hasLabel = true;
		char orderElements[TOT][BUF];


		char temp[BUF];
		strcpy(temp, ordersSection[k]);

		split(orderElements, temp, "", context);

		int len = context->lastSplitLength;

		if (ordersSection[k][0] == ' ' || ordersSection[k][0] == '\t') hasLabel = false;

		if (hasLabel) addLabelFromOrders(orderElements[0], context);

	}

}

void addAllOffsets(char dataSection[TOT][BUF], char ordersSection[TOT][BUF], int d_length, int o_length, Context* context) { // Tworzy liste przesuniec dla wszystkich etykiet pojawiajacych sie w pliku wejsciowym

	context->currentLOOidx = 0;
	context->currentLODidx = 0;

	int k;
	for (k = 0; k < d_length; k++) {

		bool hasLabel = true;
		if (dataSection[k][0] == ' ' || dataSection[k][0] == '\t') hasLabel = false;

		char dataElements[TOT][BUF];


		char temp[BUF];
		strcpy(temp, dataSection[k]);

		split(dataElements, temp, "", context);

		int len = context->lastSplitLength;

		if (hasLabel) addOffsetFromData(context);

		int multiplier;
		if (contains(dataElements[len - 1], '*')) {
			multiplier = atoi(readUntil(dataElements[len - 1], '*'));
		}
		else {
			multiplier = 1;
		}
		context->currentDataSectionOffset += multiplier * 4;

	}

	for (k = 0; k < o_length; k++) {

		bool hasLabel = true;
		if (ordersSection[k][0] == ' ') hasLabel = false;

		char orderElements[TOT][BUF];

		if (ordersSection[k][0] == ' ' || ordersSection[k][0] == '\t') hasLabel = false;

		char temp[BUF];
		strcpy(temp, ordersSection[k]);

		split(orderElements, temp, "", context);

		int len = context->lastSplitLength;

		if (hasLabel) addOffsetFromOrders(context);

		if (!contains(orderElements[len - 1], '(') && !containsT(context->labelsData, readUntil(orderElements[len - 1], '\n'), context->dataLabelsLength) && !containsT(context->labelsOrders, readUntil(orderElements[len - 1], '\n'), context->ordersLabelsLength)) {
			context->currentOrdersSectionOffset += 2;
		}
		else {
			context->currentOrdersSectionOffset += 4;
		}
	}

}

bool isTheBeginningOfOrdersSection(char str[BUF], Context* context) { // Sprawdza czy dana linia jest poczatkiem sekcji rozkazow
	char temp[BUF];
	bool hasLabel = false;

	if (str[0] == " " || str[0] == '\t') hasLabel = false;
	strcpy(temp, str);
	char a[TOT][BUF];

	split(a, temp, "", context);

	int j;
	if (hasLabel) j = 1;
	else j = 0;

	return containsT(context->orders, a[j], ORDERS_LENGTH);
}



