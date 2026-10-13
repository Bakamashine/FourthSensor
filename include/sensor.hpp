#pragma once
#include "constants/ntcPoint.hpp"
#include "constants/settings.hpp"

#ifdef EMA_FILTER
#include "ema.hpp"
#define EMA_K 2  ///< alpha = 1 / 2^EMA_K
#endif
#ifdef MEDIAN_FILTER
#include "MedianFilter.h"
#define WINDOW_SIZE 5  ///< sliding window length, must be odd and > 1
#endif

/// correction is an offset in degrees, keep it a sane displayable value
#define MIN_CORRECT_INT (-50)
#define MAX_CORRECT_INT 50

struct Sensor {
  int acp;           ///< ADC count (0-1023) after the acp filters
  int lastAcp = -1;  ///< Last accepted raw ADC, -1 until the first reading
  uint8_t spikeCount = 0;  ///< Consecutive samples rejected as glitches
  float temp;              ///< Calculated temperature in degrees Celsius
  float resist;            ///< Calculated sensor resistance in ohms
  uint8_t pin;             ///< Analog pin the sensor is connected to
  uint32_t lastSampleMs;   ///< Timestamp of the last temperature sample (ms)
  float lastTemp;          ///< Last accepted temperature value
  bool ready;              ///< true once the first sample produced a value
  bool increase = false;   ///< direction of the last temperature change
  int correctInt;          ///< Integer temperature correction offset
#if defined(MEDIAN_FILTER) || defined(EMA_FILTER)
  bool filterReady = false;  ///< true once the acp filters were primed
#endif
#ifdef EMA_FILTER
  EMA<EMA_K, uint32_t> ema;  ///< integer EMA over the raw ADC
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

float getTemp(Sensor *);
void constrSensor(Sensor *, uint8_t pin);
void sensorFilterInit(Sensor *sensor, uint8_t pin);

void sensorsLoop();
void sensorSetup();

int16_t ntcTempAt(size_t i);
int32_t ntcResAt(size_t i);

int getIAverageTemp(Sensors *sens);
float getFAverageTemp(Sensors *sens);

namespace Sens {
inline Sensor first;
inline Sensor second;
inline Sensor third;
inline Sensor fourth;

inline Sensors all{&first, &second, &third, &fourth};
}  // namespace Sens