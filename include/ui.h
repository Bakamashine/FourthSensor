#ifndef UI_H
#define UI_H
#include <Arduino.h>
#include <U8g2lib.h>
#include "sensor.h"

void mainPage (U8G2 *display, Sensor *s_main, Sensor *s_first,
               Sensor *s_second, Sensor *s_street);

#endif