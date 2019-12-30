//
// Created by paolo on 04/12/19.
//

#include <functional>
#include "HttpClient.h"
#include "Constants.h"
#include <cstring>
#include <esp_log.h>
#include <settings.h>
#include <cJSON.h>


template<typename T>
using deleted_unique_ptr = std::unique_ptr<T, std::function<void(T *)>>;

inline deleted_unique_ptr<cJSON> createCJson(char *data) {
    return deleted_unique_ptr<cJSON>(cJSON_Parse(reinterpret_cast<const char *>(data)),
                                     [](cJSON *cjson) {
                                         cJSON_Delete(cjson);
                                     });
}

HttpClient::HttpClient() {
    esp_http_client_config_t config = {
            .url="https://accounts.google.com",
            .auth_type = HTTP_AUTH_TYPE_NONE,
            .event_handler = eventHandler,
            .user_data = this};

    client = createClient(&config);
}


esp_err_t HttpClient::post(const char *url, std::list<Property> &&properties,
                           std::list<Property> &&headers) {
    uint16_t len = 0;
    esp_err_t error;

    ESP_LOGI(TAG, "POST at %s", url);

    data.clear();

    for (auto &property : properties) {
        len += property.keyLen + property.valueLen + 2;
    }

    auto *postData = new uint8_t[len];

    uint8_t *dataIter = postData;

    for (auto &property : properties) {
        memcpy(dataIter, property.key, property.keyLen);
        dataIter += property.keyLen;
        memcpy(dataIter, "=", 1);
        dataIter++;
        memcpy(dataIter, property.value, property.valueLen);
        dataIter += property.valueLen;
        *dataIter = '&';
        dataIter++;
    }
    dataIter--;
    *dataIter = 0;


    esp_http_client_set_url(client.get(), url);
    setMethod(HTTP_METHOD_POST);

    for (auto &header : headers) {
        error = setHeader(header.key, header.value);
        if (error != ESP_OK) {
            ESP_LOGE(TAG, "Error setting header: 0x%x", error);
        }
    }

    ESP_LOGI(TAG, "body sent: %s", postData);
    error = esp_http_client_set_post_field(client.get(), (const char *) postData, len);
    if (error != ESP_OK) {
        ESP_LOGE(TAG, "Error setting post field: %d", error);
    }
    error = esp_http_client_perform(client.get());
    if (error != ESP_OK) {
        ESP_LOGI(TAG, "Error perform post request field: %d", error);
    }
    return ESP_OK;
}

esp_err_t HttpClient::post(const char *url, const char * body, std::list<Header> &&headers) {
    esp_err_t error;

    ESP_LOGI(TAG, "POST at %s", url);

    data.clear();

    esp_http_client_set_url(client.get(), url);
    setMethod(HTTP_METHOD_POST);

    size_t bodyLength=strlen(body);
    addAuthorization();
    addContentLength(bodyLength);
    for (auto &header : headers) {
        error = setHeader(header.key, header.value);
        if( error != ESP_OK) {
            ESP_LOGE(TAG, "Error setting header: 0x%x", error);
            return error;
        }
    }
    error = esp_http_client_set_post_field(client.get(), body, bodyLength);
    if (error != ESP_OK){
        ESP_LOGE(TAG,"Error setting post field: 0x%x", error);
        return error;
    }


    error = esp_http_client_perform(client.get());
    if (error != ESP_OK) {
        ESP_LOGI(TAG, "Error perform post request field: 0x%x", error);
        return error;
    }
    return ESP_OK;
}

esp_err_t HttpClient::patch(const char *url,const uint8_t * body,  size_t bodySize, std::list<Header> &&headers) {
    esp_err_t error;

    ESP_LOGI(TAG, "PATCH at %s", url);

    data.clear();

    esp_http_client_set_url(client.get(), url);
    setMethod(HTTP_METHOD_PATCH);

    addAuthorization();
    addContentLength(bodySize);
    for (auto &header : headers) {
        error = setHeader(header.key, header.value);
        if( error != ESP_OK) {
            ESP_LOGE(TAG, "Error setting header: 0x%x", error);
            return error;
        }
    }
    error = esp_http_client_set_post_field(client.get(), reinterpret_cast<const char *>(body), bodySize);
    if (error != ESP_OK){
        ESP_LOGE(TAG,"Error setting post field: 0x%x", error);
        return error;
    }


    error = esp_http_client_perform(client.get());
    if (error != ESP_OK) {
        ESP_LOGI(TAG, "Error perform post request field: 0x%x", error);
        return error;
    }
    return ESP_OK;
}

esp_err_t HttpClient::postJson(const char *url, const char *body, std::list<Property> &&headers, bool includeAuth) {
    esp_err_t error;

    ESP_LOGI(TAG, "POST at %s", url);

    data.clear();

    setUrl(url);
    setMethod(HTTP_METHOD_POST);

    for (auto &header : headers) {
        error = setHeader(header.key, header.value);
        if (error != ESP_OK) {
            ESP_LOGE(TAG, "Error setting header: 0x%x", error);
        }
    }

    if (includeAuth) {
        addAuthorization();
    }

    setHeader("content-type", "application/json");

    error = esp_http_client_set_post_field(client.get(), body, strlen(body));
    if (error != ESP_OK) {
        ESP_LOGE(TAG, "Error setting post field: %d", error);
    }
    error = esp_http_client_perform(client.get());
    if (error != ESP_OK) {
        ESP_LOGI(TAG, "Error perform post request field: %d", error);
    }
    return ESP_OK;
}


void HttpClient::addAuthorization() {
    uint16_t bearerLen = BEARER_HEADER_LEN + getAccessTokenLen();
    auto bearer = std::unique_ptr<char>(new char[bearerLen + 1]);
    memcpy(bearer.get(), BEARER_HEADER, BEARER_HEADER_LEN);
    memcpy(bearer.get() + BEARER_HEADER_LEN, getAccessToken(), getAccessTokenLen());
    bearer.get()[bearerLen] = 0;
    auto error = setHeader(AUTHORIZATION_HEADER, bearer.get());
    if (error != ESP_OK) {
        ESP_LOGE(TAG, "Error setting header: %d", error);
    }
}

esp_err_t HttpClient::eventHandler(esp_http_client_event_t *evt) {
    auto *httpClient = static_cast<HttpClient *>(evt->user_data);
    switch (evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "error");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            break;

        case HTTP_EVENT_HEADERS_SENT:
            break;

        case HTTP_EVENT_ON_HEADER:
            break;
        case HTTP_EVENT_ON_DATA:
            httpClient->data.addData(static_cast<char *>(evt->data), evt->data_len);
            break;
        case HTTP_EVENT_ON_FINISH:
            break;
        case HTTP_EVENT_DISCONNECTED:
            break;
    }
    return ESP_OK;
}

esp_err_t HttpClient::get(const char *url, std::list<Property> &&headers, bool includeAuth = false) {
    esp_err_t error;
    ESP_LOGI(TAG, "GET  %s", url);
    data.clear();

    setUrl(url);
    setMethod(HTTP_METHOD_GET);


    for (auto &header : headers) {
        error = setHeader(header.key, header.value);
        if (error != ESP_OK) {
            ESP_LOGE(TAG, "Error setting header: %d", error);
        }
    }

    if (includeAuth) {
        uint16_t bearerLen = BEARER_HEADER_LEN + getAccessTokenLen();
        auto bearer = std::unique_ptr<char>(new char[bearerLen + 1]);
        memcpy(bearer.get(), BEARER_HEADER, BEARER_HEADER_LEN);
        memcpy(bearer.get() + BEARER_HEADER_LEN, getAccessToken(), getAccessTokenLen());
        bearer.get()[bearerLen] = 0;
        error = setHeader(AUTHORIZATION_HEADER, bearer.get());
        if (error != ESP_OK) {
            ESP_LOGE(TAG, "Error setting header: %d", error);
        }

    }

    error = esp_http_client_perform(client.get());
    if (error != ESP_OK) {
        ESP_LOGI(TAG, "Error perform get request field: 0x%X", error);
    }


    return ESP_OK;
}

void HttpClient::addContentLength(size_t size) {
    char buffer[20];
    itoa(size, buffer, 10);
    auto error = setHeader(CONTENT_LENGTH, buffer);
    if (error != ESP_OK) {
        ESP_LOGE(TAG, "Error setting content length header: %d", error);
    }
}
