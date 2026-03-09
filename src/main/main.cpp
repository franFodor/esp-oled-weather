/**
 * main.cpp
 *
 * Created on: Mar 6, 2026
 *     Author: Fran Fodor
 */
 
#include "esp_log.h"

#include "include/WiFi.h"
#include "include/I2C.h"

static const char *TAG = "ESP32-WEATHER";

extern "C" void app_main()
{
  ESP_LOGI(TAG, "Initializing...");

  // WiFi WiFi;

  I2C I2C;
  I2C.testDisplay();
}
