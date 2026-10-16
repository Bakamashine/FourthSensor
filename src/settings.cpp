#include "settings.hpp"

#include <EEPROM.h>

#include "constants/addr_eeprom.hpp"

namespace _EEPROM {

// hysteresis
void writeHyst(float _hyst) { EEPROM.put(ADDR_HYST, _hyst); }

void readHyst() {
  float v = 0;
  EEPROM.get(ADDR_HYST, v);
  if (v <= 0.0F || v > 100.0F) return;
  hyst = v;
}

// main func
void readFromEEPROM() {
  readHyst();
#ifdef ENABLE_VALIDATE
  _EEPROM::readMinPermOffset();
  _EEPROM::readMaxPermOffset();
  _EEPROM::readUserTemp();
#endif
}

// minimum permission offset
void writeMinPermOffset(int _min_perm) {
  if (maxPermOffset <= _min_perm) return;
  minPermOffset = _min_perm;
  EEPROM.put(ADDR_MIN_PERM_OFFSET, _min_perm);
}

void readMinPermOffset() {
  int v = 0;
  EEPROM.get(ADDR_MIN_PERM_OFFSET, v);
  if (v <= minPermOffset) return;
  minPermOffset = v;
}

// maximum permission offset
void writeMaxPermOffset(int _max_perm) {
  if (minPermOffset >= _max_perm) return;
  maxPermOffset = _max_perm;
  EEPROM.put(ADDR_MAX_PERM_OFFSET, _max_perm);
}

void readMaxPermOffset() {
  int v = 0;
  EEPROM.get(ADDR_MAX_PERM_OFFSET, v);
  if (v <= 0) return;
  maxPermOffset = v;
}

// user temperature
void writeUserTemp(int _userTemp) {
  if (_userTemp <= minPermOffset + hyst || _userTemp >= maxPermOffset - hyst)
    return;
  uTemp = _userTemp;
  EEPROM.put(ADDR_USER_TEMP, _userTemp);
}

void readUserTemp() {
  int v = 0;
  EEPROM.get(ADDR_USER_TEMP, v);
  if (v <= 0) return;
  uTemp = v;
}

}  // namespace _EEPROM
