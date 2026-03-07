#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "include/WiFi.h"

static const char *TAG = "ESP32-WEATHER";

extern "C" void app_main()
{
  ESP_LOGI(TAG, "Initilizing...");
  
  WiFi WiFi;
  WiFi.init();
}
