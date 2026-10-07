#pragma once
#include "sensor.h"
#include <Arduino.h>
#include <U8g2lib.h>


void mainPage (U8G2 *display, Sensors *sensors);

void errorPage (U8G2 *display, int error_code);
