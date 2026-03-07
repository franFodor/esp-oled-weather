/**
 * WiFi.h
 *
 * Created on: Mar 7, 2026
 *     Author: Fran Fodor
 */

#ifndef HTTP_H_
#define HTPP_H_

#include "esp_err.h"
#include "esp_http_client.h"

#define WEATHER_URL "http://api.open-meteo.com/v1/forecast?latitude=45.5511&longitude=18.6939&hourly=temperature_2m,relative_humidity_2m,wind_speed_10m"

#define BUFFER_SIZE 8192

class Http
{
public:
  void               getWeather();
private:
  static esp_err_t   httpEventHandler(esp_http_client_event_t *evt);
  static void        parseJson(const char *json);

  char               m_responseBuffer[BUFFER_SIZE];
  int                m_responseLength;
};

#endif
