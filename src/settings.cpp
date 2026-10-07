#include "settings.h"
#include "sensor.h"

int minPermOffset = 10;

int maxPermOffset = 50;
int hyst = DEFAULT_HYSTERESIS;
bool burnerStatus = false;
int userTemperature = 40;

bool
checkBurner (Sensors*)
{
  
}