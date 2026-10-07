#include "OneButtonTiny.h"
#include "constants/constants.h"
#include "constants/pin.h"
#include "constants/settings.h"
#include "macro/debugUi.h"
#include "page.h"
#include "sensor.h"
#include "settings.h"
#include "ui/main_ui.h"
#include "ui/menu.h"
#include <Arduino.h>
#include <U8g2lib.h>
#ifdef ENABLE_COMMANDS
#include "command.h"
void handleCommand ();
#endif
#ifdef ENABLE_VALIDATE
#include "validate.h"
void haltSystem ();
int systemHalted = 0;
#endif

// page.h
int currentPage = SETTINGS;

OLED_CLASS u8g2 (U8G2_R0);

Sensor s_main;
Sensor s_first;
Sensor s_second;
Sensor s_street;
Sensors sensors;

OneButtonTiny btn_plus (PLUS_BTN_PIN);
OneButtonTiny btn_minus (MINUS_BTN_PIN);
OneButtonTiny btn_menu (MENU_BTN_PIN);

void btnSetup ();
void btnPlusOneClick ();
void btnPlusLongPress ();
void btnMinusOneClick ();
void btnMinusLongPress ();
void btnMenuOnClick ();
void btnMenuLongPress ();
void btnTick ();

void ui ();
void sensorSetup ();
void ledSetup ();
#ifdef LED_DEBUG
void
ledDebug ()
{
  digitalWrite (BURNER_PIN, HIGH);
  digitalWrite (ERROR_PIN, LOW);
  delay (200);
  digitalWrite (BURNER_PIN, LOW);
  digitalWrite (ERROR_PIN, HIGH);
}
#endif

void
setup ()
{
  Serial.begin (9600);
  u8g2.begin ();

  sensorSetup ();
  btnSetup ();
  ledSetup ();
}

void
loop ()
{
  btnTick ();
#ifdef LED_DEBUG
  ledDebug ();
#endif
  constrSensor (&s_main, MAIN_SENSOR_PIN);
  constrSensor (&s_first, FIRST_RESERVE_SENSOR_PIN);
  constrSensor (&s_second, SECOND_RESERVE_SENSOR_PIN);
  constrSensor (&s_street, STREET_SENSOR_PIN);
#ifdef DEBUG
  Serial.println ("=====DEBUG INPUT====");
  // acp
  PRINT_DEBUG ("[ACP] s_main: ", s_main.acp);
  PRINT_DEBUG ("[ACP] s_first: ", s_first.acp);
  PRINT_DEBUG ("[ACP] s_second: ", s_second.acp);
  PRINT_DEBUG ("[ACP] s_street: ", s_street.acp);

  // temperature
  PRINT_DEBUG ("[TEMP] s_main: ", s_main.temp);
  PRINT_DEBUG ("[TEMP] s_first: ", s_first.temp);
  PRINT_DEBUG ("[TEMP] s_second: ", s_second.temp);
  PRINT_DEBUG ("[TEMP] s_street: ", s_street.temp);

  // correct integer
  PRINT_DEBUG ("[CORRECT INTEGER] s_main: ", s_main.correctInt);
  PRINT_DEBUG ("[CORRECT INTEGER] s_first: ", s_first.correctInt);
  PRINT_DEBUG ("[CORRECT INTEGER] s_second: ", s_second.correctInt);
  PRINT_DEBUG ("[CORRECT INTEGER] s_street: ", s_street.correctInt);

  // other variables
  PRINT_DEBUG ("[SETTINGS_H] burnerStatus: ", burnerStatus);
  PRINT_DEBUG ("[SETTINGS_H] minPermOffset: ", minPermOffset);
  PRINT_DEBUG ("[SETTINGS_H] maxPermOffset: ", maxPermOffset);
  PRINT_DEBUG ("[SETTINGS_H] Hysteresis: ", hyst);
  PRINT_DEBUG ("[MENU_UI_H] isValueOpen: ", isValueOpen);
  PRINT_DEBUG ("[MENU_UI_H] _selected: ", _selected);
  PRINT_DEBUG ("[PAGE_H] currentPage: ", currentPage);
  PRINT_DEBUG ("[ERROR_H] errorCode: ", errorCode);

#endif
#ifdef ENABLE_COMMANDS
  handleCommand ();
#endif
  ui ();

#ifdef ENABLE_VALIDATE
  // validate
  validatePipeline (&sensors);
  if (errorCode > 0)
    haltSystem ();
#endif
}

void
sensorSetup ()
{
  pinMode (MAIN_SENSOR_PIN, INPUT);
  pinMode (FIRST_RESERVE_SENSOR_PIN, INPUT);
  pinMode (SECOND_RESERVE_SENSOR_PIN, INPUT);
  pinMode (STREET_SENSOR_PIN, INPUT);
  s_main._adcFilter = -1.0F;
  s_first._adcFilter = -1.0F;
  s_second._adcFilter = -1.0F;
  s_street._adcFilter = -1.0F;
  sensors._s_main = &s_main;
  sensors._s_first = &s_first;
  sensors._s_second = &s_second;
  sensors._s_street = &s_street;
}

void
btnSetup ()
{
  const int debounce = 20;
  auto mode = INPUT_PULLUP;
  pinMode (MENU_BTN_PIN, mode);
  pinMode (PLUS_BTN_PIN, mode);
  pinMode (MINUS_BTN_PIN, mode);

  btn_menu.setDebounceMs (debounce);
  btn_plus.setDebounceMs (debounce);
  btn_minus.setDebounceMs (debounce);

  // btn_menu.attachClick (btnMenuOnClick);
  btn_menu.attachLongPressStart (btnMenuLongPress);
  btn_plus.attachClick (btnPlusOneClick);
  btn_plus.attachLongPressStart (btnPlusLongPress);
  btn_minus.attachClick (btnMinusOneClick);
  btn_minus.attachLongPressStart (btnMinusLongPress);
}

void
ledSetup ()
{
  // burner
  pinMode (BURNER_PIN, OUTPUT);

  // error
  pinMode (ERROR_PIN, OUTPUT);
}

void
btnMenuOnClick ()
{
  currentPage = currentPage == MAIN ? SETTINGS : MAIN;
}

void
btnMenuLongPress ()
{
  btnMenuOnClick ();
}
void
btnTick ()
{
  btn_menu.tick ();
  btn_plus.tick ();
  btn_minus.tick ();
}
void
btnPlusLongPress ()
{
  if (currentPage == SETTINGS)
    isValueOpen = 1;
}

void
btnPlusOneClick ()
{
  if (currentPage == SETTINGS && !isValueOpen)
    {
      // menuUI.goToUp ();
      _selected++;
#ifdef DEBUG
      Serial.println ("goToUp");
#endif
    }
  else if (currentPage && isValueOpen)
    {
      menuPageIncreaseValue ();
    }
}

void
btnMinusOneClick ()
{
  if (currentPage == SETTINGS && !isValueOpen)
    {
      _selected--;
#ifdef DEBUG
      Serial.println ("goToDown");
#endif
    }
  else if (currentPage == SETTINGS && isValueOpen)
    {
      menuPageDecreaseValue ();
    }
}

void
btnMinusLongPress ()
{
  if (currentPage == SETTINGS)
    isValueOpen = 0;
}

#ifdef ENABLE_COMMANDS

void
handleCommand ()
{
  if (Serial.available () > 0)
    {
      char *c_str = (char *)malloc (getCmdBufSize ());
      if (c_str == nullptr)
        return;
      Serial.readString ().toCharArray (c_str, getCmdBufSize ());
      readCommandAndImpl (c_str, &sensors);
    }
}

#endif

void
ui ()
{
  u8g2.firstPage ();
  do
    {
      u8g2.setFont (u8g2_font_ncenB08_tr);
      switch (currentPage)
        {
        case MAIN:
          mainPage (&u8g2, &sensors);
          break;
        case SETTINGS:
          menuPage (&u8g2, &sensors);
          break;
        case ERROR:
          errorPage (&u8g2, errorCode);
          break;
        }
    }
  while (u8g2.nextPage ());
}

#ifdef ENABLE_VALIDATE
void
haltSystem ()
{
  if (!systemHalted)
    {
      Serial.println ("ERROR");
      digitalWrite (BURNER_PIN, LOW);
      digitalWrite (ERROR_PIN, HIGH);
      burnerStatus = 0;
    }
  systemHalted = 1;
  currentPage = ERROR;
}
#endif