#include <stdio.h>
#include <stdbool.h>
#include "Definitions.h"
#include "InitializingTools.h"

Context* initContext() { // Definiuje wszystkie niezbedne do wykonania calego programu zmienne

	Context context;

	strcpy(context.delimit, " \r\n\t,");

	strcpy(context.orders[0], "AR");
	strcpy(context.orders[1], "A");
	strcpy(context.orders[2], "SR");
	strcpy(context.orders[3], "S");
	strcpy(context.orders[4], "MR");
	strcpy(context.orders[5], "M");
	strcpy(context.orders[6], "DR");
	strcpy(context.orders[7], "D");
	strcpy(context.orders[8], "CR");
	strcpy(context.orders[9], "C");
	strcpy(context.orders[10], "J");
	strcpy(context.orders[11], "JZ");
	strcpy(context.orders[12], "JP");
	strcpy(context.orders[13], "JN");
	strcpy(context.orders[14], "L");
	strcpy(context.orders[15], "LR");
	strcpy(context.orders[16], "LA");
	strcpy(context.orders[17], "ST");

	strcpy(context.ordersHEX[0], "10");
	strcpy(context.ordersHEX[1], "D1");
	strcpy(context.ordersHEX[2], "12");
	strcpy(context.ordersHEX[3], "D3");
	strcpy(context.ordersHEX[4], "14");
	strcpy(context.ordersHEX[5], "D5");
	strcpy(context.ordersHEX[6], "16");
	strcpy(context.ordersHEX[7], "D7");
	strcpy(context.ordersHEX[8], "18");
	strcpy(context.ordersHEX[9], "D9");
	strcpy(context.ordersHEX[10], "E0");
	strcpy(context.ordersHEX[11], "E1");
	strcpy(context.ordersHEX[12], "E2");
	strcpy(context.ordersHEX[13], "E3");
	strcpy(context.ordersHEX[14], "F0");
	strcpy(context.ordersHEX[15], "31");
	strcpy(context.ordersHEX[16], "F2");
	strcpy(context.ordersHEX[17], "F3");

	strcpy(context.dataOutput, "");
	strcpy(context.ordersOutput, "");
	strcpy(context.programsStateBits, "00");
	strcpy(context.outputText, "");

	context.registers[0] = 0;
	context.registers[1] = 0;
	context.registers[2] = 0;
	context.registers[3] = 0;
	context.registers[4] = 0;
	context.registers[5] = 0;
	context.registers[6] = 0;
	context.registers[7] = 0;
	context.registers[8] = 0;
	context.registers[9] = 0;
	context.registers[10] = 0;
	context.registers[11] = 0;
	context.registers[12] = 0;
	context.registers[13] = 0;
	context.registers[14] = 0;

	context.currentDataSectionOffset = 0;
	context.currentOrdersSectionOffset = 0;
	context.currentLODidx = 0;
	context.currentLOOidx = 0;
	context.dataLabelsLength = 0;
	context.ordersLabelsLength = 0;
	context.lastSplitLength = -1;

	context.psa_code = 1;
	context.debug = 0;

	context.rToChange = -1;
	context.dToChange = -1;

	return &context;
}
