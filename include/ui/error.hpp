#pragma once
#include <Arduino.h>
#include <U8g2lib.h>

#include "sensor.hpp"
namespace UI {
void errorPage(U8G2 &display, int error_code);
}
