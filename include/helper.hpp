#pragma once
#include <Arduino.h>

#include "constants/ntcPoint.hpp"
// void sort(float *array, size_t size);
int IntegerGetAverageValue(int *array, size_t size);
float FloatGetAverageValue(float *array, size_t size);

template <typename T>
const char *ConvertToConstChar(T value, size_t size);

int ntcBinarySearchArray(NTCLIST_VA_TYPE value);