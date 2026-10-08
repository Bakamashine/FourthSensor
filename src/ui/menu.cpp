#include "ui/menu.hpp"

#include "contest.hpp"
#include "helper.hpp"
#include "macro/ui.hpp"
#include "sensor.hpp"
#include "settings.hpp"

bool isValueOpen = false;
int _selected = 0;

/// bounds for the settings that are not sensor corrections
#define HYST_MIN 0
#define HYST_MAX 50
#define HYST_STEP 0.01F
#define OFFSET_MIN 0
#define OFFSET_MAX 200
#define UTEMP_MIN 0
#define UTEMP_MAX 100

/// left half: the four sensor corrections, right half: the settings
#define MENU_LEFT_ITEMS 4
#define MENU_ITEMS_COUNT (MENU_LEFT_ITEMS + 4)

/// avr-gcc only accepts a designated initializer when every member is named
/// in declaration order, so keep this list complete
struct MenuItem
{
  int row;     ///< y of the row the value is drawn on
  int markerX; ///< x of the selection marker for this cell
  int valueX;  ///< x the value (and, on the right half, the label) is drawn
  const char *preview;
  int *value;    ///< address of the edited variable, 0 if fvalue is used
  float *fvalue; ///< address of the edited variable, 0 if value is used
  float step;    ///< change per press, only used for fvalue
  int min;       ///< smallest value the variable may take
  int max;       ///< largest value the variable may take
};

static MenuItem _items[MENU_ITEMS_COUNT];

static void
buildItems (Sensors *sensors)
{
  // left half, sensor corrections
  _items[0] = { .row = U8G2_SECOND_ROW,
                .markerX = U8G2_MENU_MARKER_X,
                .valueX = U8G2_SECOND_COLUMN,
                .preview = PREVIEW_T1,
                .value = &sensors->_s_main->correctInt,
                .fvalue = nullptr,
                .step = 0,
                .min = MIN_CORRECT_INT,
                .max = MAX_CORRECT_INT };
  _items[1] = { .row = U8G2_THIRD_ROW,
                .markerX = U8G2_MENU_MARKER_X,
                .valueX = U8G2_SECOND_COLUMN,
                .preview = PREVIEW_T2,
                .value = &sensors->_s_first->correctInt,
                .fvalue = nullptr,
                .step = 0,
                .min = MIN_CORRECT_INT,
                .max = MAX_CORRECT_INT };
  _items[2] = { .row = U8G2_FOURTH_ROW,
                .markerX = U8G2_MENU_MARKER_X,
                .valueX = U8G2_SECOND_COLUMN,
                .preview = PREVIEW_T3,
                .value = &sensors->_s_second->correctInt,
                .fvalue = nullptr,
                .step = 0,
                .min = MIN_CORRECT_INT,
                .max = MAX_CORRECT_INT };
  _items[3] = { .row = U8G2_FIFTH_ROW,
                .markerX = U8G2_MENU_MARKER_X,
                .valueX = U8G2_SECOND_COLUMN,
                .preview = PREVIEW_T4,
                .value = &sensors->_s_street->correctInt,
                .fvalue = nullptr,
                .step = 0,
                .min = MIN_CORRECT_INT,
                .max = MAX_CORRECT_INT };

  // right half, settings; min/max of the permitted offset are kept ordered
  _items[4] = { .row = U8G2_SECOND_ROW,
                .markerX = U8G2_MENU_MARKER2_X,
                .valueX = U8G2_THIRD_COLUMN,
                .preview = PREVIEW_HYST,
                .value = nullptr,
                .fvalue = &hyst,
                .step = HYST_STEP,
                .min = HYST_MIN,
                .max = HYST_MAX };
  _items[5] = { .row = U8G2_THIRD_ROW,
                .markerX = U8G2_MENU_MARKER2_X,
                .valueX = U8G2_THIRD_COLUMN,
                .preview = PREVIEW_MAX,
                .value = &maxPermOffset,
                .fvalue = nullptr,
                .step = 0,
                .min = minPermOffset,
                .max = OFFSET_MAX };
  _items[6] = { .row = U8G2_FOURTH_ROW,
                .markerX = U8G2_MENU_MARKER2_X,
                .valueX = U8G2_THIRD_COLUMN,
                .preview = PREVIEW_MIN,
                .value = &minPermOffset,
                .fvalue = nullptr,
                .step = 0,
                .min = OFFSET_MIN,
                .max = maxPermOffset };
  _items[7] = { .row = U8G2_FIFTH_ROW,
                .markerX = U8G2_MENU_MARKER2_X,
                .valueX = U8G2_THIRD_COLUMN,
                .preview = PREVIEW_USER_TEMP,
                .value = &uTemp,
                .fvalue = nullptr,
                .step = 0,
                .min = UTEMP_MIN,
                .max = UTEMP_MAX };
}

/// one decimal digit only when it is not zero: the right half of the menu
/// has ~45 px per cell and AVR has no float printf
static void
setStepText (char *buf, size_t size, float v)
{
  long scaled = static_cast<long> (v * 100.0F + (v < 0.0F ? -0.05F : 0.05F));
  long frac = scaled % 100;
  if (frac < 0)
    frac = -frac;

  if (frac == 0)
    snprintf (buf, size, "%ld", scaled / 100);
  else
    snprintf (buf, size, "%ld.%02ld", scaled / 100, frac);
}

static void
formatValue (const MenuItem &item, char *buf, size_t size)
{
  if (item.value != nullptr)
    setIntText (buf, size, *item.value);
  else if (item.fvalue != nullptr)
    setStepText (buf, size, *item.fvalue);
  else
    buf[0] = '\0';
}

static void
drawItems (U8G2 *display)
{
  char valueStr[10];
  char line[20];

  for (int i = 0; i < MENU_ITEMS_COUNT; i++)
    {
      const MenuItem &item = _items[i];
      formatValue (item, valueStr, sizeof (valueStr));

      if (i < MENU_LEFT_ITEMS)
        {
          display->drawStr (U8G2_FIRST_COLUMN, item.row, item.preview);
          display->drawStr (item.valueX, item.row, valueStr);
        }
      else
        {
          // the right half is narrower, label and value share one string
          snprintf (line, sizeof (line), "%s %s", item.preview, valueStr);
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

  const MenuItem &item = _items[_selected];

  if (item.value != nullptr)
    {
      int next = *item.value + delta;
      if (next < item.min)
        next = item.min;
      if (next > item.max)
        next = item.max;
      *item.value = next;
    }
  else if (item.fvalue != nullptr)
    {
      float next = *item.fvalue + static_cast<float> (delta) * item.step;
      if (next < static_cast<float> (item.min))
        next = static_cast<float> (item.min);
      if (next > static_cast<float> (item.max))
        next = static_cast<float> (item.max);
      *item.fvalue = next;
    }
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
