/**
 * Http.cpp
 *
 * Created on: Mar 7, 2026
 *     Author: Fran Fodor
 */

#include "esp_http_client.h"
#include "esp_log.h"

#include "../include/Http.h"

static const char *TAG = "ESP_HTTP";

esp_err_t Http::httpEventHandler(esp_http_client_event_t *evt)
{
  switch(evt->event_id) {
  case HTTP_EVENT_ON_DATA:
    printf("%.*s", evt->data_len, (char*)evt->data);
    break;
  default:
  }
  return ESP_OK;
}

void Http::getWeather()
{
  esp_http_client_config_t config = {
      .url = WEATHER_URL,
      .method = HTTP_METHOD_GET,
      .event_handler = Http::httpEventHandler,
  };

  esp_http_client_handle_t client = esp_http_client_init(&config);

  esp_err_t err = esp_http_client_perform(client);

  if (err == ESP_OK) {
    ESP_LOGI(TAG, "HTTPS Status = %d",
             esp_http_client_get_status_code(client));
    ESP_LOGI(TAG, "Content length = %lld",
             esp_http_client_get_content_length(client));
  } else {
    ESP_LOGE(TAG, "Request failed: %s", esp_err_to_name(err));
  }

  esp_http_client_cleanup(client);
}
