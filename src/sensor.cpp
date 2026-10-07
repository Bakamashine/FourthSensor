#include "sensor.h"
#include "constants/ntcPoint.h"
#include "helper.h"
#include <Arduino.h>
// correction is an offset in degrees, keep it a sane displayable value
#define MIN_CORRECT_INT (-50)
#define MAX_CORRECT_INT 50

#define MAX_ACP 1023
#define RESISTOR_FROM_SENSOR 2000 // 2kOm

#define FILTER_ALPHA 0.15F // EMA coefficient (0..1], smaller = smoother
#define GET_RES(value)                                                        \
  (RESISTOR_FROM_SENSOR * (float)value / (MAX_ACP - value))
#define TEMP_INTERVAL (1000) // ms between samples

// uint32_t _lastSampleMs = 0;
// int _sampleIdx = 0;
// float _lastTemp = 0;
// float _samples[ATTEMPTS] = {};
// int _correctInt = 0;

float getTempFromTable (Sensor *);

int16_t
ntcTempAt (size_t i)
{

  return (int16_t)(pgm_read_word (&ntcTable[i].temp_c));
}
int32_t
ntcResAt (size_t i)
{
  return (int32_t)(pgm_read_dword (&ntcTable[i].resistance));
}

float
getTemp (Sensor *sensor)
{
  // if (now == 0)
  // now = millis ();
  // if (now - sensor->lastSampleMs < TEMP_INTERVAL)
  // if (return_last)
  // return sensor->lastTemp + (float)(sensor->correctInt);

  // sensor->lastSampleMs = now;

  int rawAdc = sensor->acp;
  if (sensor->_adcFilter < 0.0F)
    sensor->_adcFilter = (float)(rawAdc);
  else
    // EMA: alpha * new + (1 - alpha) * old
    sensor->_adcFilter
        = FILTER_ALPHA * rawAdc + (1.0F - FILTER_ALPHA) * sensor->_adcFilter;

  // round to the nearest whole ADC count before the table lookup: the filter
  // output is fractional, and the table is indexed by an integer count
  sensor->samples[sensor->sampleIdx] = getTempFromTable (sensor);
  sensor->sampleIdx++;

  if (sensor->sampleIdx >= ATTEMPTS)
    {
      sort (sensor->samples, ATTEMPTS);
      sensor->lastTemp
          = FloatGetAverageValue (&sensor->samples[1], ATTEMPTS - 2);
      sensor->sampleIdx = 0;
    }
  return sensor->lastTemp + (float)(sensor->correctInt);
}

float
getTempFromTable (Sensor *sensor)
{
  int acp = sensor->acp;
  if (acp >= MAX_ACP)
    acp = MAX_ACP - 1;
  if (acp <= 0)
    acp = 1;

  sensor->resist = GET_RES (acp);

  // get max or min value
  if (sensor->resist >= ntcResAt (0))
    return (float)(ntcTempAt (0));
  if (sensor->resist <= ntcResAt (NTC_TABLE_SIZE - 1))
    return (float)(ntcTempAt (NTC_TABLE_SIZE - 1));

  for (size_t i = 0; i + 1 < NTC_TABLE_SIZE; i++)
    {
      int16_t temp = ntcTempAt (i);
      int32_t res = ntcResAt (i);

      if (sensor->resist > res)
        continue;
      if (sensor->resist < ntcResAt (i + 1))
        continue;

      // interpolate between the two bracketing table rows
      float fraction
          = (float)(res - sensor->resist) / (res - ntcResAt (i + 1));
      return (float)(temp) + fraction * (float)(ntcTempAt (i + 1) - temp);
    }
  return 0.0F;
}

void
constrSensor (Sensor *sensor, uint8_t pin)
{
  uint32_t now = millis ();
  if (now - sensor->lastSampleMs < TEMP_INTERVAL)
    {
      // Too soon since last reading - skip
      return;
    }
  sensor->acp = analogRead (pin);
  sensor->temp = getTemp (sensor);
  sensor->lastSampleMs = now;
}
