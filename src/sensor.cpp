#include "sensor.hpp"

#include <Arduino.h>

#include "constants/ntcPoint.hpp"
#include "constants/pin.hpp"
#include "constants/settings.hpp"
#include "helper.hpp"
#include "settings.hpp"

#define MAX_ACP 1023
#define RESISTOR_FROM_SENSOR 2000  // 2kOm

#define GET_RES(value) \
  (RESISTOR_FROM_SENSOR * static_cast<float>(value) / (MAX_ACP - value))
#define TEMP_INTERVAL 100  // ms between samples
#define ADC_SAMPLES 8      // raw reads averaged into one sample

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
  float average = getTempFromTable(sensor);

  if (!sensor->ready) {
    sensor->lastTemp = average;
    sensor->ready = true;
    return sensor->lastTemp + static_cast<float>(sensor->correctInt);
  }

  if (average - sensor->lastTemp > hyst || sensor->lastTemp - average > hyst) {
    sensor->increase = average > sensor->lastTemp;
    sensor->lastTemp = average;
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
#ifdef MEDIAN_FILTER
  sensor->median.numNodes = WINDOW_SIZE;
  sensor->median.medianBuffer = sensor->medianBuffer;
  if (MEDIANFILTER_Init(&sensor->median) != 0) sensor->median.numNodes = 0;
#endif
}

void constrSensor(Sensor *sensor, uint8_t pin) {
  uint32_t now = millis();
  if (now - sensor->lastSampleMs < TEMP_INTERVAL) {
    return;
  }
  sensor->lastSampleMs = now;

  uint32_t sum = 0;
  for (uint8_t i = 0; i < ADC_SAMPLES; i++) sum += analogRead(pin);
  int acp = static_cast<int>(sum / ADC_SAMPLES);

  if (sensor->lastAcp >= 0 && abs(acp - sensor->lastAcp) > PERM_DIF) {
    if (sensor->spikeCount < SPIKE_LIMIT) {
      sensor->spikeCount++;
      return;
    }
  } else {
    sensor->spikeCount = 0;
  }
  sensor->lastAcp = acp;

#if defined(MEDIAN_FILTER) || defined(EMA_FILTER)
  // prime with the first real sample of this sensor: seeding from a setup time
  // analogRead would copy the previous channel's voltage, i.e. another
  // sensor's value, into this sensor's filter state
  if (!sensor->filterReady) {
#ifdef MEDIAN_FILTER
    if (sensor->median.numNodes != 0)
      for (uint8_t i = 0; i < WINDOW_SIZE; i++)
        MEDIANFILTER_Insert(&sensor->median, acp);
#endif
#ifdef EMA_FILTER
    for (uint16_t i = 0; i < (1u << EMA_K); i++) sensor->ema.filter(acp);
#endif
    sensor->filterReady = true;
  }
#endif

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
  int temp[] = {static_cast<int>(sn->_s_first->temp),
                static_cast<int>(sn->_s_second->temp),
                static_cast<int>(sn->_s_third->temp)};
  return IntegerGetAverageValue(temp, 3);
}

float getFAverageTemp(Sensors *sn) {
  float temp[] = {sn->_s_first->temp, sn->_s_second->temp, sn->_s_third->temp};
  return FloatGetAverageValue(temp, 3);
}

void sensorsLoop() {
  constrSensor(&Sens::first, FIRST_SENSOR_PIN);
  constrSensor(&Sens::second, SECOND_RESERVE_SENSOR_PIN);
  constrSensor(&Sens::third, THIRD_RESERVE_SENSOR_PIN);
  constrSensor(&Sens::fourth, FOURTH_SENSOR_PIN);
}

void sensorSetup() {
  pinMode(FIRST_SENSOR_PIN, INPUT);
  pinMode(SECOND_RESERVE_SENSOR_PIN, INPUT);
  pinMode(THIRD_RESERVE_SENSOR_PIN, INPUT);
  pinMode(FOURTH_SENSOR_PIN, INPUT);

  sensorFilterInit(&Sens::first, FIRST_SENSOR_PIN);
  sensorFilterInit(&Sens::second, SECOND_RESERVE_SENSOR_PIN);
  sensorFilterInit(&Sens::third, THIRD_RESERVE_SENSOR_PIN);
  sensorFilterInit(&Sens::fourth, FOURTH_SENSOR_PIN);
}