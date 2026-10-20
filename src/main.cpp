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
#include "led.hpp"
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
  PRINT_DEBUG("[ERROR_H] errorCode: ", Val::errorCode);
  PRINT_DEBUG("[SETTINGS_H] minPermOffset: ", minPermOffset);
  PRINT_DEBUG("[SETTINGS_H] maxPermOffset: ", maxPermOffset);
#endif
}

void setup() {
  wdt_disable();
  Serial.begin(9600);
  Wire.setWireTimeout(3000, true);
  UI::u8g2.begin();

  // sensorSetup();
  btnSetup();
#ifdef LED_BUILD_STATUS
  ledSetup();
#endif
  _EEPROM::readFromEEPROM();
  wdt_enable(WDTO_4S);
}

void loop() {
#ifdef LED_DEBUG
  ledDebug();
#endif
  wdt_reset();
  btnTickBurner();

#ifdef BURNER_PREPARE
  Burner::prepare();
#endif
#ifdef ENABLE_VALIDATE
  Val::checkError();
  if (!Val::systemHalted) {
#endif

    sensorSetup();  // only once
    btnTick();
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
#ifdef ENABLE_VALIDATE
  }
#endif
  UI::draw();

#ifdef ENABLE_VALIDATE
  // validate
  Val::pipeline(&Sens::all);
#endif

#ifdef ENABLE_BURNER
  if (!Val::systemHalted) Burner::burnerSystem();
#endif
}
