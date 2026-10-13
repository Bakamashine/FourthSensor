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
  currentPage = currentPage == MAIN ? SETTINGS : MAIN;
  isValueOpen = false;
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
  if (currentPage == SETTINGS) isValueOpen = 1;
}

void btnPlusOneClick() {
  if (currentPage == SETTINGS && !isValueOpen) {
    menuSelectPrev();
#ifdef DEBUG
    Serial.println("goToUp");
#endif
  } else if (currentPage == SETTINGS && isValueOpen) {
    menuPageIncreaseValue();
  }
}
void btnMenuLongPress() { btnMenuOnClick(); }

void btnMinusOneClick() {
  if (currentPage == SETTINGS && !isValueOpen) {
    menuSelectNext();
#ifdef DEBUG
    Serial.println("goToDown");
#endif
  } else if (currentPage == SETTINGS && isValueOpen) {
    menuPageDecreaseValue();
  }
}
void btnMinusLongPress() {
  if (currentPage == SETTINGS) isValueOpen = 0;
}