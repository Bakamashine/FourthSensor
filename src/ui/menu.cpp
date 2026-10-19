#include "ui/menu.hpp"

#include "constants/settings.hpp"
#include "contest.hpp"
#include "helper.hpp"
#include "macro/ui.hpp"
#include "sensor.hpp"
#include "settings.hpp"
#include "ui/menu_draw.hpp"
#include "ui/menu_items.hpp"

using namespace DeepMenu;

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
  writeFunc(item.target, delta, item);
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

void exitMenu() {
  _baseMenuSelected = 0;
  _deepMenuSelected = 0;
  isDeepMenuOpen = false;
}