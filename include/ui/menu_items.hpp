#pragma once
#include "sensor.hpp"
#include "settings.hpp"

#define MENU_ITEMS_COUNT 5
#define MAX_DEEP_ITEMS 4
#define INT_SUMMAND 1
#define F_SUMMAND 0.01F
struct MenuItem {
  int row;
  int marker;
  const char *preview;
  Sensor *sensor;
};

void buildItems(Sensors *sns);
inline int _baseMenuSelected = 0;
namespace DeepMenu {
inline int _hyst = hyst;
#ifdef ENABLE_VALIDATE
inline int _minTemp = minPermOffset;
inline int _maxTemp = maxPermOffset;
inline int _uTemp = uTemp;
#endif
inline int _deepMenuCount = 0;
inline int _deepMenuSelected = 0;

enum VALUE_TYPE { INT, FLOAT };
void buildSensorDeepMenu(Sensor *);
void buildOtherDeepMenu();
struct DeepMenuItem {
  int row;
  const char *preview;
  void *target;
  Sensor *sn;
  VALUE_TYPE kind;
  float step;
  // void (*write_func)(void *value, int delta, DeepMenuItem *it);
  void (*fsave_func)(float);
  void (*save_func)(int);
  float min;
  float max;
  // int addr;
  bool dirty;
};
void writeFunc(void *value, int delta, DeepMenuItem &it);

inline DeepMenuItem _deepMenuItems[MAX_DEEP_ITEMS];
inline bool isDeepMenuOpen = false;
}  // namespace DeepMenu

inline MenuItem _items[MENU_ITEMS_COUNT];
