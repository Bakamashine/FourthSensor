#pragma once
#include "sensor.hpp"

namespace Validate {
inline int errorCode = 0;

void pipeline(Sensors *);
void haltSystem();

inline bool systemHalted = false;
}  // namespace Validate
