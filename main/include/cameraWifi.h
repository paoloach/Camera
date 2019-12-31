#ifndef _APP_WIFI_H_
#define _APP_WIFI_H_

#include "freertos/event_groups.h"

#ifdef __cplusplus
extern "C" {
#endif


int wifi_connection_count();
int wifi_get_rssi();
float wifi_get_tx_power();

void app_wifi_startup();
void app_wifi_shutdown();


#ifdef __cplusplus
}
#endif

#endif