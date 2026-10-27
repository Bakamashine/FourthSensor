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

namespace Squib {
bool getSquibStatus();
void run(bool st = true);
};  // namespace Squib

namespace Burner {

void standard_runBurn();
bool checkUTemp(Sensors &sn);
inline bool isBurnerReady = false;

void prepare();

}  // namespace Burner

inline bool _vent_status = false;
inline bool _ign_trans = false;
inline bool _pil_val = false;
inline bool _bur_sens = false;

// successful prepare
inline bool suc_prep = false;

// for gui
inline bool loop_vent_status = false;
inline bool loop_ign_trans = false;
inline bool loop_pil_val = false;
inline bool loop_bur_sens = false;