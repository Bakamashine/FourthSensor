#include "ui/menu.h"

#include "contest.h"
#include "helper.h"
#include "macro/ui.h"
#include "sensor.h"

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