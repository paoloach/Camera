//
// Created by paolo on 04/12/19.
//

#pragma once

#include "HttpClient.h"
#include <cJSON.h>
#include <esp_log.h>
#include <freertos/task.h>
#include <memory>

class GDriverToken {
private:
    static constexpr const char *TAG = "GDriverToken";

public:
    GDriverToken();
    void waitForAuthorized(uint16_t interval, uint16_t expiringTime);
    bool tokenValid();
    void init();
    bool refreshToken();
private:
    static void waitForAuthorizedTaskRTos(void *);

    void waitForAuthorizedTask();

    static void refreshTaskRTos(void *);

    void refreshTokenTask();


    static void extractAccessToken(const cJSON *pJson);

    static void extractExpireIn(const cJSON *pJson);

    static void extractRefreshToken(const cJSON *pJson);
    static void logTime(time_t t);

    TaskHandle_t waitingUserCodeActivatedHandle;
    TaskHandle_t refreshTokenHandle;


    uint16_t authorizingInterval;
    uint16_t authorizingExpireTime;


};

extern GDriverToken gDriverToken;