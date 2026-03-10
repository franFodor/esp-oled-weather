/**
 * main.cpp
 *
 * Created on: Mar 6, 2026
 *     Author: Fran Fodor
 */
 
#include "esp_log.h"

#include "include/WiFi.h"
#include "include/SSD1306.h"

static const char *TAG = "ESP32-WEATHER";

extern "C" void app_main()
{
  ESP_LOGI(TAG, "Initializing...");

  // WiFi WiFi;

  SSD1306 display;

  display.drawString("CLOUD", 0);
  display.drawString("ESP32", 4);
  display.drawString("TESTING", 1);
}
