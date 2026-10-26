#pragma once
#include "sensor.hpp"

namespace Val {
inline int errorCode = 0;

void pipeline(Sensors &);
int pipelineWithoutHalt(Sensors &);
void checkError();
void haltSystem();
int checkTemp(Sensors &);

inline bool systemHalted = false;
}  // namespace Val
