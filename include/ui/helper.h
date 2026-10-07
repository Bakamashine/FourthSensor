#pragma once
#include "sensor.h"
#include <Arduino.h>
#include <U8g2lib.h>
int drawCentered (U8G2 *display, const char *text, int padding_top,
                  int padding_bottom, int padding_left, int padding_right);
