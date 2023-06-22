#include <stdio.h>
#include <stdbool.h>
#include "Definitions.h"
#include "StringManagment.h"
#include "SimulatorTools.h"
#include "ConsoleManagment.h"
#include "FileManagment.h"

void simulateProgram(char machineCode[BIG_BUF], Context* context) { // Symuluje dzialanie programu

	int i;
	char tab[BIG_BUF];

	strcpy(tab, machineCode);

	char lines[TOT][BUF];
	char s[BUF] = "";

	int k = 0;
	int divider = -1;

	for (i = 0;i < strlen(tab);i++) {

		if (tab[i] == '\n' ) {
			if (isBlank(s)) divider = k;

			strcpy(lines[k], s);
			strcpy(s, "");
			k += 1;
			continue;
		}
		strncat(s, tab + i, 1);
	}


	int j;
	context->d_idx = 0;
	context->o_idx = 0;
	bool wasDivider = false;


	for (j = 0; j < k; j++) {

		if (j == divider) {
			wasDivider = true;
			continue;
		}

		if (!wasDivider) {
			strcpy(context->dataSectionHEX[context->d_idx], lines[j]);
			context->d_idx += 1;

		}
		else {
			strcpy(context->ordersSectionHEX[context->o_idx], lines[j]);
			context->o_idx += 1;

		}
	}

	int p;
	for (p = 0; p < context->o_idx; p++) {

		if (context->debug) refreshScreen(p, context);


		char line[TOT][BUF];
		char temp[BUF];

		strcpy(temp, context->ordersSectionHEX[p]);
		split(line, temp, "", context);

		int line_len = strlen(temp);

		char* order_code = malloc(3);
		char* registers_code;
		int* r1 = (int*)malloc(sizeof(int));
		int* r2 = (int*)malloc(sizeof(int));

		switch (line_len) {

		case REG_MEM_LINE_LENGTH:


			strcpy(order_code, line[0]);

			order_code = context->orders[findIndex(context->ordersHEX, ORDERS_LENGTH, order_code)];

			registers_code = malloc(3);
			registers_code[2] = '\0';

			strcpy(registers_code, line[1]);

			char* p1 = &registers_code[0];
			char* p2 = &registers_code[1];

			r1 = hexToDecDigit(p1);
			r2 = hexToDecDigit(p2);

			char shift[BUF] = "";
			strcat(line[2], line[3]);
			strcat(shift, line[2]);

			int dataIdx = hexToDecNum(shift) / 4;

			char* dataLine = context->dataSectionHEX[dataIdx];

			char dataLineWithNoSpaces[8] = "";
			char orderLineWithNoSpaces[8] = "";


			int j;
			for (j = 0;j < 11;j++) {
				if (dataLine[j] != ' ') strncat(dataLineWithNoSpaces, dataLine + j, 1);
			}
			for (j = 0;j < 11;j++) {
				if (temp[j] != ' ') strncat(orderLineWithNoSpaces, temp + j, 1);
			}

			char hexNum[8] = "";

			int q = 0;
			while (dataLineWithNoSpaces[q] == '0' && q <= 7) q++;

			if (q > 7) strcpy(hexNum, "0");
			else {
				int m, y = 0;

				for (m = q; m <= 7;m++) {

					strncat(hexNum, dataLineWithNoSpaces + q + y, 1);
					y += 1;
				}
			}

			int* iteration = &p;

			performRegisterMemoryOperation(*r1, hexToDecNum(hexNum), order_code, orderLineWithNoSpaces, iteration, context);


			break;

		case REG_REG_LINE_LENGTH:

			strcpy(order_code, line[0]);

			order_code = context->orders[findIndex(context->ordersHEX, ORDERS_LENGTH, order_code)];

			registers_code = malloc(3);
			registers_code[2] = '\0';

			strcpy(registers_code, line[1]);

			char* p3 = &registers_code[0];
			char* p4 = &registers_code[1];

			r1 = hexToDecDigit(p3);
			r2 = hexToDecDigit(p4);

			performRegistersOperation(*r1, *r2, order_code, context);



			break;
		default:
			;
		}
		if (context->debug) {

			//Sterowanie programem
			char choice;
			char result[BUF];

		type_command:
			choice = getch();

			switch (choice) {

			case 'p':
				//nastepny krok
				break;

			case 's':
				 strcpy(result,"");

				int q;
				for (q = 0;q < context->d_idx;q++) {
					strcat(result, context->dataSectionHEX[q]);
					strcat(result, "\n");
				}
				strcat(result, "\n");
				for (q = 0;q < context->o_idx;q++) {
					strcat(result, context->ordersSectionHEX[q]);
					strcat(result, "\n");
				}

				writeToOutputFile(result, context);
				if (context->debug) refreshScreen(p, context);
				exit(0);
				break;

			case 'q':
				exit(0);

			default:
				goto type_command;
				break;
			}
		}
	}

	char result[BUF] = "";

	int q;
	for (q = 0;q < context->d_idx;q++) {
		strcat(result, context->dataSectionHEX[q]);
		strcat(result, "\n");
	}
	strcat(result, "\n");
	for (q = 0;q < context->o_idx;q++) {
		strcat(result, context->ordersSectionHEX[q]);
		strcat(result, "\n");
	}

	writeToOutputFile(result, context);
	if (context->debug) refreshScreen(context->o_idx - 1, context);
}


void performRegisterMemoryOperation(int r1, int num, char* order_code, char* dataLine, int* iteration, Context* context) { // Wykonuje rozkaz typu rejestr-pamiec

	char* temp = malloc(3);
	temp = order_code;

	r1 -= 1;


	if (strcmp(temp, "A") == 0) {
		context->rToChange = r1;

		context->registers[r1] += num;
		refreshProgramsState(r1, context);
	}

	if (strcmp(temp, "S") == 0) {
		context->rToChange = r1;

		context->registers[r1] -= num;
		refreshProgramsState(r1, context);
	}

	if (strcmp(temp, "M") == 0) {
		context->rToChange = r1;

		context->registers[r1] *= num;
		refreshProgramsState(r1, context);
	}

	if (strcmp(temp, "D") == 0) {
		context->rToChange = r1;

		context->registers[r1] /= num;
		refreshProgramsState(r1, context);
	}

	if (strcmp(temp, "L") == 0) {
		context->rToChange = r1;

		context->registers[r1] = num;
	}
	if (strcmp(temp, "ST") == 0) {

		int k = 0;
		char* s = malloc(5);
		s[4] = '\0';
		while (k <= 3) {
			s[k] = dataLine[4 + k];
			k += 1;
		}
		int* h = hexToDecDigit(&dataLine[3]);

		fillTyldesWithValue(r1, hexToDecNum(s) + context->registers[*h - 1], context);

	}


	if (strcmp(temp, "LA") == 0) {
		int k = 0;
		char* s = malloc(5);
		s[4] = '\0';
		while (k <= 3) {
			s[k] = dataLine[4 + k];
			k += 1;
		}

		int n = hexToDecNum(s);

		context->rToChange = r1;

		context->registers[r1] = n;
	}



	if (strcmp(temp, "J") == 0) {

		int k = 0;
		char* s = malloc(5);
		s[4] = '\0';
		while (k <= 3) {
			s[k] = dataLine[4 + k];
			k += 1;
		}
		int n = hexToDecNum(s);
		int r = -1;
		int i;
		for (i = 0;i < context->o_idx;i++) {
			if (n <= 0) {
				r = i;
				break;
			}

			char* t = context->ordersSectionHEX[i];

			if (strlen(t) == REG_MEM_LINE_LENGTH) n -= 4;
			else if (strlen(t) == REG_REG_LINE_LENGTH) n -= 2;
		}

		*iteration = r - 1;
	}


	if (strcmp(temp, "JN") == 0 && strcmp(context->programsStateBits, "10") == 0) {
		int k = 0;
		char* s = malloc(5);
		s[4] = '\0';
		while (k <= 3) {
			s[k] = dataLine[4 + k];
			k += 1;
		}
		int n = hexToDecNum(s);
		int r = -1;
		int i;
		for (i = 0;i < context->o_idx;i++) {
			if (n <= 0) {
				r = i;
				break;
			}

			char* t = context->ordersSectionHEX[i];

			if (strlen(t) == REG_MEM_LINE_LENGTH) n -= 4;
			else if (strlen(t) == REG_REG_LINE_LENGTH) n -= 2;
		}

		*iteration = r - 1;

	}

	if (strcmp(temp, "JZ") == 0 && strcmp(context->programsStateBits, "00") == 0) {
		int k = 0;
		char* s = malloc(5);
		s[4] = '\0';
		while (k <= 3) {
			s[k] = dataLine[4 + k];
			k += 1;
		}
		int n = hexToDecNum(s);
		int r = -1;
		int i;
		for (i = 0;i < context->o_idx;i++) {
			if (n <= 0) {
				r = i;
				break;
			}

			char* t = context->ordersSectionHEX[i];

			if (strlen(t) == REG_MEM_LINE_LENGTH) n -= 4;
			else if (strlen(t) == REG_REG_LINE_LENGTH) n -= 2;
		}

		*iteration = r - 1;

	}

	if (strcmp(temp, "JP") == 0 && strcmp(context->programsStateBits, "01") == 0) {
		int k = 0;
		char* s = malloc(5);
		s[4] = '\0';
		while (k <= 3) {
			s[k] = dataLine[4 + k];
			k += 1;
		}
		int n = hexToDecNum(s);
		int r = -1;
		int i;
		for (i = 0;i < context->o_idx;i++) {
			if (n <= 0) {
				r = i;
				break;
			}

			char* t = context->ordersSectionHEX[i];

			if (strlen(t) == REG_MEM_LINE_LENGTH) n -= 4;
			else if (strlen(t) == REG_REG_LINE_LENGTH) n -= 2;
		}

		*iteration = r - 1;
	}

	if (strcmp(temp, "C") == 0) {

		if (context->registers[r1] - num == 0) {
			strcpy(context->programsStateBits, "00");
		}
		else if (context->registers[r1] - num < 0) {
			strcpy(context->programsStateBits, "10");
		}
		else if (context->registers[r1] - num > 0) {
			strcpy(context->programsStateBits, "01");
		}
		else {
			strcpy(context->programsStateBits, "11");
		}
	}

}

void performRegistersOperation(int r1, int r2, char* order_code, Context* context) { // Wykonuje rozkaz typu rejestr-rejestr

	char* temp = malloc(3);
	temp = order_code;

	r1 -= 1;
	r2 -= 1;

	if (strcmp(temp, "AR") == 0) {
		context->rToChange = r1;

		context->registers[r1] += context->registers[r2];
		refreshProgramsState(r1, context);
	}

	if (strcmp(temp, "SR") == 0) {
		context->rToChange = r1;
		
		context->registers[r1] -= context->registers[r2];
		refreshProgramsState(r1, context);
	}
	if (strcmp(temp, "MR") == 0) {
		context->rToChange = r1;

		context->registers[r1] *= context->registers[r2];
		refreshProgramsState(r1, context);
	}

	if (strcmp(temp, "DR") == 0) {
		context->rToChange = r1;

		context->registers[r1] /= context->registers[r2];
		refreshProgramsState(r1, context);
	}

	if (strcmp(temp, "LR") == 0) {
		context->rToChange = r1;

		context->registers[r1] = context->registers[r2];
	}
	if (strcmp(temp, "CR") == 0) {
		if (context->registers[r1] - context->registers[r2] == 0) {
			strcpy(context->programsStateBits, "00");
		}
		else if (context->registers[r1] - context->registers[r2] < 0) {
			strcpy(context->programsStateBits, "10");
		}
		else if (context->registers[r1] - context->registers[r2] > 0) {
			strcpy(context->programsStateBits, "01");
		}
		else {
			strcpy(context->programsStateBits, "11");
		}
	}
}

void refreshProgramsState(int r1, Context* context) { // Odswieza wartosc rejestru stanu programu

	if (context->registers[r1] == 0) {
		strcpy(context->programsStateBits, "00");
	}
	else if (context->registers[r1] < 0) {
		strcpy(context->programsStateBits, "10");
	}
	else if (context->registers[r1] > 0) {
		strcpy(context->programsStateBits, "01");
	}
	else {
		strcpy(context->programsStateBits, "11");
	}

}