//
// Created by paolo on 12/12/19.
//

#include "gDriverMethods.h"
#include "HttpClient.h"


constexpr const char * TAG_SEARCH_SERRA_FOLDER="searchForSerraFolder";
constexpr const char  SEARCH_FOR_FOLDER[]="https://www.googleapis.com/drive/v3/files?q=mimeType%20=%20'application/vnd.google-apps.folder'%20and%20name='";
constexpr uint16_t  SEARCH_FOR_FOLDER_LEN=sizeof(SEARCH_FOR_FOLDER)-1;

std::unique_ptr<char> searchForFolder(char * folderName) {
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

std::unique_ptr<char> createFolder(char *folderName) {
    HttpClient httpClient;

    Property mimeType("mimeType",0, "application/json",0);
    Property name("name",0, folderName,0);

    auto error = httpClient.postJson("https://www.googleapis.com/drive/v3/files", {mimeType,name}, {}, true);
    if (error != ESP_OK) {
        ESP_LOGE(TAG_SEARCH_SERRA_FOLDER, "Internal error getting user code");
        return std::unique_ptr<char>();
    } else {
        return httpClient.getBody();
    }
}
