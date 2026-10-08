#include "helper.hpp"

void
setFloatText (char *buf, size_t size, float v)
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

  snprintf (buf, size, "%ld.%02ld", whole, frac);
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
  return sum / static_cast<float> (size);
}

int
IntegerGetAverageValue (int *arr, size_t size)
{
  if (size == 0)
    return 0;
  int sum = 0;
  for (size_t i = 0; i < size; i++)
    sum += arr[i];
  return static_cast<int> (sum / static_cast<int> (size));
}

void
setIntText (char *buf, size_t size, int v)
{
  if (!buf || size == 0)
    return;

  char tmp[11];
  size_t i = 0;
  bool negative = v < 0;
  unsigned int uv = negative ? static_cast<unsigned int> (-(v + 1)) + 1u
                             : static_cast<unsigned int> (v);

  do
    {
      tmp[i++] = static_cast<char> ('0' + (uv % 10u));
      uv /= 10u;
    }
  while (uv != 0 && i < sizeof (tmp));

  size_t out = 0;
  if (negative && out < size - 1)
    buf[out++] = '-';

  while (i > 0 && out < size - 1)
    buf[out++] = tmp[--i];

  buf[out] = '\0';
}