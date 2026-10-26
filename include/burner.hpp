#pragma once
#include "sensor.hpp"
#define YES_OR_NO(value) value ? "yes" : "no"

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

void prepare();

}  // namespace Burner

inline bool _vent_status = false;
inline bool _ign_trans = false;
inline bool _pil_val = false;
inline bool _bur_sens = false;

inline bool burner_checked = false;

// for gui
inline bool loop_vent_status = false;
inline bool loop_ign_trans = false;
inline bool loop_pil_val = false;
inline bool loop_bur_sens = false;