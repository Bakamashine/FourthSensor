#pragma once

#include "constants/settings.hpp"
#include "sensor.hpp"
#define DEF_MAX_PERM_OFFSET 50
#define DEF_MIN_PERM_OFFSET 10
#define DEF_USER_TEMP 30
#define MAX_PERM_OFFSET 100
#define MIN_PERM_OFFSET -34

#ifdef ENABLE_VALIDATE
inline int maxPermOffset = DEF_MAX_PERM_OFFSET;
inline int minPermOffset = DEF_MIN_PERM_OFFSET;
inline int uTemp = DEF_USER_TEMP;
#endif
inline int hyst = DEFAULT_HYSTERESIS;

#ifdef ENABLE_VALIDATE
inline int minUserTemp() { return minPermOffset + hyst + 1; }
inline int maxUserTemp() { return maxPermOffset - hyst - 1; }
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
void writeHyst(int);
void readHyst();

// main method for reading from eeprom
void readFromEEPROM();

void writeDefaultValue(Sensors &);
}  // namespace _EEPROM
