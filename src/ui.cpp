#include "ui.h"
#include "contest.h"
#include "helper.h"
#include "macro/ui.h"
#include "sensor.h"

#define MENU_ROWS_COUNT 4
static Records _rows[MENU_ROWS_COUNT];

void
buildRows (Sensors *sensors)
{
  // _rows = (Records*)malloc(sizeof(Records)*4);

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

void
menuPage (U8G2 *display, Sensors *sensors)
{
  display->drawStr (_CENTER_X (display, PREVIEW_MENU), _CENTER_Y (display),
                    PREVIEW_MENU);
  buildRows (sensors);
  drawPreviews (display);
  drawPoint (display);
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