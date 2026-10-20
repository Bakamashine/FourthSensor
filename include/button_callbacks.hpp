#pragma once
#include <OneButtonTiny.h>

#include "constants/pin.hpp"
#include "constants/settings.hpp"

namespace Btn {
inline OneButtonTiny plus(PLUS_BTN_PIN);
inline OneButtonTiny minus(MINUS_BTN_PIN);
inline OneButtonTiny menu(MENU_BTN_PIN);
inline OneButtonTiny burner_btn(BURNER_BTN_PIN);
}  // namespace Btn

void btnSetup();
void btnTick();
void btnTickBurner();