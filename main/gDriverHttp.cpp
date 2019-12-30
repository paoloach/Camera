//
// Created by paolo on 03/12/19.
//

#include <memory>
#include "gDriverHttp.h"
#include "gdriver/Constants.h"
#include "gdriver/GDriverToken.h"
#include "gdriver/HttpClient.h"
#include "settings.h"
#include <cJSON.h>
#include "gdriver/delete_unique_ptr.h"
#include "gdriver/gDriverMethods.h"

static constexpr const char *TAG = "gDriverHttp";


static std::unique_ptr<char> createFolderAndGetID(const char *folderName);

static std::unique_ptr<char> getFolderParentId(const char *folderParent);

static std::unique_ptr<char> getFileId(const char *metadata);

static std::unique_ptr<char> cloneString(cJSON *obj);

inline deleted_unique_ptr<cJSON> createCJson(const char *data) {
    return deleted_unique_ptr<cJSON>(cJSON_Parse(data),
                                     [](cJSON *cjson) {
                                         cJSON_Delete(cjson);
                                     });
}

void saveImage(const char *folderName, uint8_t *data, size_t size) {
    char fileName[64];
    struct tm timeinfo = {0};
    time_t now = time(nullptr);
    localtime_r(&now, &timeinfo);
    strftime(fileName, sizeof(fileName), "%F-%T.jpeg", &timeinfo);
    ESP_LOGI(TAG, "Filename: %s", fileName);
    auto parentId = getFolderParentId(folderName);
    if (!parentId) {
        return;
    }
    auto fileMetadata = createFileMetadata(parentId.get(), fileName);
    ESP_LOGI(TAG, "file metadata: %s", fileMetadata.get());
    auto fileId = getFileId(fileMetadata.get());
    uploadFileData(fileId.get(), data, size);
}

std::unique_ptr<char> getFolderParentId(const char *folderParent) {

    if (!gDriverToken.tokenValid()) {
        ESP_LOGE(TAG, "Token no more valid");
        return std::unique_ptr<char>();
    }
    auto body = searchForFolder(folderParent);
    if (!body) {
        return std::unique_ptr<char>();
    }
    auto root = createCJson(body.get());
    cJSON *error = cJSON_GetObjectItem(root.get(), "error");
    if (error != nullptr) {
        ESP_LOGE(TAG, "Error checking if %s exist: %s", folderParent, error->valuestring);
        return std::unique_ptr<char>();
    }
    auto files = cJSON_GetObjectItem(root.get(), "files");
    if (files != nullptr && cJSON_IsArray(files)) {
        uint16_t arraySize = cJSON_GetArraySize(files);
        if (arraySize == 0) {
            return createFolderAndGetID(folderParent);
        } else {
            auto firstElement = cJSON_GetArrayItem(files, 0);
            if (firstElement == nullptr) {
                ESP_LOGE(TAG, "Unable to get information on %s", folderParent);
                return createFolderAndGetID(folderParent);
            }
            auto jsonId = cJSON_GetObjectItem(firstElement, "id");
            return cloneString(jsonId);
        }
    } else {
        ESP_LOGE(TAG, "Error checking if %s exist: invalid list of files", folderParent);
        return std::unique_ptr<char>();
    }
}

std::unique_ptr<char> createFolderAndGetID(const char *folderName) {

    auto response = createFolder(folderName);
    if (!response) {
        ESP_LOGE(TAG, "Unable to create %s", folderName);
        return std::unique_ptr<char>();
    }
    auto root = createCJson(response.get());
    if (!root) {
        ESP_LOGE(TAG, "Unable to parse creation result of folder  %s", folderName);
        return std::unique_ptr<char>();
    }
    cJSON *error = cJSON_GetObjectItem(root.get(), "error");
    if (error != nullptr) {
        ESP_LOGE(TAG, "Error creating %s: %s", folderName, error->valuestring);
        return std::unique_ptr<char>();
    }
    cJSON *jsonId = cJSON_GetObjectItem(root.get(), "id");
    if (jsonId == nullptr) {
        ESP_LOGE(TAG, "Unable to get the folder id for  %s", folderName);
        return std::unique_ptr<char>();
    }
    return cloneString(jsonId);
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
                cJSON *jsonError = cJSON_GetObjectItem(root.get(), "error");
                if (jsonError != nullptr) {
                    httpd_resp_sendstr(req, reinterpret_cast<char *>(jsonError->valuestring));
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

std::unique_ptr<char> cloneString(cJSON *obj) {
    if (cJSON_IsString(obj) || cJSON_IsRaw(obj)) {
        size_t idLen = strlen(obj->valuestring);
        auto id = std::unique_ptr<char>(new char[idLen + 1]);
        memcpy(id.get(), obj->valuestring, idLen+1);
        return id;
    } else {
        return std::unique_ptr<char>();
    }
}

std::unique_ptr<char> getFileId(const char *metadata) {

    auto root = createCJson(metadata);
    if (!root) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != nullptr) {
            ESP_LOGE(TAG, "Error before: %s\n", error_ptr);
        } else {
            ESP_LOGE(TAG, "Unable to parse file metadata");
        }
        return std::unique_ptr<char>();
    }
    cJSON *jsonError = cJSON_GetObjectItem(root.get(), "error");
    if (jsonError != nullptr) {
        ESP_LOGE(TAG, "error creating file: %s ", jsonError->valuestring);
        return std::unique_ptr<char>();
    }
    cJSON *jsonId = cJSON_GetObjectItem(root.get(), "id");
    if (jsonId == nullptr || !cJSON_IsString(jsonId)){
        ESP_LOGE(TAG,"Unable to get the file id");
        return std::unique_ptr<char>();
    }
    return cloneString(jsonId);
}

