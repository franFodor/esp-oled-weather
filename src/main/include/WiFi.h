/**
 * WiFi.h
 *
 * Created on: Mar 7, 2026
 *     Author: Fran Fodor
 */

#ifndef WIFI_H_
#define WIFI_H_

#include "Http.h"
#include "esp_event.h"

class WiFi
{
public:
  WiFi();
  WeatherData    getWeatherData();
  static bool    connect();
  static bool    m_gotIp;
  static bool    m_timeSet;

private:
  static void    ipEventHandler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data);
  static void    wifiEventHandler(void* arg, esp_event_base_t event_base,
                                  int32_t event_id, void* event_data);
  static void    setCurrentTime();
  Http           m_http;
};

#endif
