#include "OneButtonTiny.h"
#include "constants/constants.h"
#include "constants/pin.h"
#include "constants/settings.h"
#include "macro/debugUi.h"
#include "page.h"
#include "sensor.h"
#include "ui.h"
#include <Arduino.h>
#include <U8g2lib.h>
#ifdef ENABLE_COMMANDS
#include "command.h"
void handleCommand ();

#endif
extern int isValueOpen; //  ui.h
extern int _selected;   // ui.h
extern int currentPage; // page.h
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

void sensorSetup ();
void ledSetup ();

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
  constrSensor (&s_main, MAIN_SENSOR_PIN);
  constrSensor (&s_first, FIRST_RESERVE_SENSOR_PIN);
  constrSensor (&s_second, SECOND_RESERVE_SENSOR_PIN);
  constrSensor (&s_street, STREET_SENSOR_PIN);
#ifdef DEBUG
  PRINT_DEBUG ("s_main acp: ", s_main.acp);
  PRINT_DEBUG ("s_main temp: ", s_main.temp);
  PRINT_DEBUG ("s_first acp: ", s_first.acp);
  PRINT_DEBUG ("s_first temp: ", s_first.temp);
  PRINT_DEBUG ("s_second acp: ", s_second.acp);
  PRINT_DEBUG ("s_second temp: ", s_second.temp);
  PRINT_DEBUG ("s_street acp: ", s_street.acp);
  PRINT_DEBUG ("s_street temp: ", s_street.temp);
#endif
#ifdef ENABLE_COMMANDS
  handleCommand ();
#endif
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
        }
    }
  while (u8g2.nextPage ());
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

  btn_menu.attachClick (btnMenuOnClick);
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
btnMenuOneClick ()
{
  currentPage = currentPage == MAIN ? SETTINGS : MAIN;
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
      Serial.readString ().toCharArray (c_str, getCmdBufSize ());
      readCommandAndImpl (c_str, &sensors);
    }
}

#endif