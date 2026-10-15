#include <Arduino.h>
#include <U8g2lib.h>

#include "OneButtonTiny.h"
#include "button_callbacks.hpp"
#include "constants/constants.hpp"
#include "constants/pin.hpp"
#include "constants/settings.hpp"
#include "macro/debugUi.hpp"
#include "page.hpp"
#include "sensor.hpp"
#include "settings.hpp"
#include "ui/main_ui.hpp"
#include "ui/menu.hpp"
#ifdef ENABLE_COMMANDS
#include "command.hpp"
void handleCommand();
#endif
#ifdef ENABLE_VALIDATE
#include "validate.hpp"
void haltSystem();
int systemHalted = 0;
#endif

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

  // correct integer
  PRINT_DEBUG("[CORRECT INT] first high correct int: ",
              Sens::first.settings.highCorInt);
  PRINT_DEBUG("[CORRECT INT] first cool correct int: ",
              Sens::first.settings.coolCorInt);
  PRINT_DEBUG("[CORRECT INT] second high correct int: ",
              Sens::second.settings.highCorInt);
  PRINT_DEBUG("[CORRECT INT] second cool correct int: ",
              Sens::second.settings.coolCorInt);
  PRINT_DEBUG("[CORRECT INT] third high correct int: ",
              Sens::third.settings.highCorInt);
  PRINT_DEBUG("[CORRECT INT] third cool correct int: ",
              Sens::third.settings.coolCorInt);
  PRINT_DEBUG("[CORRECT INT] fourth high correct int: ",
              Sens::fourth.settings.highCorInt);
  PRINT_DEBUG("[CORRECT INT] fourth cool correct int: ",
              Sens::fourth.settings.coolCorInt);

  // temperature border
  PRINT_DEBUG("[TEMP BORDER] first high temperature: ",
              Sens::first.settings.highBorder);
  PRINT_DEBUG("[TEMP BORDER] first cool temperature: ",
              Sens::first.settings.coolBorder);
  PRINT_DEBUG("[TEMP BORDER] second high temperature: ",
              Sens::second.settings.highBorder);
  PRINT_DEBUG("[TEMP BORDER] second cool temperature: ",
              Sens::second.settings.coolBorder);
  PRINT_DEBUG("[TEMP BORDER] third high temperature: ",
              Sens::third.settings.highBorder);
  PRINT_DEBUG("[TEMP BORDER] third cool temperature: ",
              Sens::third.settings.coolBorder);
  PRINT_DEBUG("[TEMP BORDER] fourth high temperature: ",
              Sens::fourth.settings.highBorder);
  PRINT_DEBUG("[TEMP BORDER] fourth cool temperature: ",
              Sens::fourth.settings.coolBorder);

// other variables
#ifdef ENABLE_BURNER
  PRINT_DEBUG("[SETTINGS_H] burnerStatus: ", burnerStatus);

#endif

  PRINT_DEBUG("[SETTINGS_H] Hysteresis: ", hyst);
  PRINT_DEBUG("[MENU_UI_H] isValueOpen: ", isValueOpen);
  PRINT_DEBUG("[MENU_UI_H] _selected: ", _selected);
  PRINT_DEBUG("[PAGE_H] currentPage: ", currentPage);
#ifdef ENABLE_VALIDATE
  PRINT_DEBUG("[ERROR_H] errorCode: ", errorCode);
  PRINT_DEBUG("[SETTINGS_H] minPermOffset: ", minPermOffset);
  PRINT_DEBUG("[SETTINGS_H] maxPermOffset: ", maxPermOffset);
#endif
}

void setup() {
  Serial.begin(9600);
  UI::u8g2.begin();

  sensorSetup();
  btnSetup();
#if LED_BUILD_STATUS
  ledSetup();
#endif
  readFromEEPROM();
}

void loop() {
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
  if (errorCode > 0) haltSystem();
#endif

#ifdef ENABLE_BURNER
  burnerSystem();
#endif
}
