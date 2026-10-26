#include "ui/menu_items.hpp"

#include "constants/settings.hpp"
#include "constants/ui.hpp"
#include "contest.hpp"
#include "settings.hpp"

void writeFunc(void *value, int delta, MenuItem &it) {
  if (!value) return;
  const int dir = (delta > 0) ? +1 : -1;
  switch (it.kind) {
    case INT: {
      auto *val = static_cast<int *>(value);
      const int next = *val + dir * static_cast<int>(it.step);
      if (next < static_cast<int>(it.min) || next > static_cast<int>(it.max))
        return;
      *val = next;
      if (it.save_func != nullptr) it.save_func(*val);
      break;
    }
    case FLOAT: {
      auto *val = static_cast<float *>(value);
      const float next = *val + static_cast<float>(dir) * it.step;
      if (next < it.min || next > it.max) return;
      *val = next;
      break;
    }
    default:
      return;
  }
}

void buildItems(Sensors *sensors) {
  (void)sensors;
  int count = 0;

  _items[count++] = {.row = U8G2_FIRST_ROW,
                     .preview = PREVIEW_HYST,
                     .target = &hyst,
                     .kind = INT,
                     .step = 1,
                     .save_func = _EEPROM::writeHyst,
                     .min = MIN_HYSTERESIS,
                     .max = MAX_HYSTERESIS};
#ifdef ENABLE_VALIDATE
  _items[count++] = {.row = U8G2_SECOND_ROW,
                     .preview = PREVIEW_MIN_TEMP,
                     .target = &minPermOffset,
                     .kind = INT,
                     .step = 1,
                     .save_func = _EEPROM::writeMinPermOffset,
                     .min = MIN_PERM_OFFSET,
                     .max = static_cast<float>(maxPermOffset) - 1};
  _items[count++] = {.row = U8G2_THIRD_ROW,
                     .preview = PREVIEW_MAX_TEMP,
                     .target = &maxPermOffset,
                     .kind = INT,
                     .step = 1,
                     .save_func = _EEPROM::writeMaxPermOffset,
                     .min = static_cast<float>(minPermOffset) + 1,
                     .max = MAX_PERM_OFFSET};
  _items[count++] = {.row = U8G2_FOURTH_ROW,
                     .preview = PREVIEW_USER_TEMP,
                     .target = &uTemp,
                     .kind = INT,
                     .step = 1,
                     .save_func = _EEPROM::writeUserTemp,
                     .min = static_cast<float>(minUserTemp()),
                     .max = static_cast<float>(maxUserTemp())};
#endif

  _itemsCount = count;
}
