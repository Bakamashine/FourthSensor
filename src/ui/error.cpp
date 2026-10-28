#include "constants/ui.hpp"
#include "error_helper.hpp"
#include "page.hpp"
#include "ui/ui_class.hpp"

void UI_cls::errorPage(int error_code) {
  if (error_code <= 0) {
    currentPage = MAIN;
    return;
  }

  char error_message[16] = "Error: ";
  char desc_message[24];

  errorMessageToBuf(error_code, desc_message, sizeof(desc_message));
  itoa(error_code, error_message + 7, 10);

  this->drawStr(U8G2_FIRST_COLUMN, U8G2_SECOND_ROW, error_message);
  this->drawStr(U8G2_FIRST_COLUMN, U8G2_FOURTH_ROW, desc_message);
}
