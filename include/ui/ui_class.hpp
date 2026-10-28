#pragma once
#include "macro/general.hpp"
#include "page.hpp"
#include "sensor.hpp"
#include "validate.hpp"

using namespace Sens;
class BaseUI {
 protected:
  UICLASS *d_cls = nullptr;

 public:
  BaseUI();
  void init();
  void drawStr(int x, int y, const char *s);
  virtual ~BaseUI();
  void clear();
};

class UI_cls : public BaseUI {
 public:
  UI_cls();
  void mainPage(Sensors &);
  void startPage();
  void errorPage(int err_c);
  void draw();
  void drawPage();
  void closeStartPage();
};

inline UI_cls ui_cls;