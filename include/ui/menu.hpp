#pragma once
#include "constants/settings.hpp"
#include "sensor.hpp"

class UI_cls;

void menuPage(UI_cls &, Sensors &);
void menuPageIncreaseValue();
void menuPageDecreaseValue();
void menuSelectNext();
void menuSelectPrev();
void exitMenu();
