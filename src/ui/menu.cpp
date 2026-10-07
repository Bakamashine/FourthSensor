#include "ui/menu.h"

#include "contest.h"
#include "helper.h"
#include "macro/ui.h"
#include "sensor.h"
#include "settings.h"

bool isValueOpen = false;
int _selected = 0;

/// bounds for the settings that are not sensor corrections
#define HYST_MIN 0
#define HYST_MAX 50
#define OFFSET_MIN 0
#define OFFSET_MAX 200
#define UTEMP_MIN 0
#define UTEMP_MAX 100

/// left half: the four sensor corrections, right half: the settings
#define MENU_LEFT_ITEMS 4
#define MENU_ITEMS_COUNT (MENU_LEFT_ITEMS + 4)

struct MenuItem
{
  int row;      ///< y of the row the value is drawn on
  int markerX;  ///< x of the selection marker for this cell
  int valueX;   ///< x the value (and, on the right half, the label) is drawn
  const char *preview;
  int *value; ///< address of the edited variable
  int min;    ///< smallest value the variable may take
  int max;    ///< largest value the variable may take
};

static MenuItem _items[MENU_ITEMS_COUNT];

static void
buildItems (Sensors *sensors)
{
  // left half, sensor corrections
  _items[0]
      = { U8G2_SECOND_ROW, U8G2_MENU_MARKER_X, U8G2_SECOND_COLUMN, PREVIEW_T1,
          &sensors->_s_main->correctInt, MIN_CORRECT_INT, MAX_CORRECT_INT };
  _items[1]
      = { U8G2_THIRD_ROW, U8G2_MENU_MARKER_X, U8G2_SECOND_COLUMN, PREVIEW_T2,
          &sensors->_s_first->correctInt, MIN_CORRECT_INT, MAX_CORRECT_INT };
  _items[2]
      = { U8G2_FOURTH_ROW, U8G2_MENU_MARKER_X, U8G2_SECOND_COLUMN, PREVIEW_T3,
          &sensors->_s_second->correctInt, MIN_CORRECT_INT, MAX_CORRECT_INT };
  _items[3]
      = { U8G2_FIFTH_ROW, U8G2_MENU_MARKER_X, U8G2_SECOND_COLUMN, PREVIEW_T4,
          &sensors->_s_street->correctInt, MIN_CORRECT_INT, MAX_CORRECT_INT };

  // right half, settings; min/max of the permitted offset are kept ordered
  _items[4] = { U8G2_SECOND_ROW, U8G2_MENU_MARKER2_X, U8G2_THIRD_COLUMN,
                PREVIEW_HYST, &hyst, HYST_MIN, HYST_MAX };
  _items[5] = { U8G2_THIRD_ROW, U8G2_MENU_MARKER2_X, U8G2_THIRD_COLUMN,
                PREVIEW_MAX, &maxPermOffset, minPermOffset, OFFSET_MAX };
  _items[6] = { U8G2_FOURTH_ROW, U8G2_MENU_MARKER2_X, U8G2_THIRD_COLUMN,
                PREVIEW_MIN, &minPermOffset, OFFSET_MIN, maxPermOffset };
  _items[7] = { U8G2_FIFTH_ROW, U8G2_MENU_MARKER2_X, U8G2_THIRD_COLUMN,
                PREVIEW_USER_TEMP, &uTemp, UTEMP_MIN, UTEMP_MAX };
}

static void
drawItems (U8G2 *display)
{
  char valueStr[8];
  char line[20];

  for (int i = 0; i < MENU_ITEMS_COUNT; i++)
    {
      const MenuItem &item = _items[i];
      if (i < MENU_LEFT_ITEMS)
        {
          setIntText (valueStr, sizeof (valueStr), *item.value);
          display->drawStr (U8G2_FIRST_COLUMN, item.row, item.preview);
          display->drawStr (item.valueX, item.row, valueStr);
        }
      else
        {
          // the right half is narrower, label and value share one string
          snprintf (line, sizeof (line), "%s %d", item.preview, *item.value);
          display->drawStr (item.valueX, item.row, line);
        }
    }
}

static void
drawMarker (U8G2 *display)
{
  if (_selected < 0 || _selected >= MENU_ITEMS_COUNT)
    return;
  const MenuItem &item = _items[_selected];
  display->drawStr (item.markerX, item.row, ">");
}

void
menuPage (U8G2 *display, Sensors *sensors)
{
  display->drawStr (_CENTER_X (display, PREVIEW_MENU), _CENTER_Y (display),
                    PREVIEW_MENU);
  buildItems (sensors);
  drawItems (display);
  drawMarker (display);

  if (isValueOpen)
    display->drawStr (U8G2_EDIT_X, U8G2_EDIT_Y, "ON");
}

static void
changeSelected (int delta)
{
  if (_selected < 0 || _selected >= MENU_ITEMS_COUNT)
    return;

  int *value = _items[_selected].value;
  if (value == nullptr)
    return;

  int next = *value + delta;
  if (next < _items[_selected].min)
    next = _items[_selected].min;
  if (next > _items[_selected].max)
    next = _items[_selected].max;
  *value = next;
}

void
menuPageIncreaseValue ()
{
  changeSelected (1);
}

void
menuPageDecreaseValue ()
{
  changeSelected (-1);
}

void
menuSelectNext ()
{
  _selected = (_selected + 1) % MENU_ITEMS_COUNT;
}

void
menuSelectPrev ()
{
  _selected = (_selected - 1 + MENU_ITEMS_COUNT) % MENU_ITEMS_COUNT;
}
