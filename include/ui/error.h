#ifndef ERROR_H
#define ERROR_H
#include "sensor.h"
#include <Arduino.h>
#include <U8g2lib.h>

void errorPage (U8G2 *display, int error_code);

#endif