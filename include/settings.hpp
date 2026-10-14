#pragma once
#include "constants/settings.hpp"
#include "sensor.hpp"

#ifdef ENABLE_VALIDATE
inline int minPermOffset = 10;
inline int maxPermOffset = 50;

void writeMinPermOffset(int);
int getMinPermOffset();
void writeMaxPermOffset(int);
int getMaxPermOffset();
#endif
inline float hyst = DEFAULT_HYSTERESIS;
#ifdef ENABLE_BURNER
inline bool burnerStatus = false;
#endif
inline int uTemp = 40;

void writeHystToEEPROM(float, int frac);
void readHystFromEEPROM(int frac);

void readFromEEPROM();
