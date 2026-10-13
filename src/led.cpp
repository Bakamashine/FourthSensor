#include "led.hpp"

void ledDebug() {
  digitalWrite(BURNER_PIN, HIGH);
  digitalWrite(ERROR_PIN, LOW);
  delay(200);
  digitalWrite(BURNER_PIN, LOW);
  digitalWrite(ERROR_PIN, HIGH);
}

void ledSetup() {
#ifdef ENABLE_BURNER
  pinMode(BURNER_PIN, OUTPUT);
#endif
#ifdef ENABLE_VALIDATE
  // error
  pinMode(ERROR_PIN, OUTPUT);
#endif
}