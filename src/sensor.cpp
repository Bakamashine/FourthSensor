#include "sensor.hpp"

#include <Arduino.h>
#include <EEPROM.h>
#include <GyverDS18Single.h>

#include "constants/addr_eeprom.hpp"
#include "constants/ntcPoint.hpp"
#include "constants/pin.hpp"
#include "constants/settings.hpp"
#include "helper.hpp"
#include "settings.hpp"

#define MAX_ACP 1023

#if TEMP_CALCULATION_VARIANT == VIA_RESISTANCE
#define GET_RES(value) \
  (RESISTOR_FROM_SENSOR * static_cast<float>(value) / (MAX_ACP - value))
#define RESISTOR_FROM_SENSOR 2000  // 2kOm
#endif
#define TEMP_INTERVAL 100  // ms between samples
#define ADC_SAMPLES 8      // raw reads averaged into one sample

#define PERM_DIF 150
#define SPIKE_LIMIT 3

float getTempFromTable(Sensor *);

static void changeEmaK() {
  Sens::first.ema.setK(2);
  Sens::second.ema.setK(4);
  Sens::third.ema.setK(6);
  Sens::fourth.ema.setK(7);
}

int16_t ntcTempAt(size_t i) {
  return static_cast<int16_t>(pgm_read_word(&ntcTable[i].temp_c));
}

#if TEMP_CALCULATION_VARIANT == VIA_RESISTANCE
uint32_t ntcResAt(size_t i) {
  return static_cast<uint32_t>(pgm_read_dword(&ntcTable[i].resistance));
}
#else
uint16_t ntcAcpAt(size_t i) {
  return static_cast<uint16_t>(pgm_read_word(&ntcTable[i].acp));
}
#endif

static float returnTempWithCorInt(float temp, Sensor *sensor) {
  if (sensor->settings.highBorder + hyst > temp)
    return temp + static_cast<float>(sensor->settings.highCorInt);
  if (sensor->settings.coolBorder - hyst < temp)
    return temp + static_cast<float>(sensor->settings.coolCorInt);

  return temp + static_cast<float>(sensor->settings.defaultCorInt);
};

float getTemp(Sensor *sensor) {
  float average = getTempFromTable(sensor);

  if (!sensor->ready) {
    sensor->lastTemp = average;
    sensor->ready = true;
    return returnTempWithCorInt(sensor->lastTemp, sensor);
  }

  if (average - sensor->lastTemp > hyst || sensor->lastTemp - average > hyst) {
    sensor->increase = average > sensor->lastTemp;
    sensor->lastTemp = average;
  }

  return returnTempWithCorInt(sensor->lastTemp, sensor);
}

float getTempFromTable(Sensor *sensor) {
#if TEMP_CALCULATION_VARIANT == VIA_RESISTANCE
  int acp = sensor->acp;
  if (acp >= MAX_ACP) acp = MAX_ACP - 1;
  if (acp <= 0) acp = 1;
  sensor->resist = GET_RES(acp);

  // clamp to the table ends
  if (sensor->resist >= static_cast<float>(ntcResAt(0)))
    return static_cast<float>(ntcTempAt(0));
  if (sensor->resist <= static_cast<float>(ntcResAt(NTC_TABLE_SIZE - 1)))
    return static_cast<float>(ntcTempAt(NTC_TABLE_SIZE - 1));

  for (size_t i = 0; i + 1 < NTC_TABLE_SIZE; i++) {
    if (sensor->resist > static_cast<float>(ntcResAt(i))) continue;
    if (sensor->resist < static_cast<float>(ntcResAt(i + 1))) continue;

    int32_t r1 = static_cast<int32_t>(ntcResAt(i));
    int32_t r2 = static_cast<int32_t>(ntcResAt(i + 1));
    int16_t t1 = ntcTempAt(i);
    int16_t t2 = ntcTempAt(i + 1);

    // T(x) = t1 + (x - r1) * k
    float k = static_cast<float>(t2 - t1) / static_cast<float>(r2 - r1);
    return static_cast<float>(t1) +
           (sensor->resist - static_cast<float>(r1)) * k;
  }
#else
  if (sensor->acp >= ntcAcpAt(0)) return static_cast<float>(ntcTempAt(0));
  if (sensor->acp <= ntcAcpAt(NTC_TABLE_SIZE - 1))
    return static_cast<float>(ntcTempAt(NTC_TABLE_SIZE - 1));
  for (size_t i = 0; i + 1 < NTC_TABLE_SIZE; i++) {
    // int16_t temp = ntcTempAt(i);
    if (sensor->acp > ntcAcpAt(i)) continue;
    if (sensor->acp < ntcAcpAt(i + 1)) continue;
    // y = k*x+b
    // k = (y2 - y1)/(x2-x1)
    // b = acp?
    //

    int32_t a1 = ntcAcpAt(i);
    int32_t a2 = ntcAcpAt(i + 1);
    int16_t t1 = ntcTempAt(i);
    int16_t t2 = ntcTempAt(i + 1);

    // T(x) = t1 + (x - a1) * k
    float k = static_cast<float>(t2 - t1) / static_cast<float>(a2 - a1);
    return static_cast<float>(t1) +
           (static_cast<int32_t>(sensor->acp) - a1) * k;
  }
#endif

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
#ifdef DS18_SENSOR
  DS18Setup();
#endif
  // changeEmaK();

  // read eeprom
  readSensSett(&Sens::first.settings, ADDR_SENS1_SETTINGS);
  readSensSett(&Sens::second.settings, ADDR_SENS2_SETTINGS);
  readSensSett(&Sens::third.settings, ADDR_SENS3_SETTINGS);
  readSensSett(&Sens::fourth.settings, ADDR_SENS4_SETTINGS);
}

#ifdef DS18_SENSOR
void DS18Setup() { pinMode(DS18_PIN, INPUT); }

float readFromDS18() {
  if (!Sens::ds.tick()) {
    Sens::ds18Temp = Sens::ds.getTemp();
  }
  return Sens::ds18Temp;
}

#endif

void writeSensSett(SensorSettings *ss, int addr) { EEPROM.put(addr, *ss); }

void readSensSett(SensorSettings *ss, int addr) {
  SensorSettings stored;
  EEPROM.get(addr, stored);
  if (stored.magic == SENS_SETTINGS_MAGIC) *ss = stored;
}