//
// Created by paolo on 31/12/19.
//

#ifndef ESP32_ACHDJIAN_CAMERA_GROUPSIGNALS_H
#define ESP32_ACHDJIAN_CAMERA_GROUPSIGNALS_H

#include <freertos/event_groups.h>

#define WIFI_CONNECTED_BIT  BIT0  // Event group bit is set high while connected
#define TOKEN_VALID         BIT1


extern EventGroupHandle_t event_group;

#endif //ESP32_ACHDJIAN_CAMERA_GROUPSIGNALS_H
