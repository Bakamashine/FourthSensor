#ifndef SENSOR_UI
#define SENSOR_UI
#include "constants/ntcPoint.h"
#define ATTEMPTS 5

typedef struct Sensor
{
  int acp;
  float temp;
  float resist;
  uint8_t pin;
  uint32_t lastSampleMs;
  int sampleIdx;
  float lastTemp;
  float samples[ATTEMPTS];
  int correctInt;
  float _adcFilter;
} Sensor;

typedef struct Sensors
{
  Sensor *_s_main;
  Sensor *_s_first;
  Sensor *_s_second;
  Sensor *_s_street;
} Sensors;

float getTemp (Sensor *);
void constrSensor (Sensor *, uint8_t pin);

int16_t ntcTempAt (size_t i);
int32_t ntcResAt (size_t i);

#endif