/**
 * main.cpp
 *
 * Created on: Mar 6, 2026
 *     Author: Fran Fodor
 */
 
#include "Http.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

#include "include/WiFi.h"
#include "include/SSD1306.h"
#include <cstdio>

static const char *TAG = "ESP32-WEATHER";

void displayTask(void *pvParameters)
{
  WiFi wifi;
  SSD1306 display;
  WeatherData weatherData;
  char str[SSD1306_WIDTH];

  // wait for wifi to connect
  while (!wifi.m_gotIp)
  {
    vTaskDelay(pdMS_TO_TICKS(100));
  }

  while (1)
  {
    ESP_LOGI(TAG, "Refreshing data...");
    weatherData = wifi.getWeatherData();
    display.drawString("NASICE", 0);

    snprintf(str, sizeof(str), "TEMPERATURE: %.2f C", weatherData.temperature);
    display.drawString(str, 1);
    memset(str, 0, sizeof(str));

    snprintf(str, sizeof(str), "WIND SPEED: %.2f M/S", weatherData.wind);
    display.drawString(str, 2);
    memset(str, 0, sizeof(str));

    snprintf(str, sizeof(str), "HUMIDITY: %d%%", weatherData.humidity);
    display.drawString(str, 3);
    memset(str, 0, sizeof(str));

    // every minute, currently 10 sec
    vTaskDelay(pdMS_TO_TICKS(10000));
  }
}

extern "C" void app_main()
{
  ESP_LOGI(TAG, "Initializing...");

  // TODO handle
  xTaskCreate(displayTask, "Display task", 4096, NULL, 1, NULL);
}
