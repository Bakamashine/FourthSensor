#include "settings.hpp"
#include "sensor.hpp"
#include "validate.hpp"

int minPermOffset = 10;

int maxPermOffset = 50;
float hyst = DEFAULT_HYSTERESIS;
bool burnerStatus = false;
int uTemp = 40;

bool
checkTempForBurner (Sensors *sn)
{
  if (errorCode > 0)
    return false;
  int averageTemp = getIAverageTemp (sn);
  // if (averageTemp + hyst >= uTemp)
  //   return false;
  if (averageTemp - hyst <= uTemp)
    return true;
  return false;
}