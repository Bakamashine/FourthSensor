#include "validate.hpp"

#include "burner.hpp"
#include "constants/pin.hpp"
#include "helper.hpp"
#include "page.hpp"
#include "sensor.hpp"
#include "settings.hpp"
#define PERMITTED_TEMP_DIFFERENCE 5

namespace Val {
int checkSensors(Sensors &sn) {
  /*
  codes:
  100 - all sensors agree
  101 - first sensor is defective
  102 - second sensor is defective
  103 - third sensor is defective
  104 - fourth sensor is defective

  */
  return 0;  // no error
}

int checkTemp(Sensors &sns) {
#ifdef ENABLE_VALIDATE
  float averageVal = getFAverageTemp(sns);

  // the bounds themselves are valid readings, so compare strictly
  if (maxPermOffset - hyst < averageVal) {
    return 201;  // average too hot
  }
  if (minPermOffset + hyst > averageVal) {
    return 202;  // average too cold
  }
#endif

  return 0;  // no error
}

static int basePipeline(Sensors &sns) {
  if (!getReadySensors(sns) || errorCode > 0) return 0;
  int sensor_code = checkSensors(sns);
  int temperature_code = checkTemp(sns);

  int ret_code = 0;
  if (sensor_code > 0)
    ret_code = sensor_code;
  else if (temperature_code > 0)
    ret_code = temperature_code;

  return ret_code;
}

void pipeline(Sensors &sns) {
  int code = basePipeline(sns);
  if (code > 0) {
    errorCode = code;
    haltSystem();
  }
}

int pipelineWithoutHalt(Sensors &sns) { return basePipeline(sns); }

void haltSystem() {
  if (!systemHalted) {
    Serial.print("ERROR: ");
    Serial.println(errorCode);
    digitalWrite(BURNER_PIN, LOW);
#ifdef BURNER_PREPARE
    digitalWrite(VENT_LED_PIN, LOW);
    digitalWrite(IGN_PIN, LOW);
    digitalWrite(PILOT_PIN, LOW);
    digitalWrite(BURNER_SENSOR_LED_PIN, LOW);
#endif
    digitalWrite(ERROR_PIN, HIGH);
  }
  systemHalted = 1;
  currentPage = ERROR;
}

void checkError() {
  if (errorCode > 0) {
    return haltSystem();
  }
}
}  // namespace Val
