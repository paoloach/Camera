//
// Created by paolo on 12/12/19.
//

#include <cJSON.h>
#include "gDriverMethods.h"
#include "HttpClient.h"


constexpr const char * TAG_SEARCH_SERRA_FOLDER="searchForSerraFolder";
constexpr const char  SEARCH_FOR_FOLDER[]="https://www.googleapis.com/drive/v3/files?q=mimeType%20=%20'application/vnd.google-apps.folder'%20and%20name='";
constexpr uint16_t  SEARCH_FOR_FOLDER_LEN=sizeof(SEARCH_FOR_FOLDER)-1;

struct JsonProperty {
    const char * key;
    const char * value;
};

static deleted_unique_ptr<char> createJSon(std::list<JsonProperty> &&headers);

std::unique_ptr<char> searchForFolder(const char * folderName) {
    HttpClient httpClient;
    uint16_t folderNameSize = strlen(folderName);
    uint16_t bufferSize= SEARCH_FOR_FOLDER_LEN+folderNameSize+3;
    auto buffer = std::unique_ptr<char>(new char[bufferSize]);
    memcpy(buffer.get(), SEARCH_FOR_FOLDER, SEARCH_FOR_FOLDER_LEN);
    memcpy(buffer.get()+SEARCH_FOR_FOLDER_LEN, folderName, folderNameSize);
    memcpy(buffer.get()+SEARCH_FOR_FOLDER_LEN+folderNameSize, "'",2);

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

    JsonProperty mimeType{"mimeType","application/vnd.google-apps.folder"};
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



deleted_unique_ptr<char>  createJSon(std::list<JsonProperty> &&properties) {
    cJSON *body = cJSON_CreateObject();

    for (auto &property : properties) {
        cJSON_AddStringToObject(body, property.key, property.value);
    }

    char *postData = cJSON_Print(body);
    return deleted_unique_ptr<char>(postData, [](char *p){free(p);});
}
