#include "ui/menu.hpp"

#include "ui/menu_draw.hpp"
#include "ui/menu_items.hpp"

void menuPage(UI_cls &ds, Sensors &sns) {
  (void)sns;
#ifdef ENABLE_SETTINGS
  buildItems(sns);
  if (_baseMenuSelected < 0 || _baseMenuSelected >= _itemsCount)
    _baseMenuSelected = 0;
  drawItems(ds);
  drawMarker(ds);
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
