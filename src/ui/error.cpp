#include "ui/error.hpp"

#include "error_helper.hpp"
#include "page.hpp"
#include "ui/helper.hpp"

namespace UI {
void errorPage(U8G2 &d, int error_code) {
  if (error_code <= 0) {
    currentPage = MAIN;
    return;
  }

  char error_message[16] = "Error: ";
  char desc_message[24];

  errorMessageToBuf(error_code, desc_message, sizeof(desc_message));
  itoa(error_code, error_message + 7, 10);

  drawCentered(d, error_message, -9);
  drawCentered(d, desc_message, 15);
}
}  // namespace UI
