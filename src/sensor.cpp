#include "sensor.hpp"

#include <Arduino.h>

#include "MedianFilter.h"
#include "constants/ntcPoint.hpp"
#include "constants/settings.hpp"
#include "helper.hpp"
#include "macro/debugUi.hpp"
#include "settings.hpp"

#define MAX_ACP 1023
#define RESISTOR_FROM_SENSOR 2000  // 2kOm

#define GET_RES(value) \
  (RESISTOR_FROM_SENSOR * static_cast<float>(value) / (MAX_ACP - value))
#define TEMP_INTERVAL 0  // ms between samples

#define PERM_DIF 150
#define SPIKE_LIMIT 3

float getTempFromTable(Sensor *);

int16_t ntcTempAt(size_t i) {
  return static_cast<int16_t>(pgm_read_word(&ntcTable[i].temp_c));
}
int32_t ntcResAt(size_t i) {
  return static_cast<int32_t>(pgm_read_dword(&ntcTable[i].resistance));
}

float getTemp(Sensor *sensor) {
  // round to the nearest whole ADC count before the table lookup: the filter
  // output is fractional, and the table is indexed by an integer count
  sensor->samples[sensor->sampleIdx] = getTempFromTable(sensor);
  sensor->sampleIdx++;

  if (sensor->sampleIdx >= ATTEMPTS) {
    sort(sensor->samples, ATTEMPTS);
    // float averages[3];
    // for (int i = 0; i<3;i++)
    //   averages[i] = FloatGetAverageValue(&sensor->samples[1], ATTEMPTS-2);
    float average = FloatGetAverageValue(&sensor->samples[1], ATTEMPTS - 2);

    if (!sensor->ready || average - sensor->lastTemp > hyst ||
        sensor->lastTemp - average > hyst)

      sensor->lastTemp = average;

    sensor->sampleIdx = 0;
    sensor->ready = true;
  }

  return sensor->lastTemp + static_cast<float>(sensor->correctInt);
}

float getTempFromTable(Sensor *sensor) {
  int acp = sensor->acp;
  if (acp >= MAX_ACP) acp = MAX_ACP - 1;
  if (acp <= 0) acp = 1;

  sensor->resist = GET_RES(acp);

  // get max or min value
  if (sensor->resist >= ntcResAt(0)) return static_cast<float>(ntcTempAt(0));
  if (sensor->resist <= ntcResAt(NTC_TABLE_SIZE - 1))
    return static_cast<float>(ntcTempAt(NTC_TABLE_SIZE - 1));

  for (size_t i = 0; i + 1 < NTC_TABLE_SIZE; i++) {
    int16_t temp = ntcTempAt(i);
    int32_t res = ntcResAt(i);

    if (sensor->resist > res) continue;
    if (sensor->resist < ntcResAt(i + 1)) continue;

    // interpolate between the two bracketing table rows
    float fraction =
        static_cast<float>(res - sensor->resist) / (res - ntcResAt(i + 1));
    return static_cast<float>(temp) +
           fraction * static_cast<float>(ntcTempAt(i + 1) - temp);
  }
  return 0.0F;
}

void sensorFilterInit(Sensor *sensor, uint8_t pin) {
  sensor->pin = pin;
#if defined(MEDIAN_FILTER) || defined(EMA_FILTER)
  int init = analogRead(pin);
#endif

#ifdef MEDIAN_FILTER
  sensor->median.numNodes = WINDOW_SIZE;
  sensor->median.medianBuffer = sensor->medianBuffer;
  if (MEDIANFILTER_Init(&sensor->median) == 0) {
    for (uint8_t i = 0; i < WINDOW_SIZE; i++)
      MEDIANFILTER_Insert(&sensor->median, init);
  } else {
    sensor->median.numNodes = 0;
  }
#endif
#ifdef EMA_FILTER
  for (uint16_t i = 0; i < (1u << EMA_K); i++) sensor->ema.filter(init);
#endif
}

void constrSensor(Sensor *sensor, uint8_t pin) {
  uint32_t now = millis();
  if (now - sensor->lastSampleMs < TEMP_INTERVAL) {
    return;
  }

  int acp = analogRead(pin);
  sensor->lastSampleMs = now;

  if (sensor->lastAcp >= 0 && abs(acp - sensor->lastAcp) > PERM_DIF) {
    if (sensor->spikeCount < SPIKE_LIMIT) {
      sensor->spikeCount++;
      return;
    }
  } else {
    sensor->spikeCount = 0;
  }
  sensor->lastAcp = acp;

#if defined(EMA_FILTER) && defined(MEDIAN_FILTER)
  uint16_t med =
      sensor->median.numNodes != 0
          ? static_cast<uint16_t>(MEDIANFILTER_Insert(&sensor->median, acp))
          : static_cast<uint16_t>(acp);
  sensor->acp = sensor->ema.filter(med);
#elif defined(EMA_FILTER)
  sensor->acp = sensor->ema.filter(acp);
#elif defined(MEDIAN_FILTER)
  sensor->acp = sensor->median.numNodes != 0
                    ? MEDIANFILTER_Insert(&sensor->median, acp)
                    : acp;
#else
  sensor->acp = acp;
#endif
  sensor->temp = getTemp(sensor);
}

int getIAverageTemp(Sensors *sn) {
  Sensor *all[] = {sn->_s_main, sn->_s_first, sn->_s_second};
  int temp[3];
  int count = 0;
  for (Sensor *sensor : all) {
    if (!sensor->ready) continue;
    temp[count++] = static_cast<int>(sensor->temp);
  }
  if (count == 0) return 0;
  return IntegerGetAverageValue(temp, count);
}

float getFAverageTemp(Sensors *sn) {
  Sensor *all[] = {sn->_s_main, sn->_s_first, sn->_s_second};
  float temp[3];
  int count = 0;
  for (Sensor *sensor : all) {
    if (!sensor->ready) continue;
    temp[count++] = sensor->temp;
  }
  if (count == 0) return 0.0F;
  return FloatGetAverageValue(temp, count);
}