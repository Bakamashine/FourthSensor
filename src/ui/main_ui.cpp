#include "ui/main_ui.h"
#include "contest.h"
#include "helper.h"
#include "macro/ui.h"
#include "sensor.h"


void
mainPage (U8G2 *display, Sensors *sensors)
{
  // columns
  RAW_WRITE_ROW (U8G2_FIRST_ROW, nullptr, PREVIEW_ACP_TEXT,
                 PREVIEW_COLUMN_TEMP, display);
  // rows

  // main sensor
  if (sensors->_s_main->temp)
    N_AT_WRITE_ROW (U8G2_SECOND_ROW, PREVIEW_TEMP, sensors->_s_main->temp,
                    sensors->_s_main->acp, display);

  // first reserve sensor
  if (sensors->_s_first->temp)
    N_AT_WRITE_ROW (U8G2_THIRD_ROW, PREVIEW_FIRST_RESERVE,
                    sensors->_s_first->temp, sensors->_s_first->acp, display);

  // second reserve sensor
  if (sensors->_s_second->temp)
    N_AT_WRITE_ROW (U8G2_FOURTH_ROW, PREVIEW_SECOND_RESERVE,
                    sensors->_s_second->temp, sensors->_s_second->acp,
                    display);

  // street sensor
  if (sensors->_s_street->temp)
    N_AT_WRITE_ROW (U8G2_FIFTH_ROW, PREVIEW_STREET, sensors->_s_street->temp,
                    sensors->_s_street->acp, display);
}



