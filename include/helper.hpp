#pragma once
#include <Arduino.h>

#include "constants/ntcPoint.hpp"
int IntegerGetAverageValue(int *array, size_t size);
float FloatGetAverageValue(float *array, size_t size);

int ntcBinarySearchArray(NTCLIST_VA_TYPE value);