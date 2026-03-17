/**
 * @file   print_utils.h
 *
 * @brief  Functions printing text on either OLED display or serial monitor.
 *
 * @author Fran Fodor
 */

#ifndef PRINT_UTILS_H_
#define PRINT_UTILS_H_

#include <cstdint>

#include "include/SSD1306.h"

namespace util
{
  void print(const char *text, ...);
  void print(SSD1306& display, uint8_t line, textAlign align, const char *text, ...);
}

#endif
