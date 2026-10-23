#include "validate.hpp"

#include "burner.hpp"
#include "constants/pin.hpp"
#include "helper.hpp"
#include "page.hpp"
#include "sensor.hpp"
#include "settings.hpp"
#define PERMITTED_TEMP_DIFFERENCE 5

namespace Val {
int checkSensors(Sensors *sn) {
  /*
  codes:
  100 - all sensors agree
  101 - first sensor is defective
  102 - second sensor is defective
  103 - third sensor is defective
  104 - fourth sensor is defective

  */
  // if (sn->_s_second->temp + PERMITTED_TEMP_DIFFERENCE <= sn->_s_first->temp
  // ||
  //     sn->_s_third->temp + PERMITTED_TEMP_DIFFERENCE <= sn->_s_first->temp ||
  //     sn->_s_fourth->temp + PERMITTED_TEMP_DIFFERENCE <= sn->_s_first->temp)
  //     {
  //   return 101;
  // }
  // if (sn->_s_first->temp + PERMITTED_TEMP_DIFFERENCE <= sn->_s_second->temp
  // ||
  //     sn->_s_third->temp + PERMITTED_TEMP_DIFFERENCE <= sn->_s_second->temp
  //     || sn->_s_fourth->temp + PERMITTED_TEMP_DIFFERENCE <=
  //     sn->_s_second->temp) {
  //   return 102;
  // }
  // if (sn->_s_first->temp + PERMITTED_TEMP_DIFFERENCE <= sn->_s_third->temp ||
  //     sn->_s_second->temp + PERMITTED_TEMP_DIFFERENCE <= sn->_s_third->temp
  //     || sn->_s_fourth->temp + PERMITTED_TEMP_DIFFERENCE <=
  //     sn->_s_fourth->temp) {
  //   return 103;
  // }
  // if (sn->_s_first->temp + PERMITTED_TEMP_DIFFERENCE <= sn->_s_fourth->temp
  // ||
  //     sn->_s_second->temp + PERMITTED_TEMP_DIFFERENCE <= sn->_s_fourth->temp
  //     || sn->_s_third->temp + PERMITTED_TEMP_DIFFERENCE <=
  //     sn->_s_fourth->temp)
  //   return 104;
  return 0;  // no error
}

int checkTemperature(Sensors *sn) {
#ifdef ENABLE_VALIDATE
  float averageVal = getFAverageTemp(sn);

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



void pipeline(Sensors *sns) {
  if (!getReadySensors(sns) || errorCode > 0) return;

  int sensor_code = checkSensors(sns);
  int temperature_code = checkTemperature(sns);

  if (sensor_code > 0)
    errorCode = sensor_code;
  else if (temperature_code > 0)
    errorCode = temperature_code;
  else
    errorCode = 0;

  if (errorCode > 0) return haltSystem();
}

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
    burnerStatus = false;
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
