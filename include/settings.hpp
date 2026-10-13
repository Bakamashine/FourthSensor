#pragma once
#include "constants/settings.hpp"
#include "sensor.hpp"

inline int minPermOffset = 10;
inline int maxPermOffset = 50;
inline float hyst = DEFAULT_HYSTERESIS;
#ifdef ENABLE_BURNER
inline bool burnerStatus = false;
#endif
inline int uTemp = 40;

bool checkTempForBurner(Sensors *);