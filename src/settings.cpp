#include "settings.hpp"

#include <EEPROM.h>

#include "constants/addr_eeprom.hpp"

static int fracMult(int frac) {
  int mult = 100;
  if (frac > -1) {
    mult *= frac;
  }
  return mult;
}

void writeHystToEEPROM(float _hyst, int frac) {
  int writeableHyst = _hyst * fracMult(frac);
  EEPROM.write(ADDR_HYST, writeableHyst);
}

void readHystFromEEPROM(int frac) {
  auto v_fromEEPROM = EEPROM.read(ADDR_HYST);
  if (v_fromEEPROM <= 0.0F) return;
  // divide in float: integer division would truncate 7 / 100 to 0
  hyst = static_cast<float>(v_fromEEPROM) / fracMult(frac);
  // return trueHyst;
}

void readFromEEPROM() { readHystFromEEPROM(-1); }