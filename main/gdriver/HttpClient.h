//
// Created by paolo on 04/12/19.
//

#pragma once

#include <list>
#include <esp_http_client.h>
#include <esp_log.h>
#include "HttpData.h"
#include "delete_unique_ptr.h"


struct Property {
    const char *key;
    uint16_t keyLen;
    const char *value;
    uint16_t valueLen;

    Property(const char *key, uint16_t keyLen, const char *value, uint16_t valueLen) : key(key), keyLen(keyLen),
                                                                                       value(value),
                                                                                       valueLen(valueLen) {
        if (keyLen==0)
            keyLen=strlen(key);
        if (valueLen==0)
            valueLen=strlen(value);
    }
};


struct Header {
    const char *key;
    const char *value;
};


class HttpClient {
public:
    HttpClient();

    esp_err_t post(const char *url, std::list<Property> &&properties, std::list<Property> &&headers);
    esp_err_t post(const char *url,const char * body,  std::list<Header> &&headers);
    esp_err_t patch(const char *url,const uint8_t * body,  size_t bodySize, std::list<Header> &&headers);
    esp_err_t postJson(const char *url, const char * body, std::list<Property> &&headers, bool includeAuth);

    esp_err_t get(const char *url, std::list<Property> &&headers, bool includeAuth);

    std::unique_ptr<char> getBody() { return data.getContent(); }

private:
    esp_err_t setUrl(const char * url){
        return esp_http_client_set_url(client.get(), url);
    }

    esp_err_t setMethod(esp_http_client_method_t method){
        return esp_http_client_set_method(client.get(), method);
    }


    esp_err_t setHeader(const char * key, const char * value){
        return esp_http_client_set_header(client.get(), key, value);
    }


    static constexpr const char *TAG = "HttpClient";
    deleted_unique_ptr<esp_http_client> client;
    HttpData data;
private:
    static deleted_unique_ptr<esp_http_client> createClient(esp_http_client_config_t *config) {
        esp_http_client_handle_t client = esp_http_client_init(config);
        return deleted_unique_ptr<esp_http_client>(client, [](esp_http_client *client) {
            esp_http_client_cleanup(client);
        });
    }

    static esp_err_t eventHandler(esp_http_client_event_t *evt);

    void addAuthorization();
    void addContentLength(size_t size);
};

