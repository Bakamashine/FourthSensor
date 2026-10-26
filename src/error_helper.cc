#include "error_helper.hpp"

#include <avr/pgmspace.h>

static const char* messageFor(int error_code) {
  switch (error_code) {
    case 100:
      return PSTR("All sensors agree");
    case 101:
      return PSTR("Sensor 1 defective");
    case 102:
      return PSTR("Sensor 2 defective");
    case 103:
      return PSTR("Sensor 3 defective");
    case 104:
      return PSTR("Sensor 4 defective");
    case 201:
      return PSTR("Temperature too hot");
    case 202:
      return PSTR("Temperature too cold");
    case 301:
      return PSTR("Burner did not ignite");
    case 302:
      return PSTR("Flame before ignition");
    default:
      return PSTR("Unknown error");
  }
}

// Copies a flash string into a RAM buffer, always NUL-terminating it.
static void copyFromFlash(char* buf, size_t buf_s, const char* src) {
  if (buf_s == 0) return;
  size_t i = 0;
  while (i + 1 < buf_s && pgm_read_byte(src + i) != '\0') {
    buf[i] = static_cast<char>(pgm_read_byte(src + i));
    ++i;
  }
  buf[i] = '\0';
}

void errorMessageToBuf(int error_code, char* buf, size_t buf_s) {
  copyFromFlash(buf, buf_s, messageFor(error_code));
}
