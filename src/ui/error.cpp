#include "ui/error.h"
#include "ui/helper.h"
void
errorPage (U8G2 *display, int error_code)
{

  size_t error_message_size = 16;
  char error_message[error_message_size];
  snprintf (error_message, error_message_size, "Error: %d", error_code);
  drawCentered (display, error_message, 0, 0, 0, 0);
}