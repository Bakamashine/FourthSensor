#include "led.hpp"

void ledDebug() {
  bool status = false;
  for (int i = 0; i < 2; i++) {
    digitalWrite(BURNER_PIN, !status);
    digitalWrite(ERROR_PIN, status);
    digitalWrite(VENT_LED_PIN, !status);
    digitalWrite(IGN_PIN, status);
    digitalWrite(PILOT_PIN, !status);
    digitalWrite(BURNER_SENSOR_LED_PIN, status);
    digitalWrite(SQUIB_LED_PIN, !status);
    delay(200);
    status = true;
  }
}

void ledSetup() {
#ifdef ENABLE_BURNER
  pinMode(BURNER_PIN, OUTPUT);
#endif
#ifdef ENABLE_VALIDATE
  // error
  pinMode(ERROR_PIN, OUTPUT);
#endif
  pinMode(VENT_LED_PIN, OUTPUT);
  pinMode(IGN_PIN, OUTPUT);
  pinMode(PILOT_PIN, OUTPUT);
  pinMode(BURNER_SENSOR_LED_PIN, OUTPUT);
  pinMode(SQUIB_LED_PIN, OUTPUT);
}