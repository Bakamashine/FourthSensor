#include "ui/error.hpp"
#include "page.hpp"
#include "ui/helper.hpp"
void
errorPage (U8G2 *display, int error_code)
{
  if (error_code <= 0)
    currentPage = MAIN;
  size_t error_message_size = 16;
  char error_message[error_message_size];
  snprintf (error_message, error_message_size, "Error: %d", error_code);
  drawCentered (display, error_message, 0, 0, 0, 0);
}