#pragma once
#include "sensor.hpp"

/**
 *     Vent 5 sec
 *  |----------------|
 *                    Ign.Transf
 *
 *                   |-------------|
 *
 * Ventilation duration - 5 sec
 * Ignition Transformer - first phase - 2 sec, second phase - 3 sec
 *
 *
 */

namespace Burner {

void burnerSystem();
bool checkTemp(Sensors *sn);
bool getBurnerStatus();
inline bool isBurnerReady = false;
// extern unsigned int _startMil;

/**
 * Only delay (for default 5 sec)
 */


// void ventilation();
// void ignitionTransformer();
// void pilotValve();
// void burnerSensor();
void prepare();

}  // namespace Burner
