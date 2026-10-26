#pragma once
#include "constants/settings.hpp"
#include "sensor.hpp"  // SensorSettings

#define ADDR_HYST 0

#ifdef SENSOR_SETT
#define SENS_SETTINGS_SIZE sizeof(SensorSettings)
#else
#define SENS_SETTINGS_SIZE (sizeof(int) + sizeof(uint16_t))
#endif

#define ADDR_SENS1_SETTINGS (ADDR_HYST + sizeof(int))
#define ADDR_SENS2_SETTINGS (ADDR_SENS1_SETTINGS + SENS_SETTINGS_SIZE)
#define ADDR_SENS3_SETTINGS (ADDR_SENS2_SETTINGS + SENS_SETTINGS_SIZE)
#define ADDR_SENS4_SETTINGS (ADDR_SENS3_SETTINGS + SENS_SETTINGS_SIZE)
#define ADDR_MIN_PERM_OFFSET (ADDR_SENS4_SETTINGS + SENS_SETTINGS_SIZE)
#define ADDR_MAX_PERM_OFFSET (ADDR_MIN_PERM_OFFSET + sizeof(int))
#define ADDR_USER_TEMP (ADDR_MAX_PERM_OFFSET + sizeof(int))
#define ADDR_EEPROM_SIZE (ADDR_USER_TEMP + sizeof(int))
