#include "button_callbacks.hpp"

#include "constants/pin.hpp"
#include "page.hpp"
#include "ui/menu.hpp"

namespace Btn {
OneButtonTiny plus(PLUS_BTN_PIN);
OneButtonTiny minus(MINUS_BTN_PIN);
OneButtonTiny menu(MENU_BTN_PIN);
}  // namespace Btn

void btnSetup() {
  const int debounce = 20;

  Btn::menu.setDebounceMs(debounce);
  Btn::plus.setDebounceMs(debounce);
  Btn::minus.setDebounceMs(debounce);

  // Btn::menu.attachClick (btnMenuOnClick);
  Btn::menu.attachLongPressStart(btnMenuLongPress);
  Btn::plus.attachClick(btnPlusOneClick);
  Btn::plus.attachLongPressStart(btnPlusLongPress);
  Btn::minus.attachClick(btnMinusOneClick);
  Btn::minus.attachLongPressStart(btnMinusLongPress);
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
  Btn::menu.tick();
  Btn::plus.tick();
  Btn::minus.tick();
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