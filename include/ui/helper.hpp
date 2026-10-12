#pragma once
#include <Arduino.h>
#include <U8g2lib.h>

#include "sensor.hpp"
int drawCentered(U8G2 *display, const char *text, int padding_top,
                 int padding_bottom, int padding_left, int padding_right);
