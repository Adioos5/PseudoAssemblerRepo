#pragma once
#include "Definitions.h"

void strip(char* s, Context* context);
void split(char dest[TOT][BUF], char* src, Context* context);
void fillTyldesWithValue(int r_num, Context* context);
void generateMachineCode(Context* context);

char* reverseString(char str[BUF]);
char* readUntil(char str[BUF], char c);
char* readInRange(char str[BUF], int p, int q);

int findIndex(char a[TOT][BUF], int size, char value[BUF]);
int idxOfCharInStr(char str[BUF], char c);
int idxOfCharInStrP(char str[BUF], char* c);
int hexToDecNum(char* s);

int* hexToDecDigit(char* c);

bool isBlank(char* str);
bool contains(char str[BUF], char c);
bool containsT(static char strings[TOT][BUF], char str[BUF], int len);
