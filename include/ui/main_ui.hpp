#pragma once
#include <Arduino.h>
#include <U8g2lib.h>

#include "constants/constants.hpp"
#include "sensor.hpp"

namespace UI {
void mainPage(U8G2 &display, Sensors &sns);
void startPage(U8G2 &);
void errorPage(U8G2 &display, int error_code);
void draw();
inline OLED_CLASS u8g2(U8G2_R0);
void closeStartPage();
}  // namespace UI
