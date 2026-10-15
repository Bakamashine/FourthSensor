#pragma once

#define DEFAULT_USER_TEMP 50
#define DEFAULT_HYSTERESIS 0.07F

/// settings
// #define ENABLE_COMMANDS
// #define SHOW_START_WINDOW
// #define ENABLE_VALIDATE
// #define ENABLE_BURNER
// #define ENABLE_SETTINGS

#define VIA_RESISTANCE 0
#define VIA_ACP 1
#define TEMP_CALCULATION_VARIANT VIA_ACP

#define MEDIAN_FILTER
#define EMA_FILTER

#define DS18_SENSOR

// debug
#define DEBUG
// #define LED_DEBUG
// #define DEBUG_COMMAND