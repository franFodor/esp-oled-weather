#include "print_utils.h"
#include "SSD1306.h"
#include "esp_log.h"
#include <cstdarg>
#include <cstdio>

static const char *TAG = "ESP32_PRINT";

void util::print(const char *text, ...)
{
  char buffer[SSD1306_WIDTH];
  va_list args;
  va_start(args, text);

  vsnprintf(buffer, sizeof(buffer), text, args);

  ESP_LOGI(TAG, "%s", buffer);
}

void util::print(SSD1306& display, uint8_t line, const char *text, ...)
{
  char buffer[SSD1306_WIDTH];
  va_list args;
  va_start(args, text);

  vsnprintf(buffer, sizeof(buffer), text, args);

  if (display.checkConnection())
  {
    display.drawString(buffer, line);
  }
  else
  {
    ESP_LOGW(TAG, "OLED error, fallback to serial monitor.");
    util::print(buffer);
  }
}
