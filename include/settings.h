#pragma once
#include "constants/settings.h"
#include "sensor.h"

extern int minPermOffset;
extern int maxPermOffset;
extern float hyst;
#ifdef ENABLE_BURNER
extern bool burnerStatus;
#endif
extern int uTemp;

bool checkTempForBurner (Sensors *);