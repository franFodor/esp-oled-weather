/**
 * main.cpp
 *
 * Created on: Mar 6, 2026
 *     Author: Fran Fodor
 */

#include "esp_log.h"
#include "freertos/FreeRTOS.h"

#include "WiFi.h"
#include "SSD1306.h"
#include "font.h"
#include <cstdio>
#include "time.h"

static const char *TAG = "ESP32-WEATHER";

void displayTask(void *pvParameters)
{
  WiFi wifi;
  SSD1306 display;
  WeatherData weatherData;
  char str[SSD1306_WIDTH];

  // wait for wifi to connect and set time
  while (!wifi.m_timeSet)
  {
    vTaskDelay(pdMS_TO_TICKS(100));
  }

  time_t now;
  struct tm timeinfo;

  time(&now);
  localtime_r(&now, &timeinfo);

  ESP_LOGI(TAG, "Current time: %s", asctime(&timeinfo)); 

  while (1)
  {
    time(&now);
    localtime_r(&now, &timeinfo);
    ESP_LOGI(TAG, "Refreshing data...");

    weatherData = wifi.getWeatherData();

    snprintf(str, sizeof(str), "NASICE          %d:%d", timeinfo.tm_hour, timeinfo.tm_min);
    display.drawString(str, 0);
    memset(str, 0, sizeof(str));

    snprintf(str, sizeof(str), "TEMPERATURE: %.2f C", weatherData.temperature);
    display.drawString(str, 2);
    memset(str, 0, sizeof(str));

    snprintf(str, sizeof(str), "WIND SPEED: %.2f M/S", weatherData.wind);
    display.drawString(str, 3);
    memset(str, 0, sizeof(str));

    snprintf(str, sizeof(str), "HUMIDITY: %d%%", weatherData.humidity);
    display.drawString(str, 4);
    memset(str, 0, sizeof(str));

    // https://open-meteo.com/en/docs
    if (weatherData.weatherCode == 0)
    {
      display.drawBitmap(SSD1306_WIDTH - 16 - 8, SSD1306_HEIGHT - 16 - 8, sun);
    }
    else if (weatherData.weatherCode > 0 && weatherData.weatherCode < 50)
    {
      display.drawBitmap(SSD1306_WIDTH - 16 - 8, SSD1306_HEIGHT - 16 - 8, cloud);
    }
    else if (weatherData.weatherCode > 50 && weatherData.weatherCode < 70)
    {
      display.drawBitmap(SSD1306_WIDTH - 16 - 8, SSD1306_HEIGHT - 16 - 8, rain);
    }
    else if (weatherData.weatherCode > 70 && weatherData.weatherCode < 90)
    {
      display.drawBitmap(SSD1306_WIDTH - 16 - 8, SSD1306_HEIGHT - 16 - 8, snow);
    }

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
