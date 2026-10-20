#include "ui/main_ui.hpp"

#include "contest.hpp"
#include "helper.hpp"
#include "macro/ui.hpp"
#include "page.hpp"
#include "sensor.hpp"
#include "ui/menu.hpp"
#include "validate.hpp"

namespace UI {
void mainPage(U8G2 *display, Sensors *sns) {
  display->setFont(FONT);

  // columns
  RAW_WRITE_ROW(U8G2_MAIN_ROW(0), nullptr, PREVIEW_ACP_TEXT,
                PREVIEW_COLUMN_TEMP, display);
  // rows

  // main sensor
  if (sns->_s_first->temp)
    N_AT_WRITE_ROW(U8G2_MAIN_ROW(1), PREVIEW_TEMP, sns->_s_first->temp,
                   sns->_s_first->acp, display);

  // first reserve sensor
  if (sns->_s_second->temp)
    N_AT_WRITE_ROW(U8G2_MAIN_ROW(2), PREVIEW_FIRST_RESERVE,
                   sns->_s_second->temp, sns->_s_second->acp, display);

  // second reserve sensor
  if (sns->_s_third->temp)
    N_AT_WRITE_ROW(U8G2_MAIN_ROW(3), PREVIEW_SECOND_RESERVE,
                   sns->_s_third->temp, sns->_s_third->acp, display);

  // street sensor
  if (sns->_s_fourth->temp)
    N_AT_WRITE_ROW(U8G2_MAIN_ROW(4), PREVIEW_STREET, sns->_s_fourth->temp,
                   sns->_s_fourth->acp, display);

#ifdef DS18_SENSOR
  char ds18Buf[10];
  dtostrf(Sens::ds18Temp, 0, 2, ds18Buf);
  RAW_WRITE_ROW(U8G2_MAIN_ROW(5), PREVIEW_DS18, nullptr, ds18Buf, display);
#endif
}

void draw() {
  u8g2.firstPage();
  do {
    u8g2.setFont(FONT);
    switch (currentPage) {
      case MAIN:
        mainPage(&u8g2, &Sens::all);
        break;
      case SETTINGS:
        menuPage(&u8g2, &Sens::all);
        break;
#ifdef ENABLE_VALIDATE
      case ERROR:
        errorPage(&u8g2, Val::errorCode);
        break;
#endif
    }
  } while (u8g2.nextPage());
}
}  // namespace UI
