//
// Created by paolo on 03/12/19.
//

#include "gDriverHttp.h"
#include "gdriver/Constants.h"
#include "gdriver/GDriverToken.h"
#include "gdriver/HttpClient.h"
#include "settings.h"
#include <cJSON.h>
#include "gdriver/delete_unique_ptr.h"
#include "gdriver/gDriverMethods.h"

static constexpr const char *TAG = "gDriverHttp";

inline deleted_unique_ptr<cJSON> createCJson(char * data) {
    return deleted_unique_ptr<cJSON>(cJSON_Parse(reinterpret_cast<const char *>(data)),
                              [](cJSON *cjson) {
                                  cJSON_Delete(cjson);
                              });
}

esp_err_t checkFolderExistHandler(httpd_req_t *req, char * folderName) {
    if (gDriverToken.tokenValid()) {
        auto body = searchForFolder(folderName);
        if (body) {
            ESP_LOGI(TAG, "%s", body.get());
            auto root = createCJson(body.get());
            cJSON *error = cJSON_GetObjectItem(root.get(), "error");
            if (error != nullptr){
                httpd_resp_sendstr(req, reinterpret_cast<char *>(error->valuestring));
            } else {
                cJSON *files = cJSON_GetObjectItem(root.get(), "files");
                if (files == nullptr){
                    httpd_resp_sendstr(req, "Unable to parse response \r\n");
                    return ESP_OK;
                }
                uint16_t  arraySize = cJSON_GetArraySize(files);
                if (arraySize == 0){
                    httpd_resp_sendstr(req, "Folder doesn't exist");
                } else {
                    httpd_resp_sendstr(req, "Folder exist");
                }
            }
            httpd_resp_sendstr(req, "\n\r");
        }

    } else {
        httpd_resp_sendstr(req, "token expired or invalid\n\r");
    }

    return ESP_OK;
}

esp_err_t createFolder(httpd_req_t *req, char * folderName) {
    if (gDriverToken.tokenValid()) {
        auto body = createFolder(folderName);
        if (body) {
            ESP_LOGI(TAG, "%s", body.get());
            auto root = createCJson(body.get());
            cJSON *error = cJSON_GetObjectItem(root.get(), "error");
            if (error != nullptr){
                httpd_resp_sendstr(req, reinterpret_cast<char *>(error->valuestring));
            } else {
                cJSON *files = cJSON_GetObjectItem(root.get(), "files");
                if (files == nullptr){
                    httpd_resp_sendstr(req, "Unable to parse response \r\n");
                    return ESP_OK;
                }
                uint16_t  arraySize = cJSON_GetArraySize(files);
                if (arraySize == 0){
                    httpd_resp_sendstr(req, "Folder doesn't exist");
                } else {
                    httpd_resp_sendstr(req, "Folder exist");
                }
            }
            httpd_resp_sendstr(req, "\n\r");
        }

    } else {
        httpd_resp_sendstr(req, "token expired or invalid\n\r");
    }

    return ESP_OK;
}


esp_err_t getUserCodeHandler(httpd_req_t *req) {
    HttpClient httpClient;

    if (clientIDLen() == 0) {
        ESP_LOGE(TAG, "Missing clientID");
        httpd_resp_sendstr(req, "Missing client ID");
        httpd_resp_set_status(req, "400 - BAD REQUEST");
    }

    Property propClientId(CLIENT_ID, CLIENT_ID_LEN, clientID(), clientIDLen());
    Property propScope(SCOPE, SCOPE_LEN, PROFILE, PROFILE_LEN);

    auto error =
            httpClient.post("https://accounts.google.com/o/oauth2/device/code",
                            {propClientId, propScope}, {});
    if (error != ESP_OK) {
        ESP_LOGE(TAG, "Error getting user code");
        httpd_resp_sendstr(req, "Unable to send get user request\n\r");
        httpd_resp_set_status(req, "500 INTERNAL ERROR");

    } else {
        auto body(httpClient.getBody());
        if (body) {

            ESP_LOGI(TAG, "%s", body.get());

            auto root = createCJson(body.get());
            if (root) {
                cJSON *error = cJSON_GetObjectItem(root.get(), "error");
                if (error != nullptr) {
                    httpd_resp_sendstr(req, reinterpret_cast<char *>(error->valuestring));
                } else {
                    cJSON *jsonDeviceCode = cJSON_GetObjectItem(root.get(), "device_code");
                    cJSON *jsonInterval = cJSON_GetObjectItem(root.get(), "interval");
                    cJSON *jsonExpireIn = cJSON_GetObjectItem(root.get(), "expires_in");
                    if (jsonDeviceCode != nullptr) {
                        char *deviceCode = jsonDeviceCode->valuestring;
                        setDeviceCode(deviceCode, strlen(deviceCode));
                    }
                    httpd_resp_sendstr(req, body.get());
                    httpd_resp_sendstr(req, "\n\r");
                    gDriverToken.waitForAuthorized(jsonInterval->valueint, jsonExpireIn->valueint);
                }
            } else {
                const char *error_ptr = cJSON_GetErrorPtr();
                if (error_ptr != nullptr) {
                    ESP_LOGE(TAG, "Error before: %s\n", error_ptr);
                }
                return ESP_ERR_NO_MEM;
            }
        } else {
            httpd_resp_sendstr(req, "Empty body\n");
        }
    }

    return ESP_OK;
}

void initGDriver() {
    gDriverToken.init();

}

