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

#define WEATHER_URL "http://api.open-meteo.com/v1/forecast?latitude=45.4886&longitude=18.0878&current=temperature_2m,relative_humidity_2m,wind_speed_10m,weather_code"
#define BUFFER_SIZE 1024

typedef struct {
  float     temperature;
  int       humidity;
  float     wind;
  int       weatherCode; // for displaying icon on OLED :)
  bool      valid;
  esp_err_t err;         // if http error occured
} WeatherData;

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
  static void              parseJson(const char *json, WeatherData *weatherData);
  static float             extractValue(const char *json, const char *key);

  static char              m_responseBuffer[BUFFER_SIZE];
  static int               m_responseLength;
  WeatherData              m_weatherData;
  esp_http_client_config_t m_httpConfig;
};

#endif
