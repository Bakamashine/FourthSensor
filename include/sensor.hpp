#pragma once
#include "constants/ntcPoint.hpp"
#include "constants/pin.hpp"
#include "constants/settings.hpp"
#ifdef EMA_FILTER
#include "ema.hpp"
#endif
#ifdef DS18_SENSOR
#include <GyverDS18.h>
#endif

#ifdef MEDIAN_FILTER
#include "MedianFilter.h"
#define WINDOW_SIZE 5  ///< sliding window length, must be odd and > 1
#endif

#define MIN_CORRECT_INT (-50)
#define MAX_CORRECT_INT 50

#ifdef SENSOR_SETT
#define SENS_SETTINGS_MAGIC 0x5A02

struct SensorSettings {
  uint16_t addr_magic =
      SENS_SETTINGS_MAGIC;  ///< validity marker, written as-is
  int ema_k = DEF_EMA_K;
};
#endif

struct Sensor {
  unsigned int acp;  ///< ADC count (0-1023) after the acp filters
  int lastAcp = -1;  ///< Last accepted raw ADC, -1 until the first reading
  uint8_t spikeCount = 0;  ///< Consecutive samples rejected as glitches
  float temp;              ///< Calculated temperature in degrees Celsius
#if TEMP_CALCULATION_VARIANT == VIA_RESISTANCE
  float resist;  ///< Calculated sensor resistance in ohms
#endif
  uint8_t pin;  ///< Analog pin the sensor is connected to
  // float lastTemp;  ///< Last accepted temperature value
  bool ready;  ///< true once the first sample produced a value

#ifdef SENSOR_SETT
  /**
   * SensorSettings contains the correct integer value for temperature
   * compensation. if the temperature is higher (e.g >50 C), highCorInt is used
   * overwise, defaultCorInt is used
   * if the temperature is lower (e.g <-10 C), coolCorInt is used
   */
  SensorSettings settings;
#endif

#if defined(MEDIAN_FILTER) || defined(EMA_FILTER)
  bool filterReady = false;  ///< true once the acp filters were primed
#endif
#ifdef EMA_FILTER
  EMA<uint32_t> ema;  ///< integer EMA over the raw ADC
#endif
#ifdef MEDIAN_FILTER
  MedianFilter_t median;  ///< sliding window median over the raw ADC
  MedianFilterNode_t medianBuffer[WINDOW_SIZE];  ///< caller-owned nodes
#endif
};

struct Sensors {
  Sensor *_s_first;   ///< Pointer to first sensor structure
  Sensor *_s_second;  ///< Pointer to second reserve sensor structure
  Sensor *_s_third;   ///< Pointer to third reserve sensor structure
  Sensor *_s_fourth;  ///< Pointer to fourth sensor structure
};

float getTemp(Sensor &);
void constrSensor(Sensor &, uint8_t pin);
void sensorFilterInit(Sensor &sensor, uint8_t pin);

void sensorsLoop();
void sensorSetup();
void DS18Setup();
bool getReadySensors(Sensors &sn);

int16_t ntcTempAt(size_t i);
NTCLIST_VA_TYPE ntcValAt(size_t i);

int getIAverageTemp(Sensors &sens);
float getFAverageTemp(Sensors &sens);

#ifdef DS18_SENSOR
float readFromDS18();
#endif

#ifdef SENSOR_SETT
// write sensor settings to eeprom memory
void writeSensSett(SensorSettings &, int addr);

// get the struct from eeprom memory
void readSensSett(SensorSettings &ss, int addr);

#endif

namespace Sens {
inline Sensor first;
inline Sensor second;
inline Sensor third;
inline Sensor fourth;

#ifdef DS18_SENSOR
inline GyverDS18Single ds(DS18_PIN);
inline float ds18Temp = 0.0F;
#endif

inline Sensors all{&first, &second, &third, &fourth};
}  // namespace Sens