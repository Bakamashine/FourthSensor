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

void writeDefaultValue(Sensors* sns) {
  hyst = DEFAULT_HYSTERESIS;
  writeHyst(hyst);
#ifdef ENABLE_VALIDATE
  writeMaxPermOffset(DEF_MAX_PERM_OFFSET);
  writeMinPermOffset(DEF_MIN_PERM_OFFSET);
  writeUserTemp(DEF_USER_TEMP);
#endif
  SensorSettings def_sett;
  def_sett.ema_k = DEF_EMA_K;
  def_sett.addr_magic = SENS_SETTINGS_MAGIC;
  sns->_s_first->settings = def_sett;
  sns->_s_second->settings = def_sett;
  sns->_s_third->settings = def_sett;
  sns->_s_fourth->settings = def_sett;
  writeSensSett(&sns->_s_first->settings, ADDR_SENS1_SETTINGS);
  writeSensSett(&sns->_s_second->settings, ADDR_SENS2_SETTINGS);
  writeSensSett(&sns->_s_third->settings, ADDR_SENS3_SETTINGS);
  writeSensSett(&sns->_s_fourth->settings, ADDR_SENS4_SETTINGS);
#ifdef EMA_FILTER
  sns->_s_first->ema.setK(DEF_EMA_K);
  sns->_s_second->ema.setK(DEF_EMA_K);
  sns->_s_third->ema.setK(DEF_EMA_K);
  sns->_s_fourth->ema.setK(DEF_EMA_K);
#endif
}

}  // namespace _EEPROM

#ifdef ENABLE_BURNER
void setBurnerStatus(bool st) {
  burnerStatus = st;
  digitalWrite(BURNER_PIN, st);
}
#endif