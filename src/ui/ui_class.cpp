#include "ui/ui_class.hpp"

#include <new>

#include "burner.hpp"
#include "constants/ui.hpp"
#include "contest.hpp"
#include "macro/ui.hpp"
#include "ui/menu.hpp"

void BaseUI::init() {
  if (this->d_cls != nullptr) return;
  this->d_cls = new UICLASS(
#if __U
      U8G2_R0
#endif
  );
#if __U
  this->d_cls->begin();
  this->d_cls->setFont(FONT);
#elif __G
  this->d_cls->init();
#endif
}
void BaseUI::clear() {
#if __G
  this->d_cls->clear();
#endif
}

BaseUI::BaseUI() {}
BaseUI::~BaseUI() {
  if (this->d_cls == nullptr) return;
  this->d_cls->~UICLASS();
  ::operator delete(this->d_cls);
}
void BaseUI::drawStr(int x, int y, const char* s) {
#if __U
  this->d_cls->drawStr(x, y, s);
#elif __G
  this->d_cls->setCursor(x, y);
  // this->d_cls->setScale(2);
  this->d_cls->print(s);
#endif
}

UI_cls::UI_cls() : BaseUI() {}

void UI_cls::mainPage(Sensors& sns) {
  // columns
  RAW_WRITE_ROW(U8G2_MAIN_ROW(0), nullptr, PREVIEW_ACP_TEXT,
                PREVIEW_COLUMN_TEMP, *this);
  // rows

  // first sensor
  if (sns._s_first->temp)
    N_AT_WRITE_ROW(U8G2_MAIN_ROW(1), PREVIEW_TEMP, sns._s_first->temp,
                   sns._s_first->acp, *this);

  // second sensor
  if (sns._s_second->temp)
    N_AT_WRITE_ROW(U8G2_MAIN_ROW(2), PREVIEW_FIRST_RESERVE, sns._s_second->temp,
                   sns._s_second->acp, *this);

  // second reserve sensor
  if (sns._s_third->temp)
    N_AT_WRITE_ROW(U8G2_MAIN_ROW(3), PREVIEW_SECOND_RESERVE, sns._s_third->temp,
                   sns._s_third->acp, *this);

  // street sensor
  if (sns._s_fourth->temp)
    N_AT_WRITE_ROW(U8G2_MAIN_ROW(4), PREVIEW_STREET, sns._s_fourth->temp,
                   sns._s_fourth->acp, *this);

#ifdef DS18_SENSOR
  char ds18Buf[10];
  dtostrf(Sens::ds18Temp, 0, 2, ds18Buf);
  RAW_WRITE_ROW(U8G2_MAIN_ROW(5), PREVIEW_DS18, nullptr, ds18Buf, *this);
#endif
}

void UI_cls::drawPage() {
  switch (currentPage) {
    case START: {
      startPage();
      break;
    }
    case MAIN: {
      mainPage(Sens::all);
      break;
    }
    case SETTINGS: {
      menuPage(*this, Sens::all);
      break;
    }
#ifdef ENABLE_VALIDATE
    case ERROR: {
      errorPage(Val::errorCode);
      break;
    }
#endif
  }
}

void UI_cls::draw() {
#if __U
  this->d_cls->firstPage();
  do {
    this->drawPage();
  } while (this->d_cls->nextPage());
#else
  this->clear();
  this->drawPage();
#endif
}

void UI_cls::startPage() {
  RAW_WRITE_ROW(U8G2_MAIN_ROW(0), PREVIEW_VENT, YES_OR_NO(loop_vent_status),
                nullptr, *this);
  RAW_WRITE_ROW(U8G2_MAIN_ROW(1), PREVIEW_IGN, YES_OR_NO(loop_ign_trans),
                nullptr, *this);
  RAW_WRITE_ROW(U8G2_MAIN_ROW(2), PREVIEW_PIL, YES_OR_NO(loop_pil_val), nullptr,
                *this);
  RAW_WRITE_ROW(U8G2_MAIN_ROW(3), PREVIEW_BUR, YES_OR_NO(loop_bur_sens),
                nullptr, *this);
}

void UI_cls::closeStartPage() { currentPage = MAIN; }
