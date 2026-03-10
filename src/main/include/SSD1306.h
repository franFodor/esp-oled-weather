/**
 * SSD1306.h
 *
 * Created on: Mar 9, 2026
 *     Author: Fran Fodor
 */

#ifndef SSD1306_H_
#define SSD1306_H_

#include "driver/i2c_master.h"
#include <stdint.h>

#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64
#define SSD1306_PAGES (SSD1306_HEIGHT / 8)

class SSD1306
{
public:
    SSD1306();
    void                    drawString(const char *str, uint8_t line);
    void                    clear();

private:
    void                    update();
    void                    sendCommand(uint8_t cmd);
    void                    drawChar(uint8_t pixelPointer, uint8_t line, char c);

    i2c_master_dev_handle_t m_dev;
    uint8_t                 m_buffer[SSD1306_WIDTH * SSD1306_PAGES];
};

#endif
