#ifndef COMMAND_H
#define COMMAND_H
#include "sensor.h"
#define CMD_BUF_SIZE 20

void readCommandAndImpl (char *, Sensors *);
size_t getCmdBufSize ();

#endif