#pragma once
#include <OneButtonTiny.h>

#include "constants/pin.hpp"

namespace Btn {
extern OneButtonTiny plus;
extern OneButtonTiny minus;
extern OneButtonTiny menu;
}  // namespace Btn

void btnSetup();
void btnPlusOneClick();
void btnPlusLongPress();
void btnMinusOneClick();
void btnMinusLongPress();
void btnMenuOnClick();
void btnMenuLongPress();
void btnTick();