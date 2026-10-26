#pragma once
#include <Arduino.h>
#include <U8g2lib.h>

#include "sensor.hpp"

void menuPage(U8G2 &display, Sensors &sensors);
void menuPageIncreaseValue();
void menuPageDecreaseValue();
void menuSelectNext();
void menuSelectPrev();
void exitMenu();
