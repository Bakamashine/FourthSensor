#include "ui/menu_draw.hpp"

#include <Arduino.h>

#include "constants/settings.hpp"
#include "constants/ui.hpp"
#include "contest.hpp"
#include "helper.hpp"
#include "macro/ui.hpp"
#include "sensor.hpp"
#include "settings.hpp"
#include "ui/menu.hpp"

using namespace DeepMenu;

static void formatDeepValue(const DeepMenuItem &item, char *buf) {
  if (item.target == nullptr) {
    buf[0] = '\0';
    return;
  }
  if (item.kind == FLOAT) {
    dtostrf(*static_cast<float *>(item.target), 0, 2, buf);
  } else {
    itoa(*static_cast<int *>(item.target), buf, 10);
  }
}

void drawDeepMenu(U8G2 *display, const DeepMenu::DeepMenuItem *items, int count,
                  int selected) {
  for (int i = 0; i < count; i++) {
    const DeepMenuItem &item = items[i];
    char buf[10];
    formatDeepValue(item, buf);
    display->drawStr(U8G2_FIRST_COLUMN, item.row, item.preview);
    display->drawStr(U8G2_SECOND_COLUMN, item.row, buf);
    if (i == selected) display->drawStr(U8G2_MENU_MARKER_X, item.row, ">");
  }
}

void drawBaseMenu(U8G2 *display, const MenuItem *items, int count,
                  int selected) {
  for (int i = 0; i < count; i++)
    display->drawStr(U8G2_FIRST_COLUMN, items[i].row, items[i].preview);

  if (selected < 0 || selected >= count) return;
  const MenuItem &item = items[selected];
  display->drawStr(item.marker, item.row, ">");
}

void drawDeepMenu(U8G2 *display) {
  for (int i = 0; i < _deepMenuCount; i++) {
    const DeepMenuItem &item = _deepMenuItems[i];
    char buf[10];
    formatDeepValue(item, buf);
    display->drawStr(U8G2_FIRST_COLUMN, item.row, item.preview);
    display->drawStr(U8G2_SECOND_COLUMN, item.row, buf);
    if (i == _deepMenuSelected)
      display->drawStr(U8G2_MENU_MARKER_X, item.row, ">");
  }
}

void drawItems(U8G2 *ds) {
  for (int i = 0; i < MENU_ITEMS_COUNT; i++) {
    const MenuItem &item = _items[i];
    ds->drawStr(U8G2_FIRST_COLUMN, item.row, item.preview);
  }
}

void drawMarker(U8G2 *ds) {
  if (_baseMenuSelected < 0 || _baseMenuSelected >= MENU_ITEMS_COUNT) return;
  const MenuItem &item = _items[_baseMenuSelected];
  ds->drawStr(item.marker, item.row, ">");
}