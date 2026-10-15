#include "button_callbacks.hpp"

#include "constants/pin.hpp"
#include "page.hpp"
#include "ui/menu.hpp"

void btnSetup() {
  Btn::plus = new OneButtonTiny(PLUS_BTN_PIN);
  Btn::minus = new OneButtonTiny(MINUS_BTN_PIN);
  Btn::menu = new OneButtonTiny(MENU_BTN_PIN);
  const int debounce = 20;
  auto mode = INPUT_PULLUP;
  pinMode(MENU_BTN_PIN, mode);
  pinMode(PLUS_BTN_PIN, mode);
  pinMode(MINUS_BTN_PIN, mode);

  Btn::menu->setDebounceMs(debounce);
  Btn::plus->setDebounceMs(debounce);
  Btn::minus->setDebounceMs(debounce);

  // Btn::menu.attachClick (btnMenuOnClick);
  Btn::menu->attachLongPressStart(btnMenuLongPress);
  Btn::plus->attachClick(btnPlusOneClick);
  Btn::plus->attachLongPressStart(btnPlusLongPress);
  Btn::minus->attachClick(btnMinusOneClick);
  Btn::minus->attachLongPressStart(btnMinusLongPress);
}

void btnMenuOnClick() {
  if (currentPage == SETTINGS && isDeepMenuOpen) {
    isDeepMenuOpen = false;
    return;
  }
  currentPage = currentPage == MAIN ? SETTINGS : MAIN;
  isDeepMenuOpen = false;
}

void btnTick() {
  Btn::menu->tick();
  Btn::plus->tick();
  Btn::minus->tick();
}

void deleteButtons() {
  delete Btn::menu;
  delete Btn::plus;
  delete Btn::minus;
}

void btnPlusLongPress() {
  if (currentPage != SETTINGS) return;
  if (!isDeepMenuOpen) {
    isDeepMenuOpen = true;
    _deepMenuSelected = 0;
  } else {
    menuSelectPrev();
  }
}

void btnPlusOneClick() {
  if (currentPage == SETTINGS && !isDeepMenuOpen) {
    menuSelectPrev();
#ifdef DEBUG
    Serial.println("goToUp");
#endif
  } else if (currentPage == SETTINGS && isDeepMenuOpen) {
    menuPageIncreaseValue();
  }
}
void btnMenuLongPress() { btnMenuOnClick(); }

void btnMinusOneClick() {
  if (currentPage == SETTINGS && !isDeepMenuOpen) {
    menuSelectNext();
#ifdef DEBUG
    Serial.println("goToDown");
#endif
  } else if (currentPage == SETTINGS && isDeepMenuOpen) {
    menuPageDecreaseValue();
  }
}
void btnMinusLongPress() {
  if (currentPage == SETTINGS && isDeepMenuOpen) menuSelectNext();
}