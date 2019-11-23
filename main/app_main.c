#include "app_camera.h"
#include "app_httpd.h"
#include "esp_event.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "sdkconfig.h"
#include "wifi.h"
#include <esp_log.h>
#include <nvs_flash.h>
#ifdef CONFIG_MDNS_ENABLED
#include "mdns.h"
#endif
#ifdef CONFIG_SNTP_ENABLED
#include "app_sntp.h"
#endif

EventGroupHandle_t event_group;

static void initNvs();

void app_shutdown() {
  app_illuminator_shutdown();
  #ifdef CONFIG_MDNS_ENABLED
  mdns_free();
  #endif

  app_httpd_shutdown();
  app_wifi_shutdown();
  app_camera_shutdown();
}

static void initNvs() {
  //Initialize NVS
  ESP_LOGI("initNVS", "Initializing NVS");
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);
  ESP_LOGI("initNVS", "NVS Initialized");
}

void app_main()
{

    EventBits_t uxBits;

  ESP_ERROR_CHECK(esp_event_loop_create_default());   
  event_group = xEventGroupCreate();
  initNvs();
  app_camera_startup();
  app_illuminator_startup();
  app_wifi_startup();
  
  for (;;) {
	  uxBits = xEventGroupWaitBits(event_group,WIFI_CONNECTED_BIT | WIFI_SOFTAP_BIT,pdFALSE,pdFALSE,500 / portTICK_PERIOD_MS);
	  if (uxBits > 0) {	  
      #ifdef CONFIG_SNTP_ENABLED
      app_sntp_startup();
      #endif
      #ifdef CONFIG_MDNS_ENABLED
      ESP_ERROR_CHECK(mdns_init());
//      ESP_ERROR_CHECK(mdns_hostname_set(settings.hostname));
//      ESP_ERROR_CHECK(mdns_instance_name_set(settings.mdns_instance));
      mdns_txt_item_t serviceTxtData[1] = { {"path","/"} };
//      ESP_ERROR_CHECK( mdns_service_add(settings.mdns_instance, "_http", "_tcp", 80, serviceTxtData, 1) );
      #endif
      app_httpd_startup();
      return;
    }
  }
  esp_register_shutdown_handler(&app_shutdown);
}