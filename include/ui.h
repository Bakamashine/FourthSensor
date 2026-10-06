#ifndef UI_H
#define UI_H
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

void mainPage (U8G2 *display, Sensors *sensors);
void menuPage (U8G2 *display, Sensors *sensors);
void menuPageIncreaseValue ();
void menuPageDecreaseValue ();
void errorPage (U8G2 *display, int error_code);

static int isValueOpen;
static int _selected = 0;
#endif