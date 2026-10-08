#pragma once
#include "sensor.hpp"
#define CMD_BUF_SIZE 20

void readCommandAndImpl (char *, Sensors *);
size_t getCmdBufSize ();
