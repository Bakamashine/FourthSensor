#pragma once
#include "constants/constants.hpp"
#define DEFAULT_USER_TEMP 50
#define DEFAULT_HYSTERESIS 5
#define MIN_HYSTERESIS 1
#define MAX_HYSTERESIS 100

/// settings
#define ENABLE_COMMANDS
// #define SHOW_START_WINDOW
#define ENABLE_VALIDATE
#define ENABLE_BURNER
#define ENABLE_SETTINGS

// #define SENSOR_SETT

#define VIA_RESISTANCE 0
#define VIA_ACP 1
#define TEMP_CALCULATION_VARIANT VIA_ACP
// #define TEMP_CALCULATION_VARIANT VIA_RESISTANCE

#define _U8G2 0
#define GYVER 1
#define UI_BACK _U8G2

#if UI_BACK == _U8G2
#include <U8g2lib.h>
#define UICLASS CLS_U8G2
#elif UI_BACK == GYVER
#include <GyverOLED.h>
#define G_OLED_CLASS SSD1306_128x64
#define UICLASS GyverOLED<G_OLED_CLASS, OLED_NO_BUFFER>
#endif

#define BURNER_PREPARE

#define SQUIB_BTN
#define LED_BUILD_STATUS

#define MEDIAN_FILTER
#define EMA_FILTER

#define DS18_SENSOR

// debug
// #define DEBUG
// #define LED_DEBUG
// #define DEBUG_COMMAND