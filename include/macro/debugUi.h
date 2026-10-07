#ifndef DEBUG_UI_H
#define DEBUG_UI_H

#define PRINT_DEBUG(desc, value)                                              \
  Serial.print (F (desc));                                                    \
  Serial.println (value)

#endif