#pragma once
#include <Arduino.h>
#include <U8g2lib.h>

#include "sensor.hpp"
int drawCentered(U8G2 &display, const char *text, int padding_top = 0,
                 int padding_bottom = 0, int padding_left = 0,
                 int padding_right = 0);
