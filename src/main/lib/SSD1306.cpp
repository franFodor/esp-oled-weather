/**
 * SSD1306.cpp
 *
 * Created on: Mar 9, 2026
 *     Author: Fran Fodor
 */

#include "ssd1306.h"
#include "SSD1306.h"
#include "esp_err.h"
#include "esp_log.h"
#include <string.h>

#include "font.h"
#include "I2C.h"

static const char *TAG = "ESP_SSD1306";

// SSD1306 datasheet pg. 64 with some additions gathered from testing denoted by *
static const uint8_t initCmds[27] =
{
  0xA8, 0x3F,       // set MUX ratio
  0xD3, 0x00,       // set display offset
  0x2E,             // stop scrolling *
  0x20, 0x00,       // horizontal addressing mode *
  0x21, 0x00, 0xFF, // column start *
  0x22, 0x00, 0x07, // page start *
  0x40,             // set display start line
  0xA1,             // set segment re-map
  0xC8,             // set COM output scan directiom
  0xDA, 0x12,       // set COM pins hardware configuration, 0x12 for 128x64
  0x81, 0x7F,       // set contrast control
  0xA4,             // disable entire display
  0xA6,             // set normal display
  0xD5, 0xF0,       // set osc freq
  0x8D, 0x14,       // enable charge pump regulator
  0xAF              // display on
};

/**
 * @brief Initialize SSD1306 display according to the datasheet example.
 *
 */
SSD1306::SSD1306()
{
  I2C m_I2C;

  if (!m_I2C.checkConnection()) {
    ESP_LOGI(TAG, "SSD1306 connection error.");
  }
  else
  {
    for (auto cmd : initCmds)
      sendCommand(cmd);

    clear();
  }
  ESP_LOGI(TAG, "SSD1306 initilization finished!");
}

/**
 * @brief Sends the provided command via I2C.
 * 
 * @param uint8_t cmd
 *        SSD1306 command, see datasheet for reference
 */
void SSD1306::sendCommand(uint8_t cmd)
{
  // for commands D/C# pin needs to be LOW
  uint8_t data[2] = {0x00, cmd};

  ESP_ERROR_CHECK(m_I2C.transaction(data, 2));
}

/**
 * @brief Clears the screen (sets buffer to 0).
 *
 */
void SSD1306::clear()
{
  memset(m_buffer, 0x00, sizeof(m_buffer));
  update();
}

/**
 * @brief Updates the screen with contents from the buffer.
 *
 */
void SSD1306::update()
{
  if (!m_I2C.checkConnection())
    return;

  uint8_t buffer[sizeof(m_buffer) + 1];
  // for data D/C# pin needs to be HIGH
  buffer[0] = 0x40;
  memcpy(buffer + 1, m_buffer, sizeof(m_buffer));

  ESP_ERROR_CHECK(m_I2C.transaction(buffer, sizeof(buffer)));
}

/**
 * @brief   Checks I2C connection.
 *
 * @returns bool
 *          0 if connection is not available
 */
bool SSD1306::checkConnection()
{
  return m_I2C.checkConnection();
}

/**
 * @brief Fills the buffer with provided character.
 *
 * @param uint8_t pixelPointer
 *        position of the "cursor", location from which drawing starts
 * @param uint8_6 line
 *        line (page) in the display upon which the character will be drawn
 * @param char c
 *        character to draw on the screen
 */
void SSD1306::drawChar(uint8_t pixelPointer, uint8_t line, char c)
{
  // TODO better error recovery, give user message
  if(c < 32 || c > 127) return;

  uint16_t index = (c - 32);

  for(int i = 0; i < 5; i++)
  {
      m_buffer[line * SSD1306_WIDTH + pixelPointer + i] = font5x7[index][i];
  }

  // space between letters
  m_buffer[line * SSD1306_WIDTH + pixelPointer + 5] = 0x00;
}


/**
 * @brief Draws provided string to the screen.
 *
 * @param const char *str
 *        pointer to the string
 * @param uint8_t line
 *        line (page) in the display upon which the string will be drawn
 */
void SSD1306::drawString(const char *str, uint8_t line)
{
  uint8_t pixelPointer = 0;
  while(*str)
  {
    drawChar(pixelPointer, line, *str++);
    // move pointer to next spot
    pixelPointer += 6;

    // out of bounds
    if(pixelPointer > (SSD1306_WIDTH - 8))
      break;
  }

  update();
}

/**
 * @brief Draws bitmap to the screen at the given position.
 *
 * @param uint8_t x
 *        starting pixel (rows)
 * @param uint8_t y
 *        starting pixel (columns)
 * @param const uint8_t *bitmap
 *        pointer to the bitmap to draw
 */
void SSD1306::drawBitmap(uint8_t x, uint8_t y, const uint8_t *bitmap)
{
  if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT)
    return;

  // loop through bitmap rows (bitmap is 16x16)
  for (int row = 0; row < 16; row++)
  {
    // combine two bytes
    uint16_t rowData = (bitmap[row * 2] << 8) | bitmap[row * 2 + 1];

    // loop through columns
    for (int col = 0; col < 16; col++)
    {
      // check if that pixel should be turned on
      if (rowData & (1 << (15 - col)))
      {
        // calculate specific coordinates
        uint8_t px = x + col;
        uint8_t py = y + row;

        if (px >= SSD1306_WIDTH || py >= SSD1306_HEIGHT)
          continue;

        // convert y to page number
        uint8_t pageIndex = py / 8;
        uint8_t bit = py % 8;

        // set pixel in the buffer
        m_buffer[pageIndex * SSD1306_WIDTH + px] |= (1 << bit);
      }
    }
  }

  update();
}
