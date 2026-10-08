#include "sensor.hpp"
#include "constants/ntcPoint.hpp"
#include "helper.hpp"
#include "settings.hpp"
// #include "ema.hpp"
#include <Arduino.h>

#define MAX_ACP 1023
#define RESISTOR_FROM_SENSOR 2000 // 2kOm

#define FILTER_ALPHA 1.0F // EMA coefficient (0..1], smaller = smoother
#define GET_RES(value)                                                        \
  (RESISTOR_FROM_SENSOR * static_cast<float> (value) / (MAX_ACP - value))
#define TEMP_INTERVAL (200) // ms between samples

#define PERM_DIF 50

float getTempFromTable (Sensor *);

int16_t
ntcTempAt (size_t i)
{

  return static_cast<int16_t> (pgm_read_word (&ntcTable[i].temp_c));
}
int32_t
ntcResAt (size_t i)
{
  return static_cast<int32_t> (pgm_read_dword (&ntcTable[i].resistance));
}

static float
filterAdc (unsigned int freshAdc, unsigned int oldAdc)
{
  return FILTER_ALPHA * freshAdc + (1.0F - FILTER_ALPHA) * oldAdc;
}

float
getTemp (Sensor *sensor)
{
  int rawAdc = sensor->acp;
  if (sensor->adcFilter < 0.0F)
    sensor->adcFilter = static_cast<float> (rawAdc);
  else
    // EMA: alpha * new_adc + (1 - alpha) * old_adc
    // EMA *ema = new EMA<rawAdc, uint8_t>();
    
    sensor->adcFilter
    = filterAdc(rawAdc, sensor->adcFilter);

  // round to the nearest whole ADC count before the table lookup: the filter
  // output is fractional, and the table is indexed by an integer count
  sensor->samples[sensor->sampleIdx] = getTempFromTable (sensor);
  sensor->sampleIdx++;

  if (sensor->sampleIdx >= ATTEMPTS)
    {
      sort (sensor->samples, ATTEMPTS);
      float average = FloatGetAverageValue (&sensor->samples[1], ATTEMPTS - 2);

      if (!sensor->ready || average - sensor->lastTemp > hyst
          || sensor->lastTemp - average > hyst)
        sensor->lastTemp = average;

      sensor->sampleIdx = 0;
      sensor->ready = true;
    }

  return sensor->lastTemp + static_cast<float> (sensor->correctInt);
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
    return static_cast<float> (ntcTempAt (0));
  if (sensor->resist <= ntcResAt (NTC_TABLE_SIZE - 1))
    return static_cast<float> (ntcTempAt (NTC_TABLE_SIZE - 1));

  for (size_t i = 0; i + 1 < NTC_TABLE_SIZE; i++)
    {
      int16_t temp = ntcTempAt (i);
      int32_t res = ntcResAt (i);

      if (sensor->resist > res)
        continue;
      if (sensor->resist < ntcResAt (i + 1))
        continue;

      // interpolate between the two bracketing table rows
      float fraction = static_cast<float> (res - sensor->resist)
                       / (res - ntcResAt (i + 1));
      return static_cast<float> (temp)
             + fraction * static_cast<float> (ntcTempAt (i + 1) - temp);
    }
  return 0.0F;
}

void
constrSensor (Sensor *sensor, uint8_t pin)
{
  uint32_t now = millis ();
  if (now - sensor->lastSampleMs < TEMP_INTERVAL)
    {

      return;
    }

  int acp = analogRead (pin);
  sensor->lastSampleMs = now;

  bool spike = sensor->lastAcp >= 0
               && (acp - sensor->lastAcp > PERM_DIF
                   || sensor->lastAcp - acp > PERM_DIF);
  sensor->acp = acp;
  if (spike)
    return;
  sensor->temp = getTemp (sensor);
}

int
getIAverageTemp (Sensors *sn)
{
  int temp[] = { static_cast<int> (sn->_s_main->temp),
                 static_cast<int> (sn->_s_first->temp),
                 static_cast<int> (sn->_s_second->temp) };
  float averageVal
      = IntegerGetAverageValue (temp, sizeof (temp) / sizeof (temp[0]));
  return static_cast<int> (averageVal);
}

float
getFAverageTemp (Sensors *sn)
{
  float temp[]
      = { sn->_s_main->temp, sn->_s_first->temp, sn->_s_second->temp };
  float averageVal
      = FloatGetAverageValue (temp, sizeof (temp) / sizeof (temp[0]));
  return averageVal;
}