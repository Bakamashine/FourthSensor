#include "sensor.hpp"

#include <Arduino.h>
#include <EEPROM.h>
#ifdef DS18_SENSOR
#include <GyverDS18Single.h>
#endif

#include "constants/addr_eeprom.hpp"
#include "constants/ntcPoint.hpp"
#include "constants/pin.hpp"
#include "constants/settings.hpp"
#include "helper.hpp"
#include "settings.hpp"

#define MAX_ACP 1023

#if TEMP_CALCULATION_VARIANT == VIA_RESISTANCE
#define RESISTOR_FROM_SENSOR 2000  // 2kOm
#define GET_RES(value) \
  (RESISTOR_FROM_SENSOR * static_cast<float>(value) / (MAX_ACP - value))
#endif

#define ADC_SAMPLES 8  // raw reads averaged into one sample

#define PERM_DIF 150
#define SPIKE_LIMIT 3

float getTempFromTable(Sensor *);

// static void changeEmaK() {
//   Sens::first.ema.setK(2);
//   Sens::second.ema.setK(4);
//   Sens::third.ema.setK(6);
//   Sens::fourth.ema.setK(7);
// }

int16_t ntcTempAt(size_t i) {
  return static_cast<int16_t>(pgm_read_word(&ntcTable[i].temp_c));
}

// #if TEMP_CALCULATION_VARIANT == VIA_RESISTANCE
// NTCLIST_VA_TYPE ntcValAt(size_t i) {
//   return static_cast<NTCLIST_VA_TYPE>(pgm_read_dword(&ntcTable[i].value));
// }
// #else
// NTCLIST_VA_TYPE ntcValAt(size_t i) {
//   return static_cast<NTCLIST_VA_TYPE>(pgm_read_word(&ntcTable[i].value));
// }
// #endif

NTCLIST_VA_TYPE ntcValAt(size_t i) {
  NTCLIST_VA_TYPE v;
  memcpy_P(&v, &ntcTable[i].value, sizeof(v));
  return v;
}

static float returnTempWithCorInt(float temp, Sensor *sensor) {
  // if (sensor->settings.highBorder + hyst > temp)
  //   return temp + static_cast<float>(sensor->settings.highCorInt);
  // if (sensor->settings.coolBorder - hyst < temp)
  //   return temp + static_cast<float>(sensor->settings.coolCorInt);

  // return temp + static_cast<float>(sensor->settings.defaultCorInt);

  return temp;
};

float getTemp(Sensor *sensor) {
  float average = getTempFromTable(sensor);

  if (!sensor->ready) {
    sensor->lastTemp = average;
    sensor->ready = true;
    return returnTempWithCorInt(sensor->lastTemp, sensor);
  }

  if (average - sensor->lastTemp > hyst || sensor->lastTemp - average > hyst) {
    // sensor->increase = average > sensor->lastTemp;
    sensor->lastTemp = average;
  }

  return returnTempWithCorInt(sensor->lastTemp, sensor);
}

float getTempFromTable(Sensor *sensor) {
#if TEMP_CALCULATION_VARIANT == VIA_RESISTANCE
  NTCLIST_VA_TYPE val = static_cast<NTCLIST_VA_TYPE>(sensor->resist);
#else
  NTCLIST_VA_TYPE val = static_cast<NTCLIST_VA_TYPE>(sensor->acp);
#endif

  int idx = ntcBinarySearchArray(val);
  if (idx == -1) return static_cast<float>(ntcTempAt(0));
  if (idx == -2) return static_cast<float>(ntcTempAt(NTC_TABLE_SIZE - 1));

  int32_t v1 = static_cast<int32_t>(ntcValAt(idx));
  int32_t v2 = static_cast<int32_t>(ntcValAt(idx + 1));
  int16_t t1 = ntcTempAt(idx);
  int16_t t2 = ntcTempAt(idx + 1);

  int32_t dv = v2 - v1;
  if (dv == 0) return static_cast<float>(t1);

  int32_t dx = static_cast<int32_t>(val) - v1;

  // T = t1 + (t2 - t1) * dx / dv
  int32_t num = (static_cast<int32_t>(t2 - t1) * 10) * dx;
  int32_t t_x10 = static_cast<int32_t>(t1) * 10 + (num + dv / 2) / dv;
  return t_x10 * 0.1F;
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
  // uint32_t now = millis();
  // if (now - sensor->lastSampleMs < TEMP_INTERVAL) {
  //   return;
  // }
  // sensor->lastSampleMs = now;

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
    sensor->ema.reset(acp);
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

#if TEMP_CALCULATION_VARIANT == VIA_RESISTANCE
  float adc = static_cast<float>(sensor->acp);
  if (adc >= MAX_ACP) adc = MAX_ACP - 1;
  if (adc < 0) adc = 0;
  sensor->resist = GET_RES(adc);
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
bool getReadySensors(Sensors *sns) {
  return sns->_s_first->ready && sns->_s_second->ready &&
         sns->_s_third->ready && sns->_s_fourth->ready;
}
void sensorSetup() {
  if (getReadySensors(&Sens::all)) return;
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
#ifdef SENSOR_SETT
  readSensSett(&Sens::first.settings, ADDR_SENS1_SETTINGS);
  readSensSett(&Sens::second.settings, ADDR_SENS2_SETTINGS);
  readSensSett(&Sens::third.settings, ADDR_SENS3_SETTINGS);
  readSensSett(&Sens::fourth.settings, ADDR_SENS4_SETTINGS);
#ifdef EMA_FILTER
  Sens::first.ema.setK(static_cast<uint8_t>(Sens::first.settings.ema_k));
  Sens::second.ema.setK(static_cast<uint8_t>(Sens::second.settings.ema_k));
  Sens::third.ema.setK(static_cast<uint8_t>(Sens::third.settings.ema_k));
  Sens::fourth.ema.setK(static_cast<uint8_t>(Sens::fourth.settings.ema_k));
#endif
#endif
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

#ifdef SENSOR_SETT
void writeSensSett(SensorSettings *ss, int addr) { EEPROM.put(addr, *ss); }

void readSensSett(SensorSettings *ss, int addr) {
  SensorSettings stored;
  EEPROM.get(addr, stored);
  if (stored.addr_magic == SENS_SETTINGS_MAGIC) *ss = stored;
}
#endif