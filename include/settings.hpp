#pragma once

#include "constants/settings.hpp"
#include "sensor.hpp"

#ifdef ENABLE_VALIDATE
inline int minPermOffset = 10;
inline int maxPermOffset = 50;
inline int uTemp = 40;
#endif
inline float hyst = DEFAULT_HYSTERESIS;
#ifdef ENABLE_BURNER
inline bool burnerStatus = false;
#endif

namespace _EEPROM {
#ifdef ENABLE_VALIDATE
void writeMinPermOffset(int);
void readMinPermOffset();
void writeMaxPermOffset(int);
void readMaxPermOffset();
void writeUserTemp(int);
void readUserTemp();
#endif
void writeHyst(float);
void readHyst();

// main method for reading from eeprom
void readFromEEPROM();
}  // namespace _EEPROM
