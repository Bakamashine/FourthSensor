#pragma once
#include "sensor.hpp"

namespace Val {
inline int errorCode = 0;

void pipeline(Sensors *);
void checkError();
void haltSystem();

inline bool systemHalted = false;
}  // namespace Val
