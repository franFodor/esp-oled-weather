/**
 * Http.h
 *
 * Created on: Mar 7, 2026
 *     Author: Fran Fodor
 */

#ifndef HTTP_H_
#define HTTP_H_

#include "esp_err.h"
#include "esp_http_client.h"

#define WEATHER_URL "http://api.open-meteo.com/v1/forecast?latitude=45.4886&longitude=18.0878&current=temperature_2m,relative_humidity_2m,wind_speed_10m"
#define BUFFER_SIZE 1024

typedef struct {
  double temperature;
  double humidity;
  double wind;
} WeatherData;

class Http
{
public:
  WeatherData        getWeather();
private:
  static esp_err_t   httpEventHandler(esp_http_client_event_t *evt);
  static void        parseJson(const char *json, WeatherData *weatherData);
  static float       extractValue(const char *json, const char *key);

  static char        m_responseBuffer[BUFFER_SIZE];
  static int         m_responseLength;
  WeatherData        m_weatherData;
};

#endif
