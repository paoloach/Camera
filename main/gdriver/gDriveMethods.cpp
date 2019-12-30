//
// Created by paolo on 12/12/19.
//

#include <cJSON.h>
#include "gDriveMethods.h"
#include "HttpClient.h"
#include "Constants.h"


constexpr const char *TAG_SEARCH_SERRA_FOLDER = "searchForSerraFolder";
constexpr const char SEARCH_FOR_FOLDER[] = "https://www.googleapis.com/drive/v3/files?q=mimeType%20=%20'application/vnd.google-apps.folder'%20and%20name='";
constexpr uint16_t SEARCH_FOR_FOLDER_LEN = sizeof(SEARCH_FOR_FOLDER) - 1;

struct JsonProperty {
    const char *key;
    const char *value;
};

static deleted_unique_ptr<char> createJSon(std::list<JsonProperty> &&headers);

std::unique_ptr<char> searchForFolder(const char *folderName) {
    HttpClient httpClient;
    uint16_t folderNameSize = strlen(folderName);
    uint16_t bufferSize = SEARCH_FOR_FOLDER_LEN + folderNameSize + 3;
    auto buffer = std::unique_ptr<char>(new char[bufferSize]);
    memcpy(buffer.get(), SEARCH_FOR_FOLDER, SEARCH_FOR_FOLDER_LEN);
    memcpy(buffer.get() + SEARCH_FOR_FOLDER_LEN, folderName, folderNameSize);
    memcpy(buffer.get() + SEARCH_FOR_FOLDER_LEN + folderNameSize, "'", 2);

    auto error = httpClient.get(buffer.get(), {}, true);
    if (error != ESP_OK) {
        ESP_LOGE(TAG_SEARCH_SERRA_FOLDER, "Internal error getting user code");
        return std::unique_ptr<char>();
    } else {
        return httpClient.getBody();
    }
}

std::unique_ptr<char> createFolder(const char *folderName) {
    HttpClient httpClient;

    JsonProperty mimeType{"mimeType", "application/vnd.google-apps.folder"};
    JsonProperty name{"name", folderName};
    auto body = createJSon({mimeType, name});

    auto error = httpClient.postJson("https://www.googleapis.com/drive/v3/files", body.get(), {}, true);
    if (error != ESP_OK) {
        ESP_LOGE(TAG_SEARCH_SERRA_FOLDER, "Internal error getting user code");
        return std::unique_ptr<char>();
    } else {
        return httpClient.getBody();
    }
}

std::unique_ptr<char> createFileMetadata(const char *folderNameId, const char *fileName) {
    HttpClient httpClient;

    auto json = deleted_unique_ptr<cJSON>(cJSON_CreateObject(), [](cJSON *p) { cJSON_Delete(p); });

    cJSON_AddStringToObject(json.get(), "mimeType", "image/jpeg");
    cJSON_AddStringToObject(json.get(), "name", fileName);
    auto parents = cJSON_AddArrayToObject(json.get(), "parents");
    cJSON_AddItemToArray(parents, cJSON_CreateString(folderNameId));

    auto body = deleted_unique_ptr<char>(cJSON_Print(json.get()), [](char *p) { free(p); });
    ESP_LOGI("createFileMetadata", "Body create file: %s", body.get());

    auto error = httpClient.postJson("https://www.googleapis.com/drive/v3/files", body.get(), {}, true);
    if (error != ESP_OK) {
        ESP_LOGE(TAG_SEARCH_SERRA_FOLDER, "Internal error getting user code");
        return std::unique_ptr<char>();
    } else {
        return httpClient.getBody();
    }
}

std::unique_ptr<char> uploadFileData(const char *fileID, uint8_t *data, size_t dataLen) {
    HttpClient httpClient;
    const char * TAG="uploadFileData";

    ESP_LOGI(TAG, "update %s with data size %d", fileID, dataLen);
    size_t idLen = strlen(fileID);

    size_t urlLen = UPDATE_FILE_URL_LEN + idLen + UPLOAD_TYPE_MEDIA_LEN+1;
    auto url = std::unique_ptr<char>(new char[urlLen]);
    strcpy(url.get(),UPDATE_FILE_URL );
    strcat(url.get(),fileID );
    strcat(url.get(),UPLOAD_TYPE_MEDIA );

    Header contentType{"Content-Type", "image/jpeg"};

    auto error = httpClient.patch(url.get(), data, dataLen,{contentType});
    ESP_LOGI(TAG, "result: %s", httpClient.getBody().get());
    return std::unique_ptr<char>();
}


deleted_unique_ptr<char> createJSon(std::list<JsonProperty> &&properties) {
    auto body = deleted_unique_ptr<cJSON>(cJSON_CreateObject(), [](cJSON *p) { cJSON_Delete(p); });


    for (auto &property : properties) {
        cJSON_AddStringToObject(body.get(), property.key, property.value);
    }

    char *postData = cJSON_Print(body.get());
    return deleted_unique_ptr<char>(postData, [](char *p) { free(p); });
}
