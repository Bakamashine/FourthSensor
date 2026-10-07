#pragma once
#include "sensor.h"
#include <Arduino.h>
#include <U8g2lib.h>
struct Records
{
  Sensor *sn;
  int row;
  int col;
  const char *preview;
};
void menuPage (U8G2 *display, Sensors *sensors);
void menuPageIncreaseValue ();
void menuPageDecreaseValue ();
extern bool isValueOpen;
extern int _selected;
