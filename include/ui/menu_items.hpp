#pragma once
#include "sensor.hpp"
#include "settings.hpp"

#define MAX_SETTINGS_ITEMS 4

enum VALUE_TYPE { INT, FLOAT };

struct MenuItem {
  int row;
  const char *preview;
  void *target;
  VALUE_TYPE kind;
  float step;
  void (*save_func)(int);
  float min;
  float max;
};

void buildItems(Sensors &sns);
void writeFunc(void *value, int delta, MenuItem &it);

inline MenuItem _items[MAX_SETTINGS_ITEMS];
inline int _itemsCount = 0;
inline int _baseMenuSelected = 0;
inline bool isValueOpen = false;
