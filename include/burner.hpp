#pragma once
#include "sensor.hpp"

namespace Burner {

void burnerSystem();
bool checkTemp(Sensors *sn);
inline bool status = false;
}  // namespace Burner
