#pragma once
#include "sensor.h"
#define CMD_BUF_SIZE 20

void readCommandAndImpl (char *, Sensors *);
size_t getCmdBufSize ();
