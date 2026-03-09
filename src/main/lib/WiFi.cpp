/**
 * WiFi.cpp
 *
 * Created on: Mar 7, 2026
 *     Author: Fran Fodor
 */

#include "esp_err.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "sdkconfig.h"

#include "../include/WiFi.h"
#include "../include/Http.h"

static const char *TAG = "ESP_WIFI";

/**
 * @brief Initialize WiFi in STA mode with credentials from menuconfig.
 *
 */
WiFi::WiFi() {
  ESP_ERROR_CHECK(nvs_flash_init());
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifiEventHandler, NULL, NULL));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &ipEventHandler, NULL, NULL));

  if (strlen(CONFIG_WIFI_SSID) == 0) {
    ESP_LOGE(TAG, "WiFi SSID not set. Run menuconfig.");
    return;
  }
  
  wifi_config_t wifi_config = {};
  // set parameters from menuconfig
  ::memcpy(wifi_config.sta.ssid, CONFIG_WIFI_SSID, sizeof(CONFIG_WIFI_SSID));
  ::memcpy(wifi_config.sta.password, CONFIG_WIFI_PASSWORD, sizeof(CONFIG_WIFI_PASSWORD));

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  ESP_LOGI(TAG, "WiFi initilization finished!");
}

/**
 * @brief Handle IP events.
 *
 * Called automatically after registering handlers upon event, function prototype in
 * accordance with ESP-IDF documentation.
 *
 * @param void* arg
 *        pointer to a user defined argument
 * @param esp_event_base_t event_base
 *        base of the event (WiFi event, IP event...)
 * @param int32_t event_id
 *        identifier of the event
 * @param void* event_data
 *        pointer to event specific data
 */
void WiFi::ipEventHandler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_id == IP_EVENT_STA_GOT_IP) {
        // TODO start a task to ping every minute
        Http Htpp;
        WeatherData weatherData;
        weatherData = Htpp.getWeather();

        ESP_LOGI(TAG, "Temperature: %.2f C", weatherData.temperature);
        ESP_LOGI(TAG, "Humidity: %.0f %%", weatherData.humidity);
        ESP_LOGI(TAG, "Wind: %.2f km/h", weatherData.wind);
        ESP_LOGI(TAG, "Weather code: %d", weatherData.weatherCode);
    }
}

/**
 * @brief Handle WiFi events such as starting, disconnecting.
 *
 * Called automatically after registering handlers upon event, function prototype in
 * accordance with ESP-IDF documentation.
 *
 * @param void* arg
 *        pointer to a user defined argument
 * @param esp_event_base_t event_base
 *        base of the event (WiFi event, IP event...)
 * @param int32_t event_id
*         identifier of the event
 * @param void* event_data
 *        pointer to event specific data
 */
void WiFi::wifiEventHandler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_STA_START) {
        ESP_ERROR_CHECK(esp_wifi_connect());
    } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
        // TODO alert the user and try to reconnect, stop the pinging task
        ESP_ERROR_CHECK(esp_wifi_connect());
        ESP_LOGI(TAG, "Failed connect to the AP, retrying...");
    }
}
