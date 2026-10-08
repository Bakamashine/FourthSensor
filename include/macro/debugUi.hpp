#pragma once

#define PRINT_DEBUG(desc, value)                                              \
  Serial.print (F (desc));                                                    \
  Serial.println (value)
