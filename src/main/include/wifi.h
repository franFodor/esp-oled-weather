/**
 * @file   wifi.h
 *
 * @brief  Functions for establishing and maintaining WiFi connection.
 *
 * @author Fran Fodor
 */

#ifndef WIFI_H_
#define WIFI_H_

#include "esp_event.h"
#include "include/http.h"

/**
 * @brief Base class for WiFi functionality.
 */
class WiFi
{
public:
  WiFi();
  WeatherData    getWeatherData();
  static bool    connect();
  static bool    m_ready;

private:
  static void    ipEventHandler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data);
  static void    wifiEventHandler(void* arg, esp_event_base_t event_base,
                                  int32_t event_id, void* event_data);
  static void    setCurrentTime();
  static Http*   m_http;
  static bool    m_timeSet;
};

#endif
