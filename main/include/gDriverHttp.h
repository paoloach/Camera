//
// Created by paolo on 03/12/19.
//

#ifndef ESP32_ACHDJIAN_CAMERA_GDRIVERHTTP_H
#define ESP32_ACHDJIAN_CAMERA_GDRIVERHTTP_H

#include "esp_http_server.h"

#ifdef __cplusplus
extern "C" {
#endif
void initGDriver();
esp_err_t getUserCodeHandler(httpd_req_t *req);
void saveImage(const char * folderName, uint8_t * data, size_t size );

#ifdef __cplusplus
}
#endif


#endif // ESP32_ACHDJIAN_CAMERA_GDRIVERHTTP_H
