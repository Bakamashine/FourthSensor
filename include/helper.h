#ifndef HELPER_H
#define HELPER_H
#include <Arduino.h>
void sort (float *array, size_t size);
int IntegerGetAverageValue (int *array, size_t size);
float FloatGetAverageValue (float *array, size_t size);
void setFloatText (char *buf, size_t size, const char *label, float v);
#endif