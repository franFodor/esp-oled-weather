/**
 * WiFi.h
 *
 * Created on: Mar 7, 2026
 *     Author: Fran Fodor
 */
#ifndef WIFI_H_
#define WIFI_H_

#include "sdkconfig.h"
#include "esp_event.h"

class WiFi {
public:
  // WiFi();
  // ~WiFi();
  static void    init();
private:
  static void    eventHandler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data);
};

#endif
