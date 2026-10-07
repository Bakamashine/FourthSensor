#include "validate.h"
#include "helper.h"
#include "sensor.h"
#include "settings.h"
#define PERMITTED_TEMP_DIFFERENCE 5

int errorCode = 0;

int
checkSensors (Sensors *sn)
{
  /*
  codes:
  100 - all sensors agree
  101 - main sensor is defective
  102 - first reserve sensor is defective
  103 - second reserve sensor is defective

  */
  if (sn->_s_first->temp + PERMITTED_TEMP_DIFFERENCE <= sn->_s_main->temp
      || sn->_s_second->temp + PERMITTED_TEMP_DIFFERENCE <= sn->_s_main->temp)
    {
      return 101;
    }
  if (sn->_s_main->temp + PERMITTED_TEMP_DIFFERENCE <= sn->_s_first->temp
      || sn->_s_second->temp + PERMITTED_TEMP_DIFFERENCE <= sn->_s_first->temp)
    {
      return 102;
    }
  if (sn->_s_main->temp + PERMITTED_TEMP_DIFFERENCE <= sn->_s_second->temp
      || sn->_s_first->temp + PERMITTED_TEMP_DIFFERENCE <= sn->_s_second->temp)
    {
      return 103;
    }

  return 0; // no error
}

int
checkTemperature (Sensors *sn)
{
  // street sensor excluded on purpose, see checkSensors()
  // float temp[]
  //     = { sn->_s_main->temp, sn->_s_first->temp, sn->_s_second->temp };
  // float averageVal
  //     = FloatGetAverageValue (temp, sizeof (temp) / sizeof (temp[0]));

  float averageVal = getFAverageTemp(sn);

  // the bounds themselves are valid readings, so compare strictly
  if (maxPermOffset < averageVal)
    {
      return 201; // average too hot
    }
  if (minPermOffset > averageVal)
    {
      return 202; // average too cold
    }

  return 0; // no error
}
void
validatePipeline (Sensors *sn)
{
  // until every sensor has finished its first ATTEMPTS cycle its temp is 0,
  // which would be reported as "too cold"
  if (!sn->_s_main->ready || !sn->_s_first->ready || !sn->_s_second->ready)
    return;

  int sensor_code = checkSensors (sn);
  int temperature_code = checkTemperature (sn);

  if (sensor_code > 0)
    errorCode = sensor_code;
  else if (temperature_code > 0)
    errorCode = temperature_code;
  else
    errorCode = 0;
}
