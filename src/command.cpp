#include "command.hpp"

#ifdef ENABLE_COMMANDS

#include <Arduino.h>
#include <string.h>

#include "constants/addr_eeprom.hpp"
#include "constants/settings.hpp"
#include "macro/debugUi.hpp"
#include "settings.hpp"

#ifdef SENSOR_SETT
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
    writeSensSett((sensor_ptr)->settings, eeprom_addr);               \
    _DEBUG_SENSOR_CMD(debug_msg);                                     \
  }
#endif

#ifdef DEBUG_COMMAND
#define _DEBUG_SENSOR_CMD(msg) Serial.println(msg)
#else
#define _DEBUG_SENSOR_CMD(msg) \
  do {                         \
  } while (0)
#endif

const char *_firstSlice;
const char *_secondSlice;

void help() {
  Serial.println(F("ALL COMMANDS:"));
  Serial.println(F("help              - show this list"));
  Serial.println(F("hyst=<int>        - set burner hysteresis"));
#ifdef SENSOR_SETT
  Serial.println(F("esN=<int>         - set sensor N (1..4) ema_k"));
#endif
#ifdef ENABLE_VALIDATE
  Serial.println(F("max=<int>         - set max permitted offset"));
  Serial.println(F("min=<int>         - set min permitted offset"));
  Serial.println(F("ut=<int>          - set user temperature"));
#endif
  Serial.println(F("clear             - restore factory defaults"));
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
void runCmd(char *cmd, Sensors &sns) {
#ifdef DEBUG_COMMAND
  PRINT_DEBUG("Before cmd=", cmd);
  PRINT_DEBUG("First=", _firstSlice);
  PRINT_DEBUG("Second=", _secondSlice);
#endif
  // hyst=<int> - set the burner hysteresis deadband
  int second_slice_value = atoi(_secondSlice);
  if (strcmp(_firstSlice, "hyst") == 0) {
    hyst = second_slice_value;
    _EEPROM::writeHyst(hyst);
#ifdef DEBUG_COMMAND
    Serial.println("RUN    hyst applied");
#endif
  }
  /**
   * example command: es1=<int>, es2=<int>
   * First: e - ema
   * Second: s - setting
   * Third: <int> - number sensor
   */
#ifdef SENSOR_SETT
  SENSOR_SETTINGS_CMD("es1", sns._s_first, ema_k, ADDR_SENS1_SETTINGS,
                      "Run es1 applied")
  SENSOR_SETTINGS_CMD("es2", sns._s_second, ema_k, ADDR_SENS2_SETTINGS,
                      "RUN es2 applied")
  SENSOR_SETTINGS_CMD("es3", sns._s_third, ema_k, ADDR_SENS3_SETTINGS,
                      "RUN es3 applied")
  SENSOR_SETTINGS_CMD("es4", sns._s_fourth, ema_k, ADDR_SENS4_SETTINGS,
                      "RUN es4 applied")
#endif

#ifdef ENABLE_VALIDATE
  // max=<int> - set the maximum permitted offset
  else if (strcmp(_firstSlice, "max") == 0) {
    _EEPROM::writeMaxPermOffset(second_slice_value);
#ifdef DEBUG_COMMAND
    Serial.println("RUN    max applied");
#endif
  }
  // min=<int> - set the minimum permitted offset
  else if (strcmp(_firstSlice, "min") == 0) {
    _EEPROM::writeMinPermOffset(second_slice_value);
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
void readCommandAndImpl(char *cmd, Sensors &sensors) {
  if (strlen(cmd) == 0) return;
#ifdef DEBUG_COMMAND
  PRINT_DEBUG("Read command: ", cmd);
#endif
  if (strcmp(cmd, "help") == 0) {
    help();
  } else if (strcmp(cmd, "clear") == 0) {
    _EEPROM::writeDefaultValue(sensors);
#ifdef DEBUG_COMMAND
    Serial.println("RUN    clear applied");
#endif
  } else if (tryParse(cmd)) {
#ifdef DEBUG_COMMAND
    PRINT_DEBUG("parsed first=", _firstSlice);
    PRINT_DEBUG("parsed second=", _secondSlice);
#endif
    runCmd(cmd, sensors);
  } else
    Serial.println("Command not found");
}

void handleCommand() {
  if (Serial.available() > 0) {
    static char cmd[CMD_BUF_SIZE];
    size_t i = 0;
    while (Serial.available() > 0 && i < CMD_BUF_SIZE - 1) {
      char ch = static_cast<char>(Serial.read());
      if (ch == '\n' || ch == '\r') break;
      cmd[i++] = ch;
    }
    cmd[i] = '\0';
    readCommandAndImpl(cmd, Sens::all);
  }
}

#endif  // ENABLE_COMMANDS
