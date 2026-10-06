#include "helper.h"

void
setFloatText (char *buf, size_t size, const char *label, float v)
{
  if (v < 0)
    v = -v;
  long whole = static_cast<long> (v);
  long frac
      = static_cast<long> ((v - static_cast<float> (whole)) * 100.0F + 0.5F);
  if (frac >= 100)
    {
      frac -= 100;
      whole += 1;
    }

  snprintf (buf, size, "%s: %ld.%02ld", label, whole, frac);
}

void
sort (float *array, size_t size)
{
  if (size < 2)
    return;

  for (size_t a = 1; a < size; a++)
    {
      for (size_t b = size - 1; b >= a; b--)
        {
          if (array[b] < array[b - 1])
            {
              float t = array[b - 1];
              array[b - 1] = array[b];
              array[b] = t;
            }
        }
    }
}

float
FloatGetAverageValue (float *arr, size_t size)
{
  if (size == 0)
    return 0;
  float sum = 0;
  for (size_t i = 0; i < size; i++)
    sum += arr[i];
  return sum / (float)size;
}