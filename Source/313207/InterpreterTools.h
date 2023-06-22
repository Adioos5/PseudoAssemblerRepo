#pragma once
#include "Definitions.h"

void interpreteInput(Context* context);

void decodeData(char* str);
void decodeOrder(char* str, int idx);

void addOffsetFromData(Context* context);
void addOffsetFromOrders(Context* context);

void addLabelFromData(char str[BUF], Context* context);
void addLabelFromOrders(char str[BUF], Context* context);

void addAllLabels(char dataSection[TOT][BUF], char ordersSection[TOT][BUF], int d_length, int o_length, Context* context);
void addAllOffsets(char dataSection[TOT][BUF], char ordersSection[TOT][BUF], int d_length, int o_length, Context* context);

void append_shift(char* str, char* shiftToHex);

bool isTheBeginningOfOrdersSection(char str[BUF], Context* context);