/**
 * @file   SSD1306.h
 *
 * @brief  Functions for interacting with SSD1306 OLED display.
 *
 * @author Fran Fodor
 */

#ifndef SSD1306_H_
#define SSD1306_H_

#include <stdint.h>

#include "include/I2C.h"

#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64
// lines on the display
#define SSD1306_PAGES (SSD1306_HEIGHT / 8)

/**
 * @brief Base class for SSD1306 functionality.
 */
class SSD1306
{
public:
  SSD1306();
  void    drawString(const char *str, uint8_t line);
  void    clear();
  bool    checkConnection();
  void    drawBitmap(uint8_t x, uint8_t y, const uint8_t *bitmap);

private:
  void    update();
  void    sendCommand(uint8_t cmd);
  void    drawChar(uint8_t pixelPointer, uint8_t line, char c);

  uint8_t m_buffer[SSD1306_WIDTH * SSD1306_PAGES];
  I2C     m_I2C;
};

#endif
