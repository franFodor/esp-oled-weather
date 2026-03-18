/**
 * @file   print_utils.cpp
 *
 * @brief  Functions printing text on either OLED display or serial monitor.
 *
 * @author Fran Fodor
 */

#include "esp_log.h"
#include <cstdarg>
#include <cstdio>

#include "include/print_utils.h"
#include "include/SSD1306.h"
#include "include/font.h"

static const char *TAG = "ESP32_PRINT";

/*
 * @brief   Prints provided text on the serial monitor.
 *
 * @param   const char *text
 *          text to be displayed on the serial monitor
 * @param   ...
 *          additional arguments to display
 */
void util::print(const char *text, ...)
{
  char buffer[SSD1306_WIDTH];
  va_list args;
  va_start(args, text);

  vsnprintf(buffer, sizeof(buffer), text, args);

  ESP_LOGI(TAG, "%s", buffer);
}

/*
 * @brief   Prints provided text on the OLED display.
 *
 * If the display is not connected, defaults to serial monitor.
 *
 * @param   const char *text
 *          text to be displayed on the OLED display
 * @param   ...
 *          additional arguments to display
 */
void util::print(SSD1306& display, uint8_t line, textAlign align, const char *text, ...)
{
  char buffer[SSD1306_WIDTH];
  va_list args;
  va_start(args, text);

  vsnprintf(buffer, sizeof(buffer), text, args);

  if (display.checkConnection())
  {
    display.drawString(buffer, line, align);
  }
  else
  {
    ESP_LOGW(TAG, "OLED error, fallback to serial monitor.");
    util::print(buffer);
  }
}

/*
 * @brief   Helper function to return bitmap from the provided code.
 *
 * @param   int code
 *          weather code
 * @returns const uint8_t *
 *          bitmap coorespoinding with provided weather code
 */
const uint8_t* util::getBitmap(int code) {
    if (code == 0) return sun;
    if (code < 50) return cloud;
    if (code < 70) return rain;
    return snow;
}
