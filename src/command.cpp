#include "command.hpp"

#include <Arduino.h>
#include <string.h>

#include "constants/addr_eeprom.hpp"
#include "constants/settings.hpp"
#include "macro/debugUi.hpp"
#include "settings.hpp"

/**
 *
 * @param cmd_name   command name
 * @param sensor_ptr sensor pointer
 * @param field       field in the struct (e.g highCorInt)
 * @param eeprom_addr address for saving to EEPROM (e.g, ADDR_SENS1_SETTINGS)
 * @param debug_msg  debug message
 *
 * Expands to a complete `else if` branch, so the invocation must NOT be
 * terminated with a semicolon (a stray `;` would break the if/else chain).
 */
#define SENSOR_SETTINGS_CMD(cmd_name, sensor_ptr, field, eeprom_addr, \
                            debug_msg)                                \
  else if (strcmp(_firstSlice, cmd_name) == 0) {                      \
    (sensor_ptr)->settings.field = second_slice_value;                \
    writeSensSett(&(sensor_ptr)->settings, eeprom_addr);              \
    _DEBUG_SENSOR_CMD(debug_msg);                                     \
  }

#ifdef DEBUG_COMMAND
#define _DEBUG_SENSOR_CMD(msg) Serial.println(msg)
#else
#define _DEBUG_SENSOR_CMD(msg) \
  do {                         \
  } while (0)
#endif

const char *_firstSlice;
const char *_secondSlice;

size_t getCmdBufSize() { return CMD_BUF_SIZE; }

void help() {
  Serial.println(F("ALL COMMANDS:"));
  Serial.println(F("help              - show this list"));
  Serial.println(F("hyst=<float>      - set burner hysteresis"));
  Serial.println(F(""));
  Serial.println(F("per sensor N (1..4), e.g. hcs1=<int>"));
  Serial.println(F("hcsN=<int>        - high temperature correction"));
  Serial.println(F("ccsN=<int>        - cool temperature correction"));
  Serial.println(F("dcsN=<int>        - default temperature correction"));
  Serial.println(F("hbN=<int>         - high temperature border"));
  Serial.println(F("cbN=<int>         - cool temperature border"));
#ifdef ENABLE_VALIDATE
  Serial.println(F("max=<int>         - set max permitted offset"));
  Serial.println(F("min=<int>         - set min permitted offset"));
  // Serial.println(F("ut=<int>          - set user temperature"));
#endif
  Serial.println(F("<other>=<int>     - not a command; try 'help'"));
}
int tryParse(char *cmd) {
  char *eq = strchr(cmd, '=');
  if (eq == nullptr || eq == cmd) return 0;

  *eq = '\0';
  _firstSlice = cmd;
  _secondSlice = eq + 1;

  return 1;
}
void runCmd(char *cmd, Sensors *sensors) {
#ifdef DEBUG_COMMAND
  PRINT_DEBUG("Before cmd=", cmd);
  PRINT_DEBUG("First=", _firstSlice);
  PRINT_DEBUG("Second=", _secondSlice);
#endif
  // hyst=<float> - set the burner hysteresis deadband
  int second_slice_value = atoi(_secondSlice);
  if (strcmp(_firstSlice, "hyst") == 0) {
    hyst = atof(_secondSlice);
    writeHystToEEPROM(hyst, -1);
#ifdef DEBUG_COMMAND
    Serial.println("RUN    hyst applied");
#endif
  }

  /**
   * example command: hcs1=<int>, ccs=<int>
   * Params:
   * First: h = high, c - cool
   * Second: c - correct int, b - border
   * Third: s - sensor
   * Fourth: <int> - number sensor
   */

  SENSOR_SETTINGS_CMD("hcs1", sensors->_s_first, highCorInt,
                      ADDR_SENS1_SETTINGS, "RUN hcs applied")
  SENSOR_SETTINGS_CMD("ccs1", sensors->_s_first, coolCorInt,
                      ADDR_SENS1_SETTINGS, "RUN ccs applied")
  SENSOR_SETTINGS_CMD("hb1", sensors->_s_first, highBorder, ADDR_SENS1_SETTINGS,
                      "RUN bh1 applied")
  SENSOR_SETTINGS_CMD("cb1", sensors->_s_first, coolBorder, ADDR_SENS1_SETTINGS,
                      "RUN cb1 applied")
  SENSOR_SETTINGS_CMD("dcs1", sensors->_s_first, defaultCorInt,
                      ADDR_SENS1_SETTINGS, "RUN dcs1 applied")

  SENSOR_SETTINGS_CMD("hcs2", sensors->_s_second, highCorInt,
                      ADDR_SENS2_SETTINGS, "RUN hcs2 applied")
  SENSOR_SETTINGS_CMD("ccs2", sensors->_s_second, coolCorInt,
                      ADDR_SENS2_SETTINGS, "RUN ccs2 applied")
  SENSOR_SETTINGS_CMD("hb2", sensors->_s_second, highBorder,
                      ADDR_SENS2_SETTINGS, "RUN hb2 applied")
  SENSOR_SETTINGS_CMD("cb2", sensors->_s_second, coolBorder,
                      ADDR_SENS2_SETTINGS, "RUN cb2 applied")
  SENSOR_SETTINGS_CMD("dcs2", sensors->_s_second, defaultCorInt,
                      ADDR_SENS2_SETTINGS, "RUN dcs2 applied")

  SENSOR_SETTINGS_CMD("hcs3", sensors->_s_third, highCorInt,
                      ADDR_SENS3_SETTINGS, "RUN hcs3 applied")
  SENSOR_SETTINGS_CMD("ccs3", sensors->_s_third, coolCorInt,
                      ADDR_SENS3_SETTINGS, "RUN ccs3 applied")
  SENSOR_SETTINGS_CMD("hb3", sensors->_s_third, highBorder, ADDR_SENS3_SETTINGS,
                      "RUN hb3 applied")
  SENSOR_SETTINGS_CMD("cb3", sensors->_s_third, coolBorder, ADDR_SENS3_SETTINGS,
                      "RUN cb3 applied")
  SENSOR_SETTINGS_CMD("dcs3", sensors->_s_third, defaultCorInt,
                      ADDR_SENS3_SETTINGS, "RUN dcs3 applied")

  SENSOR_SETTINGS_CMD("hcs4", sensors->_s_fourth, highCorInt,
                      ADDR_SENS4_SETTINGS, "RUN hcs4 applied")
  SENSOR_SETTINGS_CMD("ccs4", sensors->_s_fourth, coolCorInt,
                      ADDR_SENS4_SETTINGS, "RUN ccs4 applied")
  SENSOR_SETTINGS_CMD("hb4", sensors->_s_fourth, highBorder,
                      ADDR_SENS4_SETTINGS, "RUN hb4 applied")
  SENSOR_SETTINGS_CMD("cb4", sensors->_s_fourth, coolBorder,
                      ADDR_SENS4_SETTINGS, "RUN cb4 applied")
  SENSOR_SETTINGS_CMD("dcs4", sensors->_s_fourth, defaultCorInt,
                      ADDR_SENS4_SETTINGS, "RUN dcs4 applied")

#ifdef ENABLE_VALIDATE
  // max=<int> - set the maximum permitted offset
  else if (strcmp(_firstSlice, "max") == 0) {
    maxPermOffset = second_slice_value;
#ifdef DEBUG_COMMAND
    Serial.println("RUN    max applied");
#endif
  }
  // min=<int> - set the minimum permitted offset
  else if (strcmp(_firstSlice, "min") == 0) {
    minPermOffset = second_slice_value;
#ifdef DEBUG_COMMAND
    Serial.println("RUN    min applied");
#endif
  }
  else if (strcmp(_firstSlice, "ut") == 0) {
    uTemp = second_slice_value;
#ifdef DEBUG_COMMAND
    Serial.println("RUN uTemp applied");
#endif
  }
#endif
#ifdef DEBUG_COMMAND
  else Serial.println("RUN    no branch matched");
#endif
}
void readCommandAndImpl(char *cmd, Sensors *sensors) {
  if (strlen(cmd) == 0) {
    free(cmd);
    return;
  }
#ifdef DEBUG_COMMAND
  PRINT_DEBUG("Read command: ", cmd);
#endif
  if (strcmp(cmd, "help") == 0) {
    help();
  } else if (tryParse(cmd)) {
#ifdef DEBUG_COMMAND
    PRINT_DEBUG("parsed first=", _firstSlice);
    PRINT_DEBUG("parsed second=", _secondSlice);
#endif
    runCmd(cmd, sensors);
  } else
    Serial.println("Command not found");

  free(cmd);
}

void handleCommand() {
  if (Serial.available() > 0) {
    char *c_str = static_cast<char *>(malloc(getCmdBufSize()));
    if (c_str == nullptr) return;
    Serial.readString().toCharArray(c_str, getCmdBufSize());
    readCommandAndImpl(c_str, &Sens::all);
  }
}
