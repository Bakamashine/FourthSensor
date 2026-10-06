#include "ui.h"
#include "contest.h"
#include "helper.h"
#include "macro/ui.h"
#include "sensor.h"

void
mainPage (U8G2 *display, Sensor *s_main, Sensor *s_first, Sensor *s_second,
          Sensor *s_street)
{
  // columns
  RAW_WRITE_ROW (U8G2_FIRST_ROW, nullptr, PREVIEW_ACP_TEXT,
                 PREVIEW_COLUMN_TEMP, display);
  // rows

  // main sensor
  if (s_main->temp)
    N_AT_WRITE_ROW (U8G2_SECOND_ROW, PREVIEW_TEMP, s_main->temp, s_main->acp,
                    display);

  // first reserve sensor
  if (s_first->temp)
    N_AT_WRITE_ROW (U8G2_THIRD_ROW, PREVIEW_FIRST_RESERVE, s_first->temp,
                    s_first->acp, display);

  // second reserve sensor
  if (s_second->temp)
    N_AT_WRITE_ROW (U8G2_FOURTH_ROW, PREVIEW_SECOND_RESERVE, s_second->temp,
                    s_second->acp, display);

  // street sensor
  if (s_street->temp)
    N_AT_WRITE_ROW (U8G2_FIFTH_ROW, PREVIEW_STREET, s_street->temp,
                    s_street->acp, display);
}