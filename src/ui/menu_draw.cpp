#include "ui/menu_draw.hpp"

#include <Arduino.h>
#include <string.h>

#include "constants/ui.hpp"
#include "ui/menu_items.hpp"
#include "ui/ui_class.hpp"

static void formatValue(const MenuItem &item, char *buf) {
  if (item.target == nullptr) {
    buf[0] = '\0';
    return;
  }
  if (item.kind == FLOAT) {
    float value;
    memcpy(&value, item.target, sizeof(value));
    dtostrf(value, 0, 2, buf);
  } else {
    int value;
    memcpy(&value, item.target, sizeof(value));
    itoa(value, buf, 10);
  }
}

void drawItems(UI_cls &ds) {
  for (int i = 0; i < _itemsCount; i++) {
    const MenuItem &item = _items[i];
    char buf[10];
    formatValue(item, buf);
    ds.drawStr(U8G2_FIRST_COLUMN, item.row, item.preview);
    ds.drawStr(U8G2_SECOND_COLUMN, item.row, buf);
  }

  if (isValueOpen) ds.drawStr(U8G2_EDIT_X, U8G2_EDIT_Y, "ON");
}

void drawMarker(UI_cls &ds) {
  if (_baseMenuSelected < 0 || _baseMenuSelected >= _itemsCount) return;
  const MenuItem &item = _items[_baseMenuSelected];
  ds.drawStr(U8G2_MENU_MARKER_X, item.row, ">");
}
