#include "ui/helper.h"
#include "macro/ui.h"
int
drawCentered (U8G2 *display, const char *text, int padding_top,
              int padding_bottom, int padding_left, int padding_right)
{
  int x = (OLED_WIDTH - display->getStrWidth (text)) / 2;
  int y = (OLED_HEIGHT + display->getFontAscent ()) / 2;
  display->drawStr (x + padding_left - padding_right,
                    y + padding_top - padding_bottom, text);
  return y;
}