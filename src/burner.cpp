#include "burner.hpp"

#include "constants/pin.hpp"
#include "sensor.hpp"
#include "settings.hpp"
#include "validate.hpp"

namespace Burner {
bool checkTemp(Sensors *sn) {
  if (Validate::errorCode > 0) return false;
  int averageTemp = getIAverageTemp(sn);
  // if (averageTemp + hyst >= uTemp)
  //   return false;
  if (averageTemp - hyst <= uTemp) return true;
  return false;
}
void burnerSystem() {
  if (checkTemp(&Sens::all)) {
    digitalWrite(BURNER_PIN, HIGH);
  } else {
    digitalWrite(BURNER_PIN, LOW);
  }
}
}  // namespace Burner
