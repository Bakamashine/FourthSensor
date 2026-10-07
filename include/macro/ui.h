#pragma once
#include "constants/ui.h"
#define _CENTER_X(display, text)                                              \
  ((OLED_WIDTH - display->getStrWidth (text)) / 2)
#define _CENTER_Y(display) ((OLED_HEIGHT + display->getFontAscent ()) / 2 - 25)

#define RAW_WRITE_ROW(row, text, v1, v2, p_cls)                               \
  do                                                                          \
    {                                                                         \
      if (text)                                                               \
        (p_cls)->drawStr (U8G2_FIRST_COLUMN, row, text);                      \
      if (v1)                                                                 \
        (p_cls)->drawStr (U8G2_SECOND_COLUMN, row, v1);                       \
      if (v2)                                                                 \
        (p_cls)->drawStr (U8G2_THIRD_COLUMN, row, v2);                        \
    }                                                                         \
  while (0)

// display temperature as number
// temp is a float, so it goes through setFloatText rather than snprintf("%f"):
// printf's float support is absent on AVR, and pulling in float printf would
// cost more flash than the whole rest of the program.
#define N_AT_WRITE_ROW(row, text, temp, acp, p_cls)                           \
  do                                                                          \
    {                                                                         \
      char acpBuf[8];                                                         \
      char tempBuf[10];                                                       \
      setFloatText (tempBuf, sizeof (tempBuf), "", temp);                     \
      snprintf (acpBuf, sizeof (acpBuf), "%d", acp);                          \
      RAW_WRITE_ROW (row, text, acpBuf, tempBuf, p_cls);                      \
    }                                                                         \
  while (0)
