#pragma once
#include <OneButtonTiny.h>

#include "constants/pin.hpp"

namespace Btn {
inline OneButtonTiny *plus = nullptr;
inline OneButtonTiny *minus = nullptr;
inline OneButtonTiny *menu = nullptr;
}  // namespace Btn

void btnSetup();
void btnPlusOneClick();
void btnPlusLongPress();
void btnMinusOneClick();
void btnMinusLongPress();
void btnMenuOnClick();
void btnMenuLongPress();
void btnTick();
void deleteButtons();