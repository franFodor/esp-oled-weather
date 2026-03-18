/**
 * @file   SSD1306.h
 *
 * @brief  Functions for interacting with SSD1306 OLED display.
 *
 * Example usage:
 *
 *   SSD1306 display;
 *
 *   // clear any leftovers
 *   display.clear();
 *   display.drawString("Hello", 2, ALGIN_CENTER);
 *   display.drawString("SCROLLING...", 3);
 *   display.startScrollHorizonal(SCROLL_LEFT, 3, 3);
 *   // need to manually call update to apply the changes
 *   display.update();
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

enum scrollDirection
{
    SCROLL_LEFT,
    SCROLL_RIGHT
};

enum textAlign
{
    ALIGN_LEFT,
    ALIGN_CENTER,
    ALIGN_RIGHT
};

/**
 * @brief Base class for SSD1306 functionality.
 */
class SSD1306
{
public:
  SSD1306();

  void    clear();
  void    update();
  bool    checkConnection();

  void    drawString(const char *str, uint8_t line, textAlign align = ALIGN_LEFT);
  void    drawBitmap(uint8_t x, uint8_t y, const uint8_t *bitmap);

  void    startScrollHorizontal(scrollDirection direction, uint8_t startPage, uint8_t endPage);
  void    stopScroll(bool updateScreen = false);

private:
  void    sendCommand(uint8_t cmd);
  void    drawChar(uint8_t pixelPointer, uint8_t line, char c);
  void    sendChunk(uint8_t page, uint8_t colStart, uint8_t colEnd);

  uint8_t m_buffer[SSD1306_WIDTH * SSD1306_PAGES];
  uint8_t m_prevBuffer[SSD1306_WIDTH * SSD1306_PAGES];
  // +1 for data byte (0x40)
  uint8_t m_transmitBuffer[SSD1306_WIDTH + 1];

  I2C     m_I2C;
};

#endif
