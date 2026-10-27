#include "settings.hpp"

#include <EEPROM.h>

#include "constants/addr_eeprom.hpp"

namespace _EEPROM {

#ifdef ENABLE_VALIDATE
// keep the user temperature inside the editable range after another setting
// changed its bounds
static void clampUserTemp() {
  // min user temperature
  const int lo = minUserTemp();

  // max user temperature
  const int hi = maxUserTemp();
  if (lo > hi || (uTemp >= lo && uTemp <= hi)) return;
  uTemp = (uTemp < lo) ? lo : hi;
  EEPROM.put(ADDR_USER_TEMP, uTemp);
}
#endif

// hysteresis
void writeHyst(int _hyst) {
  EEPROM.put(ADDR_HYST, _hyst);
#ifdef ENABLE_VALIDATE
  clampUserTemp();
#endif
}

void readHyst() {
  int v = 0;
  EEPROM.get(ADDR_HYST, v);
  if (v < MIN_HYSTERESIS || v > MAX_HYSTERESIS) return;
  hyst = v;
}

// main func
void readFromEEPROM() {
  readHyst();
#ifdef ENABLE_VALIDATE
  _EEPROM::readMaxPermOffset();
  _EEPROM::readMinPermOffset();
  if (minPermOffset >= maxPermOffset) {
    minPermOffset = DEF_MIN_PERM_OFFSET;
    maxPermOffset = DEF_MAX_PERM_OFFSET;
  }
  _EEPROM::readUserTemp();
#endif
}

// minimum permission offset
void writeMinPermOffset(int _min_perm) {
  if (maxPermOffset <= _min_perm) return;
  minPermOffset = _min_perm;
  EEPROM.put(ADDR_MIN_PERM_OFFSET, _min_perm);
  clampUserTemp();
}

void readMinPermOffset() {
  int v = 0;
  EEPROM.get(ADDR_MIN_PERM_OFFSET, v);
  if (v < MIN_PERM_OFFSET || v >= maxPermOffset) return;
  minPermOffset = v;
}

// maximum permission offset
void writeMaxPermOffset(int _max_perm) {
  if (minPermOffset >= _max_perm) return;
  maxPermOffset = _max_perm;
  EEPROM.put(ADDR_MAX_PERM_OFFSET, _max_perm);
  clampUserTemp();
}

void readMaxPermOffset() {
  int v = 0;
  EEPROM.get(ADDR_MAX_PERM_OFFSET, v);
  if (v <= MIN_PERM_OFFSET || v > MAX_PERM_OFFSET) return;
  maxPermOffset = v;
}

// user temperature
void writeUserTemp(int _userTemp) {
  if (_userTemp < minUserTemp() || _userTemp > maxUserTemp()) return;
  uTemp = _userTemp;
  EEPROM.put(ADDR_USER_TEMP, _userTemp);
}

void readUserTemp() {
  int v = 0;
  EEPROM.get(ADDR_USER_TEMP, v);
  const int lo = minUserTemp();
  const int hi = maxUserTemp();
  if (lo > hi) {
    // hysteresis leaves no room for a valid user temperature
    uTemp = (minPermOffset + maxPermOffset) >> 1;
    return;
  }
  if (v < lo || v > hi)
    v = (DEF_USER_TEMP < lo) ? lo : (DEF_USER_TEMP > hi) ? hi : DEF_USER_TEMP;
  uTemp = v;
}

void writeDefaultValue(Sensors &sns) {
  (void)sns;
  hyst = DEFAULT_HYSTERESIS;
  writeHyst(hyst);
#ifdef ENABLE_VALIDATE
  writeMaxPermOffset(DEF_MAX_PERM_OFFSET);
  writeMinPermOffset(DEF_MIN_PERM_OFFSET);
  writeUserTemp(DEF_USER_TEMP);
#endif
#ifdef SENSOR_SETT
  SensorSettings def_sett;
  def_sett.ema_k = DEF_EMA_K;
  def_sett.addr_magic = SENS_SETTINGS_MAGIC;
  sns._s_first->settings = def_sett;
  sns._s_second->settings = def_sett;
  sns._s_third->settings = def_sett;
  sns._s_fourth->settings = def_sett;
  writeSensSett(sns._s_first->settings, ADDR_SENS1_SETTINGS);
  writeSensSett(sns._s_second->settings, ADDR_SENS2_SETTINGS);
  writeSensSett(sns._s_third->settings, ADDR_SENS3_SETTINGS);
  writeSensSett(sns._s_fourth->settings, ADDR_SENS4_SETTINGS);
#endif
#ifdef EMA_FILTER
  sns._s_first->ema.setK(DEF_EMA_K);
  sns._s_second->ema.setK(DEF_EMA_K);
  sns._s_third->ema.setK(DEF_EMA_K);
  sns._s_fourth->ema.setK(DEF_EMA_K);
#endif
}

}  // namespace _EEPROM