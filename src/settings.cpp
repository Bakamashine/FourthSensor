#include "settings.h"
#include "sensor.h"

int minPermOffset = 10;

int maxPermOffset = 50;
int hyst = DEFAULT_HYSTERESIS;
bool burnerStatus = false;
int uTemp = 40;

bool
checkTempForBurner (Sensors *sn)
{
  int averageTemp = getIAverageTemp (sn);
  if (averageTemp + hyst >= uTemp)
    return false;
  if (averageTemp - hyst <= uTemp)
    return true;
  return false;
}