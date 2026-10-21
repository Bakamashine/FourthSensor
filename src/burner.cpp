#include "burner.hpp"

#include <avr/wdt.h>

#include "constants/pin.hpp"
#include "sensor.hpp"
#include "settings.hpp"
#include "validate.hpp"

#define ATTEMPTS 5

static constexpr int vent_delay = 1000 * 5;         // 5 sec
static constexpr int ignition_fp_delay = 1000 * 2;  // 2 sec
static constexpr int ignition_sp_delay = 1000 * 3;  // 3 sec
static constexpr int ignition_delay =
    ignition_fp_delay + ignition_sp_delay;  // 5 sec
static constexpr int pilot_delay = 1000 * 5;
static constexpr int pilot_valve_delay = 1000 * 3;      // pilot valve on time
static constexpr int pilot_valve_off_delay = 1000 * 2;  // pilot valve off time
static constexpr int burner_sensor_window = 1000 * 2;   // burner signal search
static constexpr int burner_sensor_step = burner_sensor_window / 5;

static bool _vent_status = false;
static bool _ign_trans = false;
static bool _pil_val = false;
static bool _bur_sens = false;

namespace Burner {
bool getBurnerStatus() { return static_cast<bool>(digitalRead(BURNER_PIN)); }

bool checkTemp(Sensors *sn) {
  if (Val::errorCode > 0) return false;
  int averageTemp = getIAverageTemp(sn);
  if (averageTemp - hyst <= uTemp) return true;
  return false;
}

static void changeStatus(bool *p_st, int pin) {
  *p_st = !(*p_st);
  digitalWrite(pin, *p_st);
}

// blocking delay that keeps feeding the watchdog (< WDTO_4S)
static void burnerDelay(int ms) {
  for (int elapsed = 0; elapsed < ms; elapsed += 250) {
    wdt_reset();
    delay(250);
  }
}

static bool flameDetected() {
  changeStatus(&_bur_sens, BURNER_SENSOR_LED_PIN);
  bool flameSeen = false;
  for (int i = 0; i < 5; i++) {
    burnerDelay(burner_sensor_step);
    if (getBurnerStatus()) {
      flameSeen = true;
      break;
    }
  }
  changeStatus(&_bur_sens, BURNER_SENSOR_LED_PIN);
  return flameSeen;
}

static void checkNoFlame() {
  if (flameDetected()) Val::errorCode = 302;
}

static void checkFlamePresent() {
  if (flameDetected()) isBurnerReady = true;
}

// blue led
static void ventilation() {
  changeStatus(&_vent_status, VENT_LED_PIN);
  burnerDelay(vent_delay);
  changeStatus(&_vent_status, VENT_LED_PIN);
}

// yellow led
static void pilotValve() {
  changeStatus(&_pil_val, PILOT_PIN);
  burnerDelay(pilot_valve_delay);
  changeStatus(&_ign_trans, IGN_PIN);
  // burnerDelay(pilot_delay);
  burnerDelay(pilot_valve_off_delay);
  changeStatus(&_pil_val, PILOT_PIN);
}

// white led
static void ignitionTransformer() {
  changeStatus(&_ign_trans, IGN_PIN);
  burnerDelay(ignition_fp_delay);
  pilotValve();
  // changeStatus(&_ign_trans, IGN_PIN);
}

void burnerSystem() {
  if (manualBurnerStatus) return;

  if (!getReadySensors(&Sens::all)) return;

  burnerStatus = checkTemp(&Sens::all);
  digitalWrite(BURNER_PIN, burnerStatus);
}

void prepare() {
  for (unsigned int i = 0; i < ATTEMPTS; i++) {
    ventilation();
    checkNoFlame();
    // error
    if (Val::errorCode > 0) return;
    ignitionTransformer();
    checkFlamePresent();

    // success
    if (isBurnerReady && Val::errorCode == 0) break;
  }

  if (!isBurnerReady && Val::errorCode == 0) Val::errorCode = 301;
}
}  // namespace Burner
