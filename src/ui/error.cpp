#include "ui/error.hpp"

#include "page.hpp"
#include "ui/helper.hpp"

namespace UI {
void errorPage(U8G2 *display, int error_code) {
  if (error_code <= 0) currentPage = MAIN;
  char num[8];
  char error_message[16];
  itoa(error_code, num, 10);
  strcpy(error_message, "Error: ");
  strcat(error_message, num);
  drawCentered(display, error_message, 0, 0, 0, 0);
}
}  // namespace UI