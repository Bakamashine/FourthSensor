#pragma once
#include "constants/ntcPoint.hpp"
#include "ema.hpp"
#define ATTEMPTS 5

/// correction is an offset in degrees, keep it a sane displayable value
#define MIN_CORRECT_INT (-50)
#define MAX_CORRECT_INT 50

/// Structure representing a single NTC temperature sensor.
/// Measures temperature using a voltage divider with a series resistor.
struct Sensor
{
  int acp;          ///< Raw ADC count (0-1023) from the sensor voltage divider
  int lastAcp = -1; ///< Previous raw ADC, -1 until the first reading
  float temp;       ///< Calculated temperature in degrees Celsius
  float resist;     ///< Calculated sensor resistance in ohms
  uint8_t pin;      ///< Analog pin the sensor is connected to
  uint32_t lastSampleMs; ///< Timestamp of the last temperature sample (ms)
  int sampleIdx;         ///< Current index in the samples buffer
  float lastTemp;        ///< Last calculated temperature value
  bool ready; ///< true after the first full ATTEMPTS cycle produced a value
  float samples[ATTEMPTS]; ///< Buffer for temperature samples (for median
                           ///< filtering)
  int correctInt;          ///< Integer temperature correction offset
  EMA<5, uint16_t> ema; ///< integer EMA over the raw ADC, alpha = 1/8
};

struct Sensors
{
  Sensor *_s_main;   ///< Pointer to main sensor structure
  Sensor *_s_first;  ///< Pointer to first reserve sensor structure
  Sensor *_s_second; ///< Pointer to second reserve sensor structure
  Sensor *_s_street; ///< Pointer to street sensor structure
};

float getTemp (Sensor *);
void constrSensor (Sensor *, uint8_t pin);

int16_t ntcTempAt (size_t i);
int32_t ntcResAt (size_t i);

int getIAverageTemp (Sensors *sens);
float getFAverageTemp (Sensors *sens);