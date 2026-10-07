#pragma once
#include "constants/settings.h"
#include "sensor.h"

extern int minPermOffset;
extern int maxPermOffset;
extern int hyst;
extern bool burnerStatus;
extern int userTemperature;

bool checkBurner (Sensors*);