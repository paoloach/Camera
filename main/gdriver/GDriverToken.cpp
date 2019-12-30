//
// Created by paolo on 04/12/19.
//

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "Constants.h"
#include "GDriverToken.h"
#include "HttpClient.h"
#include "cJSON.h"
#include "settings.h"

GDriverToken gDriverToken;


GDriverToken::GDriverToken() {

}

void GDriverToken::waitForAuthorized(uint16_t interval, uint16_t expiringTime) {
    authorizingInterval = interval;
    authorizingExpireTime = expiringTime;

    xTaskCreate(waitForAuthorizedTaskRTos, "waitUserCode", 4096, this, 0,
                &waitingUserCodeActivatedHandle);
}

void GDriverToken::waitForAuthorizedTaskRTos(void *args) {
    auto *GDriverToken = reinterpret_cast<class GDriverToken *>(args);
    GDriverToken->waitForAuthorizedTask();
}

void GDriverToken::waitForAuthorizedTask() {
    auto start = time(nullptr);
    bool exit=false;
    ESP_LOGI(TAG,"Start waiting for authorization");
    while ( (start + authorizingExpireTime > time(nullptr)) && !exit) {
        vTaskDelay((authorizingInterval * 1000) / portTICK_PERIOD_MS);
        ESP_LOGI(TAG,"New check for authorization");
        if (clientSecretLen() == 0) {
            ESP_LOGE(TAG, "Missing secret ID, waiting");
            continue;
        }

        Property propClientId(CLIENT_ID, CLIENT_ID_LEN, clientID(), clientIDLen());
        Property propSecret(CLIENT_SECRET, CLIENT_SECRET_LEN, clientSecret(),
                            clientSecretLen());
        Property propCode(CODE, CODE_LEN, deviceCode(), deviceCodeLen());
        Property propGrant(GRANT, GRANT_LEN, GRANT_TYPE, GRANT_TYPE_LEN);

        Property headerContentType(CONTENT_TYPE, CONTENT_TYPE_LEN,
                                   CONTENT_TYPE_FORM, CONTENT_TYPE_FORM_LEN);

        HttpClient httpClient;

        auto error = httpClient.post(
                "https://oauth2.googleapis.com/token",
                {propClientId, propSecret, propCode, propGrant}, {headerContentType});
        if (error != ESP_OK) {
            ESP_LOGE(TAG, "Internal error getting user code");
        } else {
            auto body(httpClient.getBody());
            if (body) {
                ESP_LOGI(TAG, "%s", body.get());

                auto root = deleted_unique_ptr<cJSON>(
                        cJSON_Parse(reinterpret_cast<const char *>(body.get())),
                        [](cJSON *cjson) { cJSON_Delete(cjson); });
                if (root) {
                    cJSON *jsonError = cJSON_GetObjectItem(root.get(), "error");
                    if (jsonError != nullptr) {
                        ESP_LOGI(TAG, "error: %s", jsonError->valuestring);
                        continue;
                    } else {

                        extractAccessToken(root.get());
                        extractExpireIn(root.get());
                        extractRefreshToken(root.get());
                        exit=true;
                        xTaskCreate(refreshTaskRTos, "refresh token", 4096, this, 0,
                                    &refreshTokenHandle);

                    }
                }
            }
        }
    }
    vTaskDelete(nullptr);
    waitingUserCodeActivatedHandle = nullptr;

}

void GDriverToken::refreshTaskRTos(void *args) {
    auto *GDriverToken = reinterpret_cast<class GDriverToken *>(args);
    GDriverToken->refreshTokenTask();
}

void GDriverToken::refreshTokenTask() {
    bool exit=false;
    while (!exit) {
        uint32_t expireTime = getAccessTokenExpire();
        vTaskDelay(((expireTime * 1000) / 3) / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "Refresh token");
        exit = !refreshToken();
    }
    vTaskDelete(nullptr);
    refreshTokenHandle = nullptr;
}

void GDriverToken::extractAccessToken(const cJSON *pJson) {
    cJSON *jsonAccessToken = cJSON_GetObjectItem(pJson, "access_token");
    if (jsonAccessToken != nullptr) {
        ESP_LOGI(TAG, "access token: %s", jsonAccessToken->valuestring);
        setAccessToken(jsonAccessToken->valuestring);
    } else {
        ESP_LOGE(TAG, "Unable to decode the response in order to get the access token");
    }
}

void GDriverToken::extractExpireIn(const cJSON *pJson) {
    cJSON *jsonExpireIn = cJSON_GetObjectItem(pJson, "expires_in");
    if (jsonExpireIn != nullptr) {

        time_t expireTime = time(nullptr) + jsonExpireIn->valueint;
        logTime(expireTime);
        setAccessTokenExpire(jsonExpireIn->valueint);
    } else {
        ESP_LOGE(TAG, "Unable to decode the response in order to get the access token expire time");
    }
}

void GDriverToken::extractRefreshToken(const cJSON *pJson) {
    cJSON *jsonRefreshToken = cJSON_GetObjectItem(pJson, "refresh_token");
    if (jsonRefreshToken != nullptr) {
        ESP_LOGI(TAG, "refresh token: %s", jsonRefreshToken->valuestring);
        setRefreshToken(jsonRefreshToken->valuestring);
    } else {
        ESP_LOGE(TAG, "Unable to decode the response in order to get the refresh token");
    }
}

bool GDriverToken::tokenValid() {
    if (getAccessTokenLen() ==0)
        return false;
    auto expireTime = getAccessTokenExpireTime();
    if (expireTime < time(nullptr)){
        logTime(expireTime);
        ESP_LOGE(TAG, "Token expired");
        setAccessToken(nullptr);
        return false;
    }
    return true;
}

void GDriverToken::logTime(time_t t) {
    char strftime_buf[64];
    struct tm timeInfo = {};
    localtime_r(&t, &timeInfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeInfo);
    ESP_LOGI(TAG, "Expire date is : %s", strftime_buf);
}

void GDriverToken::init() {
    if (getAccessTokenLen() > 0){
        if ( getAccessTokenExpireTime() < time(nullptr)){
            if (refreshToken()){
                xTaskCreate(refreshTaskRTos, "refresh token", 4096, this, 0,
                            &refreshTokenHandle);
            }
        }
    }
}

bool GDriverToken::refreshToken() {
    Property propRefreshToken(REFRESH_TOKEN, REFRESH_TOKEN_LEN, getRefreshToken(), getRefreshTokenLen());
    Property propCode(CODE, CODE_LEN, deviceCode(), deviceCodeLen());
    Property propClientId(CLIENT_ID, CLIENT_ID_LEN, clientID(), clientIDLen());
    Property propSecret(CLIENT_SECRET, CLIENT_SECRET_LEN, clientSecret(), clientSecretLen());
    Property propGrant(GRANT, GRANT_LEN, GRANT_TYPE, GRANT_TYPE_LEN);

    Header headerContentType{CONTENT_TYPE, CONTENT_TYPE_FORM};

    HttpClient httpClient;
    auto error = httpClient.post("https://oauth2.googleapis.com/token", {propRefreshToken, propCode, propClientId, propSecret, propGrant},
                            {headerContentType});

    if (error != ESP_OK) {
        ESP_LOGE(TAG, "Internal error refreshing token");
    } else {

        auto body(httpClient.getBody());
        if (body) {
            ESP_LOGI(TAG, "%s", body.get());

            auto root = deleted_unique_ptr<cJSON>(
                    cJSON_Parse(reinterpret_cast<const char *>(body.get())),
                    [](cJSON *cjson) { cJSON_Delete(cjson); });
            if (root) {
                cJSON *jsonError = cJSON_GetObjectItem(root.get(), "error");
                if (jsonError != nullptr) {
                    ESP_LOGE(TAG, "error: %s", jsonError->valuestring);
                    ESP_LOGI(TAG, "remove token. You need to authenticate again");
                    setAccessToken(nullptr);
                    return false;
                } else {
                    extractAccessToken(root.get());
                    extractExpireIn(root.get());
                }
            } else {
                ESP_LOGE(TAG, "invalid json refreshing the token");
            }
        } else {
            ESP_LOGE(TAG,"Empty body refreshing the token");
        }
    }
    return true;
}

