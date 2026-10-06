#include "constants/constants.h"
#include "constants/pin.h"
#include "macro/debugUi.h"
#include "page.h"
#include "sensor.h"
#include "ui.h"
#include <Arduino.h>
#include <U8g2lib.h>

extern int currentPage;
OLED_CLASS u8g2 (U8G2_R0);

Sensor s_main;
Sensor s_first;
Sensor s_second;
Sensor s_street;

void sensorSetup ();
void
setup ()
{
  Serial.begin (9600);
  u8g2.begin ();

  sensorSetup ();
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
  u8g2.firstPage ();
  do
    {
      u8g2.setFont (u8g2_font_ncenB08_tr);
      switch (currentPage)
        {
        case MAIN:
          mainPage (&u8g2, &s_main, &s_first, &s_second, &s_street);
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
}