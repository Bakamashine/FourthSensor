#include "ui/menu.hpp"

#include "constants/settings.hpp"
#include "contest.hpp"
#include "helper.hpp"
#include "macro/ui.hpp"
#include "sensor.hpp"
#include "settings.hpp"

/// bounds for the settings that are not sensor corrections
#define HYST_MIN 0
#define HYST_MAX 50
#define HYST_STEP 0.01F
#define OFFSET_MIN 0
#define OFFSET_MAX 200
#define UTEMP_MIN 0
#define UTEMP_MAX 100

#define MENU_ITEMS_COUNT 5
#define MAX_DEEP_ITEMS 4

#define INT_SUMMAND  1
#define F_SUMMAND 0.01F


struct MenuItem {
  int row;
  int marker;
  const char *preview;
  Sensor *sensor;
};

enum DeepValueKind { DEEP_INT, DEEP_FLOAT };

struct DeepMenuItem {
  int row;
  const char *preview;
  void *target;
  DeepValueKind kind;
  float step;
  void (*fwrite_func)(float);
  void (*write_func)(int);
  float min;
  float max;
};

static MenuItem _items[MENU_ITEMS_COUNT];
static DeepMenuItem _deepMenuItems[MAX_DEEP_ITEMS];
static int _deepMenuCount = 0;

#ifdef ENABLE_SETTINGS
static void buildItems(Sensors *sensors) {
  _items[0] = {.row = U8G2_FIRST_ROW,
               .marker = 0,
               .preview = PREVIEW_S1,
               .sensor = sensors->_s_first};
  _items[1] = {.row = U8G2_SECOND_ROW,
               .marker = 0,
               .preview = PREVIEW_S2,
               .sensor = sensors->_s_second};
  _items[2] = {.row = U8G2_THIRD_ROW,
               .marker = 0,
               .preview = PREVIEW_S3,
               .sensor = sensors->_s_third};
  _items[3] = {.row = U8G2_FOURTH_ROW,
               .marker = 0,
               .preview = PREVIEW_S4,
               .sensor = sensors->_s_fourth};
  _items[4] = {.row = U8G2_FIFTH_ROW,
               .marker = 0,
               .preview = PREVIEW_OTHER,
               .sensor = nullptr};
}

static void buildSensorDeepMenu(Sensor *sensor) {
  _deepMenuCount = 1;
  _deepMenuItems[0] = {.row = U8G2_FIRST_ROW,
                       .preview = "EMA",
                       .target = &sensor->settings.ema_k,
                       .kind = DEEP_INT,
                       .step = 1,
                       .fwrite_func = nullptr,
                       .write_func = nullptr,
                       .min = 1,
                       .max = 10};
}

static void buildOtherDeepMenu() {
  int count = 0;
  _deepMenuItems[count] = {.row = U8G2_FIRST_ROW,
                           .preview = "Hyst",
                           .target = &hyst,
                           .kind = DEEP_FLOAT,
                           .step = HYST_STEP,
                           .fwrite_func = _EEPROM::writeHyst,
                           .write_func = nullptr,
                           .min = HYST_MIN,
                           .max = HYST_MAX};
  count++;
#ifdef ENABLE_VALIDATE
  _deepMenuItems[count] = {.row = U8G2_SECOND_ROW,
                           .preview = "Max",
                           .target = &maxPermOffset,
                           .kind = DEEP_INT,
                           .step = 1,
                           .fwrite_func = nullptr,
                           .write_func = _EEPROM::writeMaxPermOffset,
                           .min = static_cast<float>(minPermOffset),
                           .max = OFFSET_MAX};
  count++;
  _deepMenuItems[count] = {.row = U8G2_THIRD_ROW,
                           .preview = "Min",
                           .target = &minPermOffset,
                           .kind = DEEP_INT,
                           .step = 1,
                           .fwrite_func = nullptr,
                           .write_func = _EEPROM::writeMinPermOffset,
                           .min = OFFSET_MIN,
                           .max = static_cast<float>(maxPermOffset)};
  count++;
#endif
  _deepMenuItems[count] = {.row = U8G2_FIRST_ROW + count * U8G2_ROW_STEP,
                           .preview = "UserT",
                           .target = &uTemp,
                           .kind = DEEP_INT,
                           .step = 1,
                           .fwrite_func = nullptr,
                           .write_func = _EEPROM::writeUserTemp,
                           .min = UTEMP_MIN,
                           .max = UTEMP_MAX};
  count++;
  _deepMenuCount = count;
}

static void formatDeepValue(const DeepMenuItem &item, char *buf, size_t size) {
  if (item.kind == DEEP_FLOAT) {
    dtostrf(*static_cast<float *>(item.target), 0, 2, buf);
  } else {
    itoa(*static_cast<int *>(item.target), buf, 10);
  }
}

static void drawDeepMenu(U8G2 *display) {
  for (int i = 0; i < _deepMenuCount; i++) {
    const DeepMenuItem &item = _deepMenuItems[i];
    char buf[10];
    formatDeepValue(item, buf, sizeof(buf));
    display->drawStr(U8G2_FIRST_COLUMN, item.row, item.preview);
    display->drawStr(U8G2_SECOND_COLUMN, item.row, buf);
    if (i == _deepMenuSelected)
      display->drawStr(U8G2_MENU_MARKER_X, item.row, ">");
  }
}
#endif

#ifdef ENABLE_SETTINGS

static void drawItems(U8G2 *display) {
  for (int i = 0; i < MENU_ITEMS_COUNT; i++) {
    const MenuItem &item = _items[i];
    display->drawStr(U8G2_FIRST_COLUMN, item.row, item.preview);
  }
}

static void drawMarker(U8G2 *display) {
  if (_baseMenuSelected < 0 || _baseMenuSelected >= MENU_ITEMS_COUNT) return;
  const MenuItem &item = _items[_baseMenuSelected];
  display->drawStr(item.marker, item.row, ">");
}
#endif  // ENABLE_SETTINGS

void menuPage(U8G2 *display, Sensors *sensors) {
#ifdef ENABLE_SETTINGS
  buildItems(sensors);
  if (isDeepMenuOpen && _baseMenuSelected >= 0 &&
      _baseMenuSelected < MENU_ITEMS_COUNT) {
    Sensor *selected = _items[_baseMenuSelected].sensor;
    if (selected != nullptr) {
      buildSensorDeepMenu(selected);
#ifdef EMA_FILTER
      selected->ema.setK(static_cast<uint8_t>(selected->settings.ema_k));
#endif
    } else {
      buildOtherDeepMenu();
    }
    if (_deepMenuSelected < 0 || _deepMenuSelected >= _deepMenuCount)
      _deepMenuSelected = 0;
    drawDeepMenu(display);
    return;
  }
  drawItems(display);
  drawMarker(display);
#endif
}

static void changeDeepValue(int delta) {
  if (_deepMenuSelected < 0 || _deepMenuSelected >= _deepMenuCount) return;
  DeepMenuItem &item = _deepMenuItems[_deepMenuSelected];
  
  // decrease
  if (delta < 0) {
    if (item.fwrite_func != nullptr)
      item.fwrite_func(*static_cast<float *>(item.target)-F_SUMMAND);
    else if (item.write_func != nullptr)
      item.write_func(*static_cast<int *>(item.target)-INT_SUMMAND);
  }
  // increase
  else if (delta > 0) {
    if (item.fwrite_func != nullptr)
      item.fwrite_func(*static_cast<float *>(item.target)+F_SUMMAND);
    else if (item.write_func != nullptr)
      item.write_func(*static_cast<int *>(item.target)+INT_SUMMAND);
  }
  // if (item.kind == DEEP_FLOAT) {
  //   float *p = static_cast<float *>(item.target);
  //   float next = *p + static_cast<float>(delta) * item.step;
  //   if (next < item.min) next = item.min;
  //   if (next > item.max) next = item.max;
  //   *p = next;
  // } else {
  //   int *p = static_cast<int *>(item.target);
  //   long next = static_cast<long>(*p) + delta;
  //   if (next < static_cast<long>(item.min)) next =
  //   static_cast<long>(item.min); if (next > static_cast<long>(item.max)) next
  //   = static_cast<long>(item.max); *p = static_cast<int>(next);
  // }
}

void menuPageIncreaseValue() { changeDeepValue(1); }

void menuPageDecreaseValue() { changeDeepValue(-1); }

void menuSelectNext() {
  if (!isDeepMenuOpen) {
    _baseMenuSelected = (_baseMenuSelected + 1) % MENU_ITEMS_COUNT;
  } else if (_deepMenuCount > 0) {
    _deepMenuSelected = (_deepMenuSelected + 1) % _deepMenuCount;
  }
}

void menuSelectPrev() {
  if (!isDeepMenuOpen) {
    _baseMenuSelected =
        (_baseMenuSelected - 1 + MENU_ITEMS_COUNT) % MENU_ITEMS_COUNT;
  } else if (_deepMenuCount > 0) {
    _deepMenuSelected =
        (_deepMenuSelected - 1 + _deepMenuCount) % _deepMenuCount;
  }
}
