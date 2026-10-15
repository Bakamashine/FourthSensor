#pragma once
#include <Arduino.h>
// void sort(float *array, size_t size);
int IntegerGetAverageValue(int *array, size_t size);
float FloatGetAverageValue(float *array, size_t size);

template <typename T>
const char *ConvertToConstChar(T value, size_t size);