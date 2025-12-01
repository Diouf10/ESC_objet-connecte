#ifndef WIFI_H
#define WIFI_H

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"

extern EventGroupHandle_t wifi_event_group;

#define WIFI_CONNECTED_BIT BIT0

void wifi_init_sta(void);

#endif
