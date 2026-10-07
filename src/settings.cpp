#include "settings.h"
#include "sensor.h"
#include "validate.h"

int minPermOffset = 10;

int maxPermOffset = 50;
int hyst = DEFAULT_HYSTERESIS;
bool burnerStatus = false;
int uTemp = 40;

bool
checkTempForBurner (Sensors *sn)
{
  if (errorCode > 0)
    return false;
  int averageTemp = getIAverageTemp (sn);
  if (averageTemp + hyst >= uTemp)
    return false;
  if (averageTemp - hyst <= uTemp)
    return true;
  return false;
}