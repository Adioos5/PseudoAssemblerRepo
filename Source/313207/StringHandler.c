#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "StringManagment.h"


void fillTyldesWithValue(int r_num, int shift, Context* context) { // Wypelnia tyldowa linie w kodzie maszynowym wartoscia liczby heksadecymalnej

	int idx = shift / 4;

	int num = context->registers[r_num];
	char numToString[BUF] = "";

	int x = num;
	int j = 0;

	sprintf(numToString, "%X", num);

	char s1[BUF] = "";
	char s2[BUF] = "";

	int k = strlen(numToString);
	strcpy(s1, numToString);
	strcpy(s1, reverseString(s1));

	while (k <= 7) {
		s1[k] = '0';
		k += 1;
	}
	strcpy(s1, reverseString(s1));

	int r = 0, m = 0;
	for (r = 0;r <= 11;r++) {
		if ((r + 1) % 3 == 0 && r != 11)
			s2[r] = ' ';
		else {
			s2[r] = s1[m];
			m += 1;
		}
	}

	context->dToChange = idx;

	strcpy(context->dataSectionHEX[idx], s2);

}

int* hexToDecDigit(char* c) { // Zamienia cyfre heksadecymalna w postaci wskaznika do chara do liczby dziesietnej w postaci wskaznika do inta

	char T[] = { 'A', 'B', 'C', 'D', 'E', 'F' };
	int idx = idxOfCharInStrP(T, c);

	if (idx != -1) {
		static int* result;
		result = (int*)malloc(sizeof(int));

		*result = idx + 10;


		return result;
	}
	else {
		static int* result;

		result = (int*)malloc(sizeof(int));

		*result = *c - '0';

		return result;
	}

	return -1;

}

int hexToDecNum(char* s) { // Zamienia liczbe heksadecymalna w postaci stringa do liczby dziesietnej w postaci inta

	int n = strlen(s);
	char* temp = malloc(n + 1);
	strcpy(temp, s);

	char T[] = { 'A', 'B', 'C', 'D', 'E', 'F' };

	int i, p = 1, r = 0, w;

	for (i = n - 1;i >= 0;i--) {
		if (contains(T, temp[i])) {
			int idx = idxOfCharInStr(T, temp[i]);
			w = idx + 10;
		}
		else {
			w = temp[i] - '0';
		}

		r += w * p;

		p *= 16;

	}

	return r;

}

int findIndex(char a[TOT][BUF], int size, char value[BUF]) { // Szuka indeksu ciagu znakow w tablicy ciagow znakowych

	int j = 0;
	while (j < size) {

		if (strcmp(a[j], value) == 0) return j;
		j += 1;
	}

	return -1;

}

int idxOfCharInStrP(char str[BUF], char* c) { // Szuka indeksu znaku w ciagu znakow (druga wersja)

	static char* ptr;

	ptr = (char*)malloc(sizeof(char));

	static index;
	static char val;

	val = *c;
	ptr = strchr(str, val);

	if (ptr) {
		index = ptr - str;
		return index;
	}
	return -1;
}

int idxOfCharInStr(char str[BUF], char c) { // Szuka indeksu znaku w ciagu znakow

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

bool containsT(static char strings[TOT][BUF], char str[BUF], int len) { // Sprawdza czy tablica ciagow znakowych zawiera okreslony ciag znakow
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

bool isBlank(char* str) { //Sprawdza czy linia nie jest pusta (czy nie jest pelna spacji)
	while (*str) {
		if (!isspace(*str++)) {
			return false;
		}
	}
	return true;
}


void split(char dest[TOT][BUF], char* src, char additionalDelimiters[BUF], Context* context) { // Podzial linii na wyrazy wzgledem specjalnych znakow

	char delimiters[BUF] = "";

	strcpy(delimiters, context->delimit);

	strcat(delimiters, additionalDelimiters);

	char* temp = malloc(strlen(src) + 1);
	strcpy(temp, src);

	char* token = strtok(temp, delimiters); 

	int i = 0;

	while (token != NULL) {
		if (token[0] == '/') break;
		strcpy(dest[i], token);
		token = strtok(NULL, delimiters);
		i += 1;
	}

	context->lastSplitLength = i;

	free(temp);


}

void strip(char* s, Context* context) { 
	char* temp = malloc(strlen(s) + 1);
	strcpy(temp, s);

	char* token = strtok(temp, context->delimit); 

	int i = 0;

	while (token != NULL) {
		strcpy(s, token);
		token = strtok(NULL, context->delimit);
		i += 1;
	}


}


void generateMachineCode(Context* context) { // Zapisuje kod maszynowy do zmiennej w programie

	int i;
	int wasDivider;

	switch (context->psa_code) {
	case 1:
		strcat(context->dataOutput, "\n");	
		strcat(context->dataOutput, context->ordersOutput);
		strcpy(context->outputText, context->dataOutput);

		break;

	case 0:

		wasDivider = 0;
		for (i = 0;i < context->total;i++) {
			if (isBlank(context->input[i]) && wasDivider) break;
			if (isBlank(context->input[i]) && !wasDivider) wasDivider = 1;

			strcat(context->outputText, context->input[i]);

		}

		strcat(context->outputText, "\n");

	//	printf("%s", context->outputText);

		break;
	}

}