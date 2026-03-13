#include "SSD1306.h"
#include <cstdint>

namespace util
{
  void print(const char *text, ...);
  void print(SSD1306& display, uint8_t line, const char *text, ...);
}
