/**
 * @file   main.cpp
 *
 * @brief  Main program functinality using FreeRTOS tasks.
 *
 * @author Fran Fodor
 */

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include <cstdio>
#include "sdkconfig.h"
#include "time.h"

#include "print_utils.h"
#include "WiFi.h"
#include "SSD1306.h"
#include "font.h"

static const char *TAG = "ESP32-WEATHER";

TaskHandle_t displayHandle;
TaskHandle_t watchdogHandle;

// wrapper for shared classes between tasks
struct SharedData
{
  WiFi wifi;
  SSD1306 display;
};

void watchdogTask(void *pvParameters)
{
  // get data from task parameters
  SharedData &sharedData = *static_cast<SharedData *>(pvParameters);
  WiFi wifi = sharedData.wifi;
  SSD1306 display = sharedData.display;
  // since this task refreshes every 5 seconds, when counter reaches 30 minute has passed
  uint8_t minuteCounter = 0;

  while (1)
  {
    // get the notif only if its the first iteration
    if (minuteCounter == 0)
    {
      ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    }

    // check if WiFi is running
    bool displayed = false;
    while (!wifi.m_ready)
    {
      // if the error was already displayed, skip refreshing the screen constantly and just try to reconnect
      if (!displayed)
      {
        display.clear();
        util::print(display, 2, ALIGN_CENTER, "WiFi reconnecting...");
        displayed = true;
      }
      wifi.connect();
      vTaskDelay(pdMS_TO_TICKS(100));
    }

    // if a minute has passed or WiFi just reconnected update the weather information
    minuteCounter++;
    if (minuteCounter >= 30 || displayed == true)
    {
      xTaskNotifyGive(displayHandle);
      minuteCounter = 0;
      displayed = false;
    }

    // every 2 seconds
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

void displayTask(void *pvParameters)
{
  // cpp magic
  SharedData &sharedData = *static_cast<SharedData *>(pvParameters);
  WiFi wifi = sharedData.wifi;
  SSD1306 display = sharedData.display;

  WeatherData weatherData;

  util::print(display, 0, ALIGN_CENTER, "Connecting to");
  util::print(display, 1, ALIGN_CENTER, "WiFi...");

  // wait for wifi to connect
  while (!wifi.m_ready)
  {
    vTaskDelay(pdMS_TO_TICKS(100));
  }

  // local time
  time_t now;
  struct tm timeinfo;

  // convert location to uppercase no matter what user input
  char location[SSD1306_WIDTH];

  snprintf(location, strlen(CONFIG_WEATHER_LOCATION) + 1, "%s", CONFIG_WEATHER_LOCATION);
  for (int i = 0; location[i] != '\0'; i++)
  {
    location[i] = toupper(location[i]);
  }

  while (1)
  {
    display.clear();

    time(&now);
    localtime_r(&now, &timeinfo);
    ESP_LOGI(TAG, "Refreshing data...");
    ESP_LOGI(TAG, "Current time: %s", asctime(&timeinfo)); 

    weatherData = wifi.getWeatherData();

    if (weatherData.valid)
    {
      util::print(display, 0, ALIGN_CENTER, "%s", location);
      util::print(display, 2, ALIGN_LEFT, "Temperature: %.2f C", weatherData.temperature);
      util::print(display, 3, ALIGN_LEFT, "Wind speed: %.2f m/s", weatherData.wind);
      util::print(display, 4, ALIGN_LEFT, "Humidity: %d%%", weatherData.humidity);
      util::print(display, 7, ALIGN_LEFT, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);

      // https://open-meteo.com/en/docs
      const uint8_t *bitmap = nullptr;
      if (weatherData.weatherCode == 0)
      {
        bitmap = sun;
      }
      else if (weatherData.weatherCode < 50)
      {
        bitmap = cloud;
      }
      else if (weatherData.weatherCode < 70)
      {
        bitmap = rain;
      }
      else if (weatherData.weatherCode < 90)
      {
        bitmap = snow;
      }

      display.drawBitmap(SSD1306_WIDTH - 16, SSD1306_HEIGHT - 16, bitmap);
    }
    else
    {
      // HTTP error
      util::print(display, 2, ALIGN_CENTER, "HTTP ERROR");
      util::print(display, 3, ALIGN_CENTER, "%s", esp_err_to_name(weatherData.err));
      util::print(display, 4, ALIGN_CENTER, "Check WiFi or API");
    }

    xTaskNotifyGive(watchdogHandle);
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    // vTaskDelay(pdMS_TO_TICKS(60000));
  }
}

extern "C" void app_main()
{
  ESP_LOGI(TAG, "Initializing...");

  static WiFi wifi;
  static SSD1306 display;

  static SharedData sharedData =
  {
    .wifi = wifi,
    .display = display,
  };

  xTaskCreate(displayTask, "Display task", 4096, &sharedData, 1, &displayHandle);
  xTaskCreate(watchdogTask, "Watchdog task", 4096, &sharedData, 2, &watchdogHandle);
}
