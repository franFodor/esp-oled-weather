/**
 * @file   http.cpp
 *
 * @brief  Functions for HTTP clients.
 *
 * @author Fran Fodor
 */

#include "esp_http_client.h"
#include "esp_log.h"
#include <cstdio>

#include "include/http.h"

static const char *TAG = "ESP_HTTP";

char Http::m_weatherUrl[URL_SIZE];
char Http::m_responseBuffer[BUFFER_SIZE];
int Http::m_responseLength = 0;

/**
 * @brief   Constructor for HTTP client to set up URL and GET method.
 *
 */
Http::Http()
{
  Coordinates coordinates = getCoordinates();

  snprintf(m_weatherUrl, sizeof(m_weatherUrl), WEATHER_URL, coordinates.latitude, coordinates.longitude);
  
  m_httpConfig = {};
  m_httpConfig.url = m_weatherUrl;
  m_httpConfig.method = HTTP_METHOD_GET;
  m_httpConfig.event_handler = Http::httpEventHandler;
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
      memcpy(m_responseBuffer + m_responseLength, evt->data ,evt->data_len);
      m_responseLength += evt->data_len;
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
 * @param   bool weather
 *          if you with to extract weather data set this to true as the json is slightly different
 * @returns float
 *          value of the given key 
 */
float Http::extractValue(const char *json, const char *key, bool weather)
{
  const char *current = json;
  if (weather)
  {
    // position the pointer on the "current" field after which keys and values are given
    current = strstr(json, "\"current\":");
  }

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
 * @brief   Extract weather info from JSON data.
 *
 * Wrapper for the extractValue function.
 *
 * @param   const char *json
 *          pointer to json array from the GET request
 * @param   WeatherData *weatherData
 *          pointer to the struct in which the results will be saved
 */
void Http::extractWeather(const char *json, WeatherData *weatherData)
{
  weatherData->temperature = Http::extractValue(json, "temperature_2m", true);
  weatherData->humidity = Http::extractValue(json, "relative_humidity_2m", true);
  weatherData->wind = Http::extractValue(json, "wind_speed_10m", true);
  weatherData->weatherCode = Http::extractValue(json, "weather_code", true);
  weatherData->valid = true;
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
  memset(m_responseBuffer, 0, sizeof(m_responseBuffer));
  WeatherData weatherData;
  weatherData.valid = false;

  esp_http_client_handle_t client = esp_http_client_init(&m_httpConfig);
  esp_err_t err = esp_http_client_perform(client);

  if (err == ESP_OK)
  {
    // ESP_LOGI(TAG, "response received %s", m_responseBuffer);

    extractWeather(m_responseBuffer, &weatherData);
  }
  else
  {
    weatherData.err = err;
    ESP_LOGE(TAG, "Request failed: %s", esp_err_to_name(err));
  }

  esp_http_client_cleanup(client);

  return weatherData;
}

/**
 * @brief   Extract location info from JSON data.
 *
 * Wrapper for the extractValue function.
 *
 * @param   const char *json
 *          pointer to json array from the GET request
 * @param   Coordinates *coordinates
 *          pointer to the struct in which the results will be saved
 */
void Http::extractCoordinates(const char *json, Coordinates *coordinates)
{
  coordinates->latitude = extractValue(json, "latitude", false);
  coordinates->longitude = extractValue(json, "longitude", false);
}

/**
 * @brief   Get location data from Open-Meteo API.
 *
 * @returns Coordinates
 *          struct containing location coordinates
 */
Coordinates Http::getCoordinates()
{
  ESP_LOGI(TAG, "Getting coordinates...");
  m_responseLength = 0;
  memset(m_responseBuffer, 0, sizeof(m_responseBuffer));
  char location[URL_SIZE];
  Coordinates coordinates;

  snprintf(location, sizeof(location), LOCATION_URL, CONFIG_WEATHER_LOCATION);

  esp_http_client_config_t config = {};
  config.url = location;
  config.method = HTTP_METHOD_GET;
  config.event_handler = Http::httpEventHandler;

  esp_http_client_handle_t client = esp_http_client_init(&config);
  esp_err_t err = esp_http_client_perform(client);

  if (err == ESP_OK)
  {
    // ESP_LOGI(TAG, "response received %s", m_responseBuffer);
    extractCoordinates(m_responseBuffer, &coordinates);
  }
  else
  {
    ESP_LOGE(TAG, "Request failed: %s", esp_err_to_name(err));
  }

  esp_http_client_cleanup(client);

  return coordinates;
}
