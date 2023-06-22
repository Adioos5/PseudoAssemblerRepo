#pragma once
#include "Definitions.h"

void simulateProgram(char s[BIG_BUF], Context* context);

void modifyRegisterValue(char operation[BUF], int dest, int src);

void refreshSections();
void refreshProgramsState(int r1, Context* context);

void performRegistersOperation(int r1, int r2, char* order_code, Context* context);
void performRegisterMemoryOperation(int r1, int num, char* order_code, char* dataLine, Context* context);
