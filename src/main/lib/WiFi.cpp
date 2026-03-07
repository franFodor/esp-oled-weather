/**
 * WiFi.cpp
 *
 * Created on: Mar 7, 2026
 *     Author: Fran Fodor
 */

#include "../include/WiFi.h"
#include "../include/Http.h"
#include "esp_err.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include <cstring>

static const char *TAG = "ESP_WIFI";
 
/**
 * @brief Initialize WiFi.
 *
 */
void WiFi::init() {
  ESP_ERROR_CHECK(nvs_flash_init());
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  esp_event_handler_instance_t instance_any_id;
  esp_event_handler_instance_t instance_got_ip;
  ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                      ESP_EVENT_ANY_ID,
                                                      &eventHandler,
                                                      NULL,
                                                      &instance_any_id));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                      IP_EVENT_STA_GOT_IP,
                                                      &eventHandler,
                                                      NULL,
                                                      &instance_got_ip));

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
 * @brief Handle WiFi events such as starting, disconnecting, getting ip...
 *
 * Called automatically after registering handlers, function signature in accordance with ESP-IDF documentation.
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
void WiFi::eventHandler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        // TODO alert the user and try to reconnect, stop the pinging task
        esp_wifi_connect();
        ESP_LOGI(TAG, "Failed connect to the AP, retrying...");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        // TODO start a task to ping every hour
        Http Htpp;
        Htpp.getWeather();
    }
}
