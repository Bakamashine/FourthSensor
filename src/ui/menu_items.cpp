#include "ui/menu_items.hpp"

#include "constants/addr_eeprom.hpp"
#include "constants/settings.hpp"
#include "constants/ui.hpp"
#include "contest.hpp"
#include "helper.hpp"
#include "macro/ui.hpp"
#include "sensor.hpp"
#include "settings.hpp"
#include "ui/menu.hpp"

using namespace DeepMenu;

namespace DeepMenu {
static int sensEepromAddr(Sensor* sn) {
  if (sn == &Sens::first) return ADDR_SENS1_SETTINGS;
  if (sn == &Sens::second) return ADDR_SENS2_SETTINGS;
  if (sn == &Sens::third) return ADDR_SENS3_SETTINGS;
  return ADDR_SENS4_SETTINGS;
}

void writeFunc(void* value, int delta, DeepMenu::DeepMenuItem& it) {
  if (!value) return;
  const int dir = (delta > 0) ? +1 : -1;
  switch (it.kind) {
    case DeepMenu::INT: {
      auto* val = static_cast<int*>(value);
      const int next = *val + dir * static_cast<int>(it.step);
      if (next < static_cast<int>(it.min) || next > static_cast<int>(it.max))
        return;
      *val = next;
      if (it.save_func != nullptr) it.save_func(*val);
      break;
    }
    case DeepMenu::FLOAT: {
      auto* val = static_cast<float*>(value);
      const float next = *val + dir * it.step;
      if (next < it.min || next > it.max) return;
      *val = next;
      if (it.fsave_func != nullptr) it.fsave_func(*val);
      break;
    }

    default:
      return;
  }
  if (it.sn != nullptr) writeSensSett(&it.sn->settings, sensEepromAddr(it.sn));
}

void buildOtherDeepMenu() {
  int count = 0;
  DeepMenu::_deepMenuItems[count++] = {.row = U8G2_FIRST_ROW,
                                       .preview = "Hyst",
                                       .target = &hyst,
                                       .sn = nullptr,
                                       .kind = DeepMenu::INT,
                                       .step = 1,
                                       .fsave_func = nullptr,
                                       .save_func = _EEPROM::writeHyst,
                                       .min = 0,
                                       .max = 10,
                                       .dirty = false};
#ifdef ENABLE_VALIDATE
  DeepMenu::_deepMenuItems[count++] = {
      .row = U8G2_SECOND_ROW,
      .preview = "Max",
      .target = &maxPermOffset,
      .sn = nullptr,
      .kind = DeepMenu::INT,
      .step = 1,
      .fsave_func = nullptr,
      .save_func = _EEPROM::writeMaxPermOffset,
      .min = static_cast<float>(minPermOffset) + 1,
      .max = MAX_PERM_OFFSET,
      .dirty = false};
  DeepMenu::_deepMenuItems[count++] = {
      .row = U8G2_THIRD_ROW,
      .preview = "Min",
      .target = &minPermOffset,
      .sn = nullptr,
      .kind = DeepMenu::INT,
      .step = 1,
      .fsave_func = nullptr,
      .save_func = _EEPROM::writeMinPermOffset,
      .min = MIN_PERM_OFFSET,
      .max = static_cast<float>(maxPermOffset) - 1,
      .dirty = false};
  const int lastRow = U8G2_FIRST_ROW + count * U8G2_ROW_STEP;
  DeepMenu::_deepMenuItems[count++] = {
      .row = lastRow,
      .preview = "UserT",
      .target = &uTemp,
      .sn = nullptr,
      .kind = DeepMenu::INT,
      .step = 1,
      .fsave_func = nullptr,
      .save_func = _EEPROM::writeUserTemp,
      .min = static_cast<float>(minPermOffset) + hyst + 1,
      .max = static_cast<float>(maxPermOffset) - hyst - 1,
      .dirty = false};
#endif
  DeepMenu::_deepMenuCount = count;
}

void buildSensorDeepMenu(Sensor* sensor) {
  DeepMenu::_deepMenuCount = 1;
  DeepMenu::_deepMenuItems[0] = {.row = U8G2_FIRST_ROW,
                                 .preview = "EMA",
                                 .target = &sensor->settings.ema_k,
                                 .sn = sensor,
                                 .kind = DeepMenu::INT,
                                 .step = 1,
                                 .fsave_func = nullptr,
                                 .save_func = nullptr,
                                 .min = 1,
                                 .max = 10,
                                 .dirty = false};
}

}  // namespace DeepMenu

void buildItems(Sensors* sensors) {
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
