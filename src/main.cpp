#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <avr/wdt.h>

#include "OneButtonTiny.h"
#include "button_callbacks.hpp"
#include "constants/settings.hpp"
#include "macro/debugUi.hpp"
#include "page.hpp"
#include "sensor.hpp"
#include "settings.hpp"
#include "ui/main_ui.hpp"
#include "ui/menu.hpp"
#include "ui/menu_items.hpp"
#ifdef ENABLE_COMMANDS
#include "command.hpp"
void handleCommand();
#endif
#ifdef ENABLE_VALIDATE
#include "validate.hpp"
#endif
#ifdef ENABLE_BURNER
#include "burner.hpp"
#endif
using namespace DeepMenu;
void debugPrint() {
  Serial.println("=====DEBUG INPUT====");
  // acp
  PRINT_DEBUG("[ACP] s_first: ", Sens::first.acp);
  PRINT_DEBUG("[ACP] s_second: ", Sens::second.acp);
  PRINT_DEBUG("[ACP] s_third: ", Sens::third.acp);
  PRINT_DEBUG("[ACP] s_fourth: ", Sens::fourth.acp);

  // temperature
  PRINT_DEBUG("[TEMP] s_first: ", Sens::first.temp);
  PRINT_DEBUG("[TEMP] s_second: ", Sens::second.temp);
  PRINT_DEBUG("[TEMP] s_third: ", Sens::third.temp);
  PRINT_DEBUG("[TEMP] s_fourth: ", Sens::fourth.temp);
#ifdef DS18_SENSOR
  PRINT_DEBUG("[TEMP] DS18: ", Sens::ds18Temp);
#endif

// other variables
#ifdef ENABLE_BURNER
  PRINT_DEBUG("[SETTINGS_H] burnerStatus: ", burnerStatus);

#endif

  PRINT_DEBUG("[SETTINGS_H] Hysteresis: ", hyst);
  PRINT_DEBUG("[MENU_UI_H] isValueOpen: ", isDeepMenuOpen);
  PRINT_DEBUG("[MENU_UI_H] _baseMenuSelected: ", _baseMenuSelected);
  PRINT_DEBUG("[MENU_UI_H] _deepMenuSelected: ", _deepMenuSelected);
  PRINT_DEBUG("[PAGE_H] currentPage: ", currentPage);
#ifdef ENABLE_VALIDATE
  PRINT_DEBUG("[ERROR_H] errorCode: ", Validate::errorCode);
  PRINT_DEBUG("[SETTINGS_H] minPermOffset: ", minPermOffset);
  PRINT_DEBUG("[SETTINGS_H] maxPermOffset: ", maxPermOffset);
#endif
}

#ifdef LED_DEBUG
inline void ledDebug() {
  digitalWrite(ERROR_PIN, HIGH);
  digitalWrite(BURNER_PIN, LOW);
  delay(100);
  digitalWrite(ERROR_PIN, LOW);
  digitalWrite(BURNER_PIN, HIGH);
}
#endif

void setup() {
  wdt_disable();
  Serial.begin(9600);
  Wire.setWireTimeout(3000, true);
  UI::u8g2.begin();

  sensorSetup();
  btnSetup();
#if LED_BUILD_STATUS
  ledSetup();
#endif
  _EEPROM::readFromEEPROM();
  wdt_enable(WDTO_4S);
}

void loop() {
  wdt_reset();
  btnTick();
#ifdef LED_DEBUG
  ledDebug();
#endif
  sensorsLoop();

#ifdef DS18_SENSOR
  readFromDS18();
#endif
#ifdef DEBUG
  debugPrint();
#endif

#ifdef ENABLE_COMMANDS
  handleCommand();
#endif
  UI::draw();

#ifdef ENABLE_VALIDATE
  // validate
  Validate::pipeline(&Sens::all);
  if (Validate::errorCode > 0) Validate::haltSystem();
#endif

#ifdef ENABLE_BURNER
  Burner::burnerSystem();
#endif
}
