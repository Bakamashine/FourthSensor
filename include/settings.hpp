#pragma once
#include "constants/settings.hpp"
#include "sensor.hpp"

extern int minPermOffset;
extern int maxPermOffset;
extern float hyst;
#ifdef ENABLE_BURNER
extern bool burnerStatus;
#endif
extern int uTemp;

bool checkTempForBurner (Sensors *);