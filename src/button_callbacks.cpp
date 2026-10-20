#include "button_callbacks.hpp"

#include "constants/pin.hpp"
#include "page.hpp"
#include "ui/menu.hpp"
#include "ui/menu_items.hpp"
#include "validate.hpp"

using namespace DeepMenu;

static void btnBurner() { setBurnerStatus(true); }

static void btnMenuOnClick() {
  if (currentPage == SETTINGS && isDeepMenuOpen) {
    isDeepMenuOpen = false;
    return;
  }
  if (currentPage == SETTINGS) {
    exitMenu();
  }
  currentPage = currentPage == MAIN ? SETTINGS : MAIN;
  isDeepMenuOpen = false;
}

void btnTick() {
  Btn::menu.tick();
  Btn::plus.tick();
  Btn::minus.tick();
}

void btnTickBurner() {
#ifdef BURNER_BTN
  Btn::burner_btn.tick();
#endif
}

static void btnPlusLongPress() {
  if (currentPage != SETTINGS) return;
  if (!isDeepMenuOpen) {
    isDeepMenuOpen = true;
    _deepMenuSelected = 0;
  } else {
    menuSelectPrev();
  }
}

static void btnPlusOneClick() {
  if (currentPage == SETTINGS && !isDeepMenuOpen) {
    menuSelectPrev();
  } else if (currentPage == SETTINGS && isDeepMenuOpen) {
    menuPageIncreaseValue();
  }
}
static void btnMenuLongPress() { btnMenuOnClick(); }

static void btnMinusOneClick() {
  if (currentPage == SETTINGS && !DeepMenu::isDeepMenuOpen) {
    menuSelectNext();
  } else if (currentPage == SETTINGS && isDeepMenuOpen) {
    menuPageDecreaseValue();
  }
}
static void btnMinusLongPress() {
  if (currentPage == SETTINGS && isDeepMenuOpen) menuSelectNext();
}

void btnSetup() {
  const int debounce = 20;

  Btn::menu.setDebounceMs(debounce);
  Btn::plus.setDebounceMs(debounce);
  Btn::minus.setDebounceMs(debounce);
#ifdef BURNER_BTN
  Btn::burner_btn.setDebounceMs(debounce);
  Btn::burner_btn.attachLongPressStart(btnBurner);
#endif
  // Btn::menu.attachClick (btnMenuOnClick);
  Btn::menu.attachLongPressStart(btnMenuLongPress);
  Btn::plus.attachClick(btnPlusOneClick);
  Btn::plus.attachLongPressStart(btnPlusLongPress);
  Btn::minus.attachClick(btnMinusOneClick);
  Btn::minus.attachLongPressStart(btnMinusLongPress);
}