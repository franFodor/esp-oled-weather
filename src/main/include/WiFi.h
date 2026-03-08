/**
 * WiFi.h
 *
 * Created on: Mar 7, 2026
 *     Author: Fran Fodor
 */

#ifndef WIFI_H_
#define WIFI_H_

#include "esp_event.h"

// TODO maybe need to sync time

class WiFi
{
public:
  static void    init();
private:
  static void    ipEventHandler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data);
  static void    wifiEventHandler(void* arg, esp_event_base_t event_base,
                                  int32_t event_id, void* event_data);
};

#endif
