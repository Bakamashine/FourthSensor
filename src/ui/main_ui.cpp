#include "ui/main_ui.hpp"

#include "contest.hpp"
#include "helper.hpp"
#include "macro/ui.hpp"
#include "page.hpp"
#include "sensor.hpp"
#include "ui/menu.hpp"

namespace UI {
void mainPage(U8G2 *display, Sensors *sensors) {
  display->setFont(FONT);

  // columns
  RAW_WRITE_ROW(U8G2_MAIN_ROW(0), nullptr, PREVIEW_ACP_TEXT,
                PREVIEW_COLUMN_TEMP, display);
  // rows

  // main sensor
  if (sensors->_s_first->temp)
    N_AT_WRITE_ROW(U8G2_MAIN_ROW(1), PREVIEW_TEMP, sensors->_s_first->temp,
                   sensors->_s_first->acp, display);

  // first reserve sensor
  if (sensors->_s_second->temp)
    N_AT_WRITE_ROW(U8G2_MAIN_ROW(2), PREVIEW_FIRST_RESERVE,
                   sensors->_s_second->temp, sensors->_s_second->acp, display);

  // second reserve sensor
  if (sensors->_s_third->temp)
    N_AT_WRITE_ROW(U8G2_MAIN_ROW(3), PREVIEW_SECOND_RESERVE,
                   sensors->_s_third->temp, sensors->_s_third->acp, display);

  // street sensor
  if (sensors->_s_fourth->temp)
    N_AT_WRITE_ROW(U8G2_MAIN_ROW(4), PREVIEW_STREET, sensors->_s_fourth->temp,
                   sensors->_s_fourth->acp, display);

#ifdef DS18_SENSOR
  // DS18 sensor: temperature only, so it goes into the temperature column
  char ds18Buf[10];
  setFloatText(ds18Buf, sizeof(ds18Buf), Sens::ds18Temp);
  RAW_WRITE_ROW(U8G2_MAIN_ROW(5), PREVIEW_DS18, nullptr, ds18Buf, display);
#endif
}

void draw() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB08_tr);
    switch (currentPage) {
      case MAIN:
        mainPage(&u8g2, &Sens::all);
        break;
      case SETTINGS:
        menuPage(&u8g2, &Sens::all);
        break;

#ifdef ENABLE_VALIDATE
      case ERROR:
        errorPage(&u8g2, errorCode);
        break;
#endif
    }
  } while (u8g2.nextPage());
}
}  // namespace UI
