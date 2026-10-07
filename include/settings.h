#pragma once
#include "constants/settings.h"
#include "sensor.h"

extern int minPermOffset;
extern int maxPermOffset;
extern float hyst;
extern bool burnerStatus;
extern int uTemp;

bool checkTempForBurner (Sensors *);