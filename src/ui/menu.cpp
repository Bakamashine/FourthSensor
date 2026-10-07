#include "ui/menu.h"

#include "contest.h"
#include "helper.h"
#include "macro/ui.h"
#include "sensor.h"
#include "settings.h"

bool isValueOpen = false;
int _selected = 0;

#define MENU_ROWS_COUNT 4
static Records _rows[MENU_ROWS_COUNT];
void
drawPoint (U8G2 *display)
{
  for (int i = 0; i < MENU_ROWS_COUNT; i++)
    {
      display->drawStr (U8G2_MENU_MARKER_X, _rows[i].row,
                        i == _selected ? ">" : " ");
    }
}

void
drawPreviews (U8G2 *display)
{
  for (int i = 0; i < MENU_ROWS_COUNT; i++)
    {
      if (_rows[i].preview)
        display->drawStr (_rows[i].col, _rows[i].row, _rows[i].preview);
    }
}
void
buildRows (Sensors *sensors)
{
  // _rows = static_cast<Records *>(malloc (sizeof (Records) * 4));

  _rows[0].sn = sensors->_s_main;
  _rows[0].row = U8G2_SECOND_ROW;
  _rows[0].col = U8G2_FIRST_COLUMN;
  _rows[0].preview = PREVIEW_T1;

  _rows[1].sn = sensors->_s_first;
  _rows[1].row = U8G2_THIRD_ROW;
  _rows[1].col = U8G2_FIRST_COLUMN;
  _rows[1].preview = PREVIEW_T2;

  _rows[2].sn = sensors->_s_second;
  _rows[2].row = U8G2_FOURTH_ROW;
  _rows[2].col = U8G2_FIRST_COLUMN;
  _rows[2].preview = PREVIEW_T3;

  _rows[3].sn = sensors->_s_street;
  _rows[3].row = U8G2_FIFTH_ROW;
  _rows[3].col = U8G2_FIRST_COLUMN;
  _rows[3].preview = PREVIEW_T4;
}

void
menuPage (U8G2 *display, Sensors *sensors)
{
  display->drawStr (_CENTER_X (display, PREVIEW_MENU), _CENTER_Y (display),
                    PREVIEW_MENU);
  buildRows (sensors);
  drawPreviews (display);
  drawPoint (display);

  const int size = 8;
  char ci_main_sensor_str[size];
  char ci_first_sensor_str[size];
  char ci_second_sensor_str[size];
  char ci_street_sensor_str[size];
  setIntText (ci_main_sensor_str, size, sensors->_s_main->correctInt);
  setIntText (ci_first_sensor_str, size, sensors->_s_first->correctInt);
  setIntText (ci_second_sensor_str, size, sensors->_s_second->correctInt);
  setIntText (ci_street_sensor_str, size, sensors->_s_street->correctInt);
  RAW_WRITE_ROW (U8G2_SECOND_ROW, PREVIEW_T1, ci_main_sensor_str, nullptr,
                 display);
  RAW_WRITE_ROW (U8G2_THIRD_ROW, PREVIEW_T2, ci_first_sensor_str, nullptr,
                 display);
  RAW_WRITE_ROW (U8G2_FOURTH_ROW, PREVIEW_T3, ci_second_sensor_str, nullptr,
                 display);

  const int message_size = 16;
  char __hyst[message_size];
  char max_permitted_offset[message_size];
  char min_permitted_offset[message_size];
  snprintf (max_permitted_offset, message_size, "%s %d", PREVIEW_MAX,
            maxPermOffset);
  snprintf (min_permitted_offset, message_size, "%s %d", PREVIEW_MIN,
            minPermOffset);
  snprintf (__hyst, message_size, "%s %d", PREVIEW_HYST, hyst);
  RAW_WRITE_ROW (U8G2_SECOND_ROW, PREVIEW_T1, nullptr, __hyst, display);
  RAW_WRITE_ROW (U8G2_THIRD_ROW, PREVIEW_T2, nullptr, max_permitted_offset,
                 display);
  RAW_WRITE_ROW (U8G2_FOURTH_ROW, PREVIEW_T3, nullptr, min_permitted_offset,
                 display);

  if (isValueOpen)
    {
      RAW_WRITE_ROW (U8G2_FIFTH_ROW, PREVIEW_T4, ci_street_sensor_str, "ON",
                     display);
    }
  else
    {
      char userTemperature_str[message_size];
      snprintf (userTemperature_str, message_size, "%s %d", PREVIEW_USER_TEMP,
                userTemperature);
      RAW_WRITE_ROW (U8G2_FIFTH_ROW, PREVIEW_T4, ci_street_sensor_str,
                     userTemperature_str, display);
    }
}

void
menuPageIncreaseValue ()
{
  _rows[_selected].sn->correctInt++;
}

void
menuPageDecreaseValue ()
{
  _rows[_selected].sn->correctInt--;
}