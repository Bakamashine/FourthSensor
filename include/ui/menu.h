#pragma once
#include "sensor.h"
#include <Arduino.h>
#include <U8g2lib.h>

void menuPage (U8G2 *display, Sensors *sensors);
void menuPageIncreaseValue ();
void menuPageDecreaseValue ();
void menuSelectNext ();
void menuSelectPrev ();
extern bool isValueOpen;
extern int _selected;
