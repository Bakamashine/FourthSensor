#ifndef MENU_H
#define MENU_H
#include "sensor.h"
#include <Arduino.h>
#include <U8g2lib.h>
typedef struct Records
{
  Sensor *sn;
  int row;
  int col;
  const char *preview;
} Records;
void menuPage (U8G2 *display, Sensors *sensors);
void menuPageIncreaseValue ();
void menuPageDecreaseValue ();
static bool isValueOpen = false;
static int _selected = 0;
#endif