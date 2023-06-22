#pragma once
#include "Definitions.h"

void refreshScreen(int idx, Context* context);
void clrscr();

void setConsoleColour(WORD* Attributes, DWORD Colour);
void resetConsoleColour(WORD Attributes);