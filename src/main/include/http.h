/**
 * @file   http.h
 *
 * @brief  Functions for HTTP clients.
 *
 * @author Fran Fodor
 */

#ifndef HTTP_H_
#define HTTP_H_

#include "esp_err.h"
#include "esp_http_client.h"

#define LOCATION_URL "http://geocoding-api.open-meteo.com/v1/search?name=%s&count=1&language=en&format=json"
#define WEATHER_URL  "http://api.open-meteo.com/v1/forecast?latitude=%f&longitude=%f&current=temperature_2m,relative_humidity_2m,wind_speed_10m,weather_code"

#define BUFFER_SIZE 1024
#define URL_SIZE 256

typedef struct {
  float     temperature;
  int       humidity;
  float     wind;
  int       weatherCode; // for displaying icon on OLED :)
  bool      valid;
  esp_err_t err;         // if http error occured
} WeatherData;

typedef struct {
  float longitude;
  float latitude;
} Coordinates;

/**
 * @brief Base class for HTTP functionality.
 */
class Http
{
public:
  Http();
  WeatherData              getWeather();
private:
  static esp_err_t         httpEventHandler(esp_http_client_event_t *evt);
  static void              extractWeather(const char *json, WeatherData *weatherData);
  static float             extractValue(const char *json, const char *key, bool weather);
  static void              extractCoordinates(const char *json, Coordinates *coordinates);
  Coordinates              getCoordinates();

  static char              m_weatherUrl[URL_SIZE];
  static char              m_responseBuffer[BUFFER_SIZE];
  static int               m_responseLength;
  WeatherData              m_weatherData;
  esp_http_client_config_t m_httpConfig;
};

#endif
