#include "button_callbacks.hpp"

#include "constants/pin.hpp"
#include "page.hpp"
#include "ui/menu.hpp"
#include "ui/menu_items.hpp"
#include "validate.hpp"

static void btnBurnerOnClick() { setBurnerStatus(true); }

// MAIN:      long press menu -> open settings
// SETTINGS:  click +/-       -> move selection
//            long press +    -> enter edit mode
//            long press -    -> leave edit mode
//            in edit mode +/- -> change the selected value
//            long menu        -> close edit mode / leave settings
static void btnMenuOnClick() {
  if (currentPage == SETTINGS && isValueOpen) {
    isValueOpen = false;
    return;
  }
  if (currentPage == SETTINGS) {
    exitMenu();
  }
  currentPage = currentPage == MAIN ? SETTINGS : MAIN;
  isValueOpen = false;
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
  if (currentPage == SETTINGS) isValueOpen = true;
}

static void btnPlusOneClick() {
  if (currentPage != SETTINGS) return;
  if (isValueOpen)
    menuPageIncreaseValue();
  else
    menuSelectPrev();
}

static void btnMenuLongPress() { btnMenuOnClick(); }

static void btnMinusOneClick() {
  if (currentPage != SETTINGS) return;
  if (isValueOpen)
    menuPageDecreaseValue();
  else
    menuSelectNext();
}

static void btnMinusLongPress() {
  if (currentPage == SETTINGS) isValueOpen = false;
}

void btnSetup() {
  const int debounce = 20;

  Btn::menu.setDebounceMs(debounce);
  Btn::plus.setDebounceMs(debounce);
  Btn::minus.setDebounceMs(debounce);
  // fire every click immediately so repeated +/- presses are not swallowed as
  // a double click
  Btn::menu.setClickMs(0);
  Btn::plus.setClickMs(0);
  Btn::minus.setClickMs(0);
#ifdef BURNER_BTN
  Btn::burner_btn.setDebounceMs(debounce);
  Btn::burner_btn.attachLongPressStart(btnBurnerOnClick);
  attachInterrupt(digitalPinToInterrupt(BURNER_BTN_PIN), btnBurnerOnClick,
                  FALLING);
#endif
  Btn::menu.attachLongPressStart(btnMenuLongPress);
  Btn::plus.attachClick(btnPlusOneClick);
  Btn::plus.attachLongPressStart(btnPlusLongPress);
  Btn::minus.attachClick(btnMinusOneClick);
  Btn::minus.attachLongPressStart(btnMinusLongPress);
}
