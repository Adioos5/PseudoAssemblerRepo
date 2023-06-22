#pragma once

#define _CRT_SECURE_NO_DEPRECATE

#define BUF 100
#define BIG_BUF 2000
#define TOT 200
#define ORDERS_LENGTH 18
#define DATA_ADDRESS_REGISTER_NUMBER 15
#define ORDERS_ADDRESS_REGISTER_NUMBER 14
#define REG_MEM_LINE_LENGTH 11
#define REG_REG_LINE_LENGTH 5

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

#include <Windows.h>

typedef struct context Context;

typedef struct context { // Deklaruje wszystkie niezbedne do wykonania calego programu zmienne

	HANDLE hStdOut;

	char input[TOT][BUF];
	char inputFileName[BUF];
	char delimit[BUF];
	char orders[TOT][BUF];
	char ordersHEX[TOT][BUF];
	char dataSection[TOT][BUF];
	char ordersSection[TOT][BUF];
	char dataSectionHEX[TOT][BUF];
	char ordersSectionHEX[TOT][BUF];
	char dataOutput[BIG_BUF];
	char ordersOutput[BIG_BUF];
	char programsStateBits[BUF];
	char outputText[BIG_BUF];
	char labelsData[TOT][BUF];
	char labelsOrders[TOT][BUF];

	int total;
	int ordersLength;
	int dataLength;
	int currentDataSectionOffset;
	int currentOrdersSectionOffset;
	int currentLODidx;
	int currentLOOidx;
	int dataLabelsLength;
	int ordersLabelsLength;
	int d_idx, o_idx;
	int offsetsData[TOT];
	int offsetsOrders[TOT];
	int registers[15];
	int lastSplitLength;

	int psa_code;
	int debug;

	int rToChange;
	int dToChange;

}Context;

