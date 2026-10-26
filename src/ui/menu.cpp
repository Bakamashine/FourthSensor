#include "ui/menu.hpp"

#include "constants/settings.hpp"
#include "ui/menu_draw.hpp"
#include "ui/menu_items.hpp"

void menuPage(U8G2 &display, Sensors &sensors) {
  (void)sensors;
#ifdef ENABLE_SETTINGS
  buildItems(sensors);
  if (_baseMenuSelected < 0 || _baseMenuSelected >= _itemsCount)
    _baseMenuSelected = 0;
  drawItems(display);
  drawMarker(display);
#endif
}

static void changeValue(int delta) {
  if (_baseMenuSelected < 0 || _baseMenuSelected >= _itemsCount) return;
  MenuItem &item = _items[_baseMenuSelected];
  writeFunc(item.target, delta, item);
}

void menuPageIncreaseValue() { changeValue(1); }

void menuPageDecreaseValue() { changeValue(-1); }

void menuSelectNext() {
  if (_itemsCount > 0)
    _baseMenuSelected = (_baseMenuSelected + 1) % _itemsCount;
}

void menuSelectPrev() {
  if (_itemsCount > 0)
    _baseMenuSelected = (_baseMenuSelected - 1 + _itemsCount) % _itemsCount;
}

void exitMenu() {
  _baseMenuSelected = 0;
  isValueOpen = false;
}
