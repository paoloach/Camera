

#include "esp_camera.h"
#include "esp_http_server.h"

#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "sdkconfig.h"
#include "cameraIlluminator.h"
#include "settings.h"
#include "gDriverHttp.h"


static const char *TAG = "camera_httpd";

httpd_handle_t camera_httpd = NULL;
#define CLIENT_ID_MAX_SIZE 256

extern EventGroupHandle_t event_group;

#define UINT_MAX_DIGIT 20


static esp_err_t getPhotoPeriodHandler(httpd_req_t *req) {
    char buffer[UINT_MAX_DIGIT+3];
    itoa(getPhotoPeriod(), buffer, 10);
    strcat(buffer,"\n\r");
    httpd_resp_sendstr(req, buffer);
    httpd_resp_set_type(req,"text/plain");
    return ESP_OK;
}

static esp_err_t setPhotoPeriodHandler(httpd_req_t *req) {
    int totalLen = req->content_len;
    char buffer[UINT_MAX_DIGIT+1];
    memset(buffer, 0, UINT_MAX_DIGIT+1);
    if (totalLen > UINT_MAX_DIGIT) {
        totalLen = UINT_MAX_DIGIT;
    }
    int readByte = httpd_req_recv(req, buffer, totalLen);
    if (readByte>0){
        char * endPtr;
        uint32_t period = strtol(buffer, &endPtr, 10);
        if (endPtr > buffer){
            setPhotoPeriod(period);
            httpd_resp_sendstr(req, "\n\r");
        } else {
            httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Photo period must be an integer value");
        }
    } else {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Photo period must be an integer value");
    }
    return ESP_OK;
}

static esp_err_t setClientIdHandler(httpd_req_t *req) {
    char clientId[CLIENT_ID_MAX_SIZE];
    int totalLen = req->content_len;
    int curLen = 0;
    int received = 0;
    if (totalLen >= CLIENT_ID_MAX_SIZE) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Content too long");
        return ESP_FAIL;
    }
    while (curLen < totalLen) {
        received = httpd_req_recv(req, clientId + curLen, totalLen);
        if (received < 0) {
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Fail to send client ID");
            return ESP_FAIL;
        }
        curLen += received;
    }
    setClientId(clientId, totalLen);
    httpd_resp_sendstr(req, "\n\r");
    return ESP_OK;
}

static esp_err_t setSecretIdHandler(httpd_req_t *req) {
    char secretId[CLIENT_ID_MAX_SIZE];
    int totalLen = req->content_len;
    int curLen = 0;
    int received = 0;
    if (totalLen >= CLIENT_ID_MAX_SIZE) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Content too long");
        return ESP_FAIL;
    }
    while (curLen < totalLen) {
        received = httpd_req_recv(req, secretId + curLen, totalLen);
        if (received < 0) {
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Fail to send secret ID");
            return ESP_FAIL;
        }
        curLen += received;
    }
    setSecretId(secretId, totalLen);
    httpd_resp_sendstr(req, "\n\r");
    return ESP_OK;
}

static esp_err_t setDeviceCodeHandler(httpd_req_t *req) {
    char deviceCode[CLIENT_ID_MAX_SIZE];
    int totalLen = req->content_len;
    int curLen = 0;
    int received = 0;
    if (totalLen >= CLIENT_ID_MAX_SIZE) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Content too long");
        return ESP_FAIL;
    }
    while (curLen < totalLen) {
        received = httpd_req_recv(req, deviceCode + curLen, totalLen);
        if (received < 0) {
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Fail to send secret ID");
            return ESP_FAIL;
        }
        curLen += received;
    }
    setDeviceCode(deviceCode, totalLen);
    httpd_resp_sendstr(req, "\n\r");
    return ESP_OK;
}

static esp_err_t capture_handler(httpd_req_t *req) {
    camera_fb_t *fb = NULL;
    esp_err_t res;
    int64_t fr_start = esp_timer_get_time();

    app_illuminator_set_led_intensity(200);
    vTaskDelay(150 / portTICK_PERIOD_MS); // The LED requires ~150ms to "warm up"
    fb = esp_camera_fb_get();

    app_illuminator_set_led_intensity(0);
    if (!fb) {
        ESP_LOGE(TAG, "Camera capture failed");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, "image/jpeg");
    httpd_resp_set_hdr(req, "Content-Disposition",
                       "inline; filename=capture.jpg");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

    size_t fb_len = 0;
    fb_len = fb->len;
    res = httpd_resp_send(req, (const char *) fb->buf, fb->len);
    saveImage("serra", fb->buf, fb->len);
    esp_camera_fb_return(fb);
    int64_t fr_end = esp_timer_get_time();
    ESP_LOGI(TAG, "JPG: %uB %ums", (uint32_t) (fb_len),
             (uint32_t) ((fr_end - fr_start) / 1000));
    return res;
}

void app_httpd_startup() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    config.max_open_sockets = 3;
    config.max_uri_handlers = 10;
    config.max_resp_headers = 3;

    httpd_uri_t capture_uri = {.uri = "/",
            .method = HTTP_GET,
            .handler = capture_handler,
            .user_ctx = NULL};

    httpd_uri_t setPhotoPeriodUri = {.uri = "/photo-period",
            .method = HTTP_POST,
            .handler = setPhotoPeriodHandler,
            .user_ctx = NULL};

    httpd_uri_t getPhotoPeriodUri = {.uri = "/photo-period",
            .method = HTTP_GET,
            .handler = getPhotoPeriodHandler,
            .user_ctx = NULL};

    httpd_uri_t setClientIdUri = {.uri = "/client-id",
            .method = HTTP_POST,
            .handler = setClientIdHandler,
            .user_ctx = NULL};

    httpd_uri_t setSecretIdURI = {.uri = "/secret-id",
            .method = HTTP_POST,
            .handler = setSecretIdHandler,
            .user_ctx = NULL};

    httpd_uri_t setCodeURI = {.uri = "/device-code",
            .method = HTTP_POST,
            .handler = setDeviceCodeHandler,
            .user_ctx = NULL};

    httpd_uri_t getUserCode = {.uri = "/getUserCode",
            .method = HTTP_GET,
            .handler = getUserCodeHandler,
            .user_ctx = NULL};

    httpd_uri_t refreshCode = {.uri = "/refresh",
            .method = HTTP_POST,
            .handler = postRefreshTokenHandler,
            .user_ctx = NULL};

    ESP_LOGI(TAG, "Starting web server on port: '%d'", config.server_port);
    if (httpd_start(&camera_httpd, &config) == ESP_OK) {
        httpd_register_uri_handler(camera_httpd, &capture_uri);
        httpd_register_uri_handler(camera_httpd, &setPhotoPeriodUri);
        httpd_register_uri_handler(camera_httpd, &getPhotoPeriodUri);
        httpd_register_uri_handler(camera_httpd, &setClientIdUri);
        httpd_register_uri_handler(camera_httpd, &setSecretIdURI);
        httpd_register_uri_handler(camera_httpd, &setCodeURI);
        httpd_register_uri_handler(camera_httpd, &getUserCode);
        httpd_register_uri_handler(camera_httpd, &refreshCode);
    }
}

