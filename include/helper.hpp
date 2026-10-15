#pragma once
#include <Arduino.h>
// Unused (no callers) - see helper.cpp. Kept for reference.
// void sort(float *array, size_t size);
int IntegerGetAverageValue(int *array, size_t size);
float FloatGetAverageValue(float *array, size_t size);
void setFloatText(char *buf, size_t size, float v);
void setIntText(char *buf, size_t size, int v);
