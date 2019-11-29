#include "esp_camera.h"
#include "esp_http_server.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
//#include "img_converters.h"
//#include "fb_gfx.h"
#include "esp_log.h"
#include "sdkconfig.h"
#ifdef CONFIG_SNTP_ENABLED
#include "app_sntp.h"
#include <app_illuminator.h>
#include <sys/time.h>
#include <time.h>
#endif
#include "app_illuminator.h"

static const char *TAG = "camera_httpd";

httpd_handle_t stream_httpd = NULL;
httpd_handle_t camera_httpd = NULL;

extern EventGroupHandle_t event_group;

static int led_duty = 0;

static esp_err_t capture_handler(httpd_req_t *req) {
  camera_fb_t *fb = NULL;
  esp_err_t res;
  int64_t fr_start = esp_timer_get_time();

 // app_illuminator_set_led_intensity(led_duty);
  vTaskDelay(150 / portTICK_PERIOD_MS); // The LED requires ~150ms to "warm up"
  fb = esp_camera_fb_get();

 // app_illuminator_set_led_intensity(0);
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
  res = httpd_resp_send(req, (const char *)fb->buf, fb->len);
  esp_camera_fb_return(fb);
  int64_t fr_end = esp_timer_get_time();
  ESP_LOGI(TAG, "JPG: %uB %ums", (uint32_t)(fb_len),
           (uint32_t)((fr_end - fr_start) / 1000));
  return res;
}

void app_httpd_startup() {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();

  config.max_uri_handlers = 1;
  config.max_open_sockets = 3;
  config.max_uri_handlers = 3;
  config.max_resp_headers = 3;

  httpd_uri_t capture_uri = {.uri = "/",
                             .method = HTTP_GET,
                             .handler = capture_handler,
                             .user_ctx = NULL};

  ESP_LOGI(TAG, "Starting web server on port: '%d'", config.server_port);
  if (httpd_start(&camera_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(camera_httpd, &capture_uri);
  }
}

void app_httpd_shutdown() {
  if (stream_httpd)
    httpd_stop(stream_httpd);
  if (camera_httpd)
    httpd_stop(camera_httpd);
}
