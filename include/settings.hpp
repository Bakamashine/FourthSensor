#pragma once

#include "constants/settings.hpp"
#include "sensor.hpp"
#define DEF_MAX_PERM_OFFSET 50
#define DEF_MIN_PERM_OFFSET 10
#define DEF_USER_TEMP 40
#define MAX_PERM_OFFSET 100
#define MIN_PERM_OFFSET -34

#ifdef ENABLE_VALIDATE
inline int maxPermOffset = DEF_MAX_PERM_OFFSET;
inline int minPermOffset = DEF_MIN_PERM_OFFSET;
inline int uTemp = DEF_USER_TEMP;
#endif
// inline bool burnStatus = false;
inline float hyst = DEFAULT_HYSTERESIS;
#ifdef ENABLE_BURNER
inline bool burnerStatus = false;
void setBurnerStatus(bool);
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

void writeDefaultValue(Sensors*);
}  // namespace _EEPROM
