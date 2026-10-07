#include "command.h"
#include "constants/settings.h"
#include "macro/debugUi.h"
#include "settings.h"
#include <Arduino.h>
#include <string.h>

const char *_firstSlice;
const char *_secondSlice;

size_t
getCmdBufSize ()
{
  return CMD_BUF_SIZE;
}

void
help ()
{
  Serial.println (F ("ALL COMMANDS:"));
  Serial.println (F ("help              - show this list"));
  Serial.println (F ("hyst=<int>        - set burner hysteresis"));
  Serial.println (F ("sens1=<int>       - set main sensor correction"));
  Serial.println (
      F ("sens2=<int>       - set first reserve sensor correction"));
  Serial.println (
      F ("sens3=<int>       - set second reserve sensor correction"));
  Serial.println (F ("sens4=<int>       - set street sensor correction"));
  Serial.println (F ("max=<int>         - set max permitted offset"));
  Serial.println (F ("min=<int>         - set min permitted offset"));
  Serial.println (F ("ut=<int>          - set user temperature"));
  Serial.println (F ("<other>=<int>     - not a command; try 'help'"));
}
int
tryParse (char *cmd)
{
  char *eq = strchr (cmd, '=');
  if (eq == nullptr || eq == cmd)
    return 0;

  *eq = '\0';
  _firstSlice = cmd;
  _secondSlice = eq + 1;

  return 1;
}
void
runCmd (char *cmd, Sensors *sensors)
{

#ifdef DEBUG_COMMAND
  PRINT_DEBUG ("Before cmd=", cmd);
  PRINT_DEBUG ("First=", _firstSlice);
  PRINT_DEBUG ("Second=", _secondSlice);
#endif
  // hyst=<int> - set the burner hysteresis deadband
  int second_slice_value = atoi (_secondSlice);
  if (strcmp (_firstSlice, "hyst") == 0)
    {
      hyst = second_slice_value;
#ifdef DEBUG_COMMAND
      Serial.println ("RUN    hyst applied");
#endif
    }
  // sens1=<int> - set the correction added to the main sensor reading
  else if (strcmp (_firstSlice, "sens1") == 0)
    {
      sensors->_s_main->correctInt = second_slice_value;
#ifdef DEBUG_COMMAND
      Serial.println ("RUN    sens1 applied");
#endif
    }
  // sens2=<int> - set the correction for the first reserve sensor
  else if (strcmp (_firstSlice, "sens2") == 0)
    {
      sensors->_s_first->correctInt = second_slice_value;
#ifdef DEBUG_COMMAND
      Serial.println ("RUN    sens2 applied");
#endif
    }
  // sens3=<int> - set the correction for the second reserve sensor
  else if (strcmp (_firstSlice, "sens3") == 0)
    {
      sensors->_s_second->correctInt = second_slice_value;
#ifdef DEBUG_COMMAND
      Serial.println ("RUN    sens3 applied");
#endif
    }
  // sens4=<int> - set the correction for the street sensor
  else if (strcmp (_firstSlice, "sens4") == 0)
    {
      sensors->_s_street->correctInt = second_slice_value;
#ifdef DEBUG_COMMAND
      Serial.println ("RUN    sens4 applied");
#endif
    }
  // max=<int> - set the maximum permitted offset
  else if (strcmp (_firstSlice, "max") == 0)
    {
      maxPermOffset = second_slice_value;
#ifdef DEBUG_COMMAND
      Serial.println ("RUN    max applied");
#endif
    }
  // min=<int> - set the minimum permitted offset
  else if (strcmp (_firstSlice, "min") == 0)
    {
      minPermOffset = second_slice_value;
#ifdef DEBUG_COMMAND
      Serial.println ("RUN    min applied");
#endif
    }
  else if (strcmp (_firstSlice, "ut") == 0)
    {
      uTemp = second_slice_value;
#ifdef DEBUG_COMMAND
      Serial.println ("RUN uTemp applied");
#endif
    }
#ifdef DEBUG_COMMAND
  else
    Serial.println ("RUN    no branch matched");
#endif
}
void
readCommandAndImpl (char *cmd, Sensors *sensors)
{

  if (strlen (cmd) == 0)
    {
      free (cmd);
      return;
    }
#ifdef DEBUG_COMMAND
  PRINT_DEBUG ("Read command: ", cmd);
#endif
  if (strcmp (cmd, "help") == 0)
    {
      help ();
    }
  else if (tryParse (cmd))
    {
#ifdef DEBUG_COMMAND
      PRINT_DEBUG ("parsed first=", _firstSlice);
      PRINT_DEBUG ("parsed second=", _secondSlice);
#endif
      runCmd (cmd, sensors);
    }
  else
    Serial.println ("Command not found");

  free (cmd);
}
