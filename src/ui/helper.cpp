#include "ui/helper.hpp"

#include "macro/ui.hpp"
int drawCentered(U8G2 &d, const char *text, int padding_top, int padding_bottom,
                 int padding_left, int padding_right) {
  int x = (OLED_WIDTH - static_cast<int>(d.getStrWidth(text))) >> 1;
  if (x < 0) x = 0;  // never let a too-wide string wrap around off-screen
  int y = (OLED_HEIGHT + d.getFontAscent()) >> 1;
  d.drawStr(x + padding_left - padding_right, y + padding_top - padding_bottom,
            text);
  return y;
}