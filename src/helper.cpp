#include "helper.hpp"

#include "constants/ntcPoint.hpp"
#include "constants/settings.hpp"
#include "sensor.hpp"

float FloatGetAverageValue(float *arr, size_t size) {
  if (size == 0) return 0;
  float sum = 0;
  for (size_t i = 0; i < size; i++) sum += arr[i];
  return sum / static_cast<float>(size);
}

int IntegerGetAverageValue(int *arr, size_t size) {
  if (size == 0) return 0;
  int sum = 0;
  for (size_t i = 0; i < size; i++) sum += arr[i];
  return static_cast<int>(sum / static_cast<int>(size));
}

int ntcBinarySearchArray(NTCLIST_VA_TYPE val) {
  if (val >= ntcValAt(0)) return -1;
  if (val <= ntcValAt(NTC_TABLE_SIZE - 1)) return -2;
  int l = 0;
  int r = NTC_TABLE_SIZE - 1;
  while (r - l > 1) {
    int m = (l + r) >> 1;
    NTCLIST_VA_TYPE vm = ntcValAt(m);
    if (vm > val)
      l = m;
    else if (vm < val)
      r = m;
    else
      return m;
  }
  return l;
}