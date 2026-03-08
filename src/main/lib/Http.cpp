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

// TODO sort this out
char Http::m_responseBuffer[BUFFER_SIZE];
int Http::m_responseLength = 0;

/**
 * @brief   Constructor for HTTP client to set up URL and GET method.
 *
 */
Http::Http()
{
  esp_http_client_config_t http_config = {};
  http_config.url = WEATHER_URL;
  http_config.method = HTTP_METHOD_GET;
  http_config.event_handler = Http::httpEventHandler;

  m_client = esp_http_client_init(&http_config);
}

/**
 * @brief   Handler for HTTP events such as "on data recieved".
 *
 * @param   esp_http_client_event_t *evt
 *          event data
 * @returns esp_err_t
 *          ESP error code
 */
esp_err_t Http::httpEventHandler(esp_http_client_event_t *evt)
{
  switch(evt->event_id) {
  case HTTP_EVENT_ON_DATA:
    if (m_responseLength + evt->data_len < BUFFER_SIZE) {
      ::memcpy(m_responseBuffer + m_responseLength, evt->data ,evt->data_len);
      m_responseLength += evt->data_len;
    } else {
      return ESP_ERR_HTTP_RANGE_NOT_SATISFIABLE;
    }
    break;

  default:
    break;
  }
  return ESP_OK;
}

/**
 * @brief   Extract weather data from JSON.
 *
 * Since the JSON is quite simple, doesn't change nor does it require modifications there is no need to use
 * a JSON library like cJSON.
 *
 * @param   const char *json
 *          pointer to json array from the GET request
 * @param   const char *key
 *          pointer to value that wants to be extracted (eg. temperature_2m)
 * @returns float
 *          value of the given key 
 */
float Http::extractValue(const char *json, const char *key)
{
  // position the pointer on the "current" field after which keys and values are given
  const char *current = strstr(json, "\"current\":");

  // position the pointer on the key
  const char *pos = strstr(current, key);
  if (!pos) return 0;

  // position the pointer on the value
  pos = strchr(pos, ':');
  if (!pos) return 0;

  // return the value converted to float
  return atof(pos + 1);
}

/**
 * @brief   Parse JSON data.
 *
 * Wrapper for the extractValue function.
 *
 * @param   const char *json
 *          pointer to json array from the GET request
 * @param   WeatherData *weatherData
 *          pointer to the struct in which the results will be saved
 */
void Http::parseJson(const char *json, WeatherData *weatherData)
{
  weatherData->temperature = Http::extractValue(json, "temperature_2m");
  weatherData->humidity = Http::extractValue(json, "relative_humidity_2m");
  weatherData->wind = Http::extractValue(json, "wind_speed_10m");
  weatherData->weatherCode = Http::extractValue(json, "weather_code");

  // ESP_LOGI(TAG, "Temperature: %.2f C", weatherData.temperature);
  // ESP_LOGI(TAG, "Humidity: %.0f %%", weatherData.humidity);
  // ESP_LOGI(TAG, "Wind: %.2f km/h", weatherData.wind);
}

/**
 * @brief   Get weather data from Open-Meteo API.
 *
 * @returns WeatherData
 *          struct containing weather information
 */
WeatherData Http::getWeather()
{
  m_responseLength = 0;
  WeatherData weatherData;

  esp_err_t err = esp_http_client_perform(m_client);

  if (err == ESP_OK) {
    m_responseBuffer[m_responseLength] = 0;

    // ESP_LOGI(TAG, "response received %s", m_responseBuffer);

    Http::parseJson(m_responseBuffer, &weatherData);
  } else {
    ESP_LOGE(TAG, "Request failed: %s", esp_err_to_name(err));
  }

  return weatherData;
}
