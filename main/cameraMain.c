#include <gDriverHttp.h>
#include <cJSON.h>
#include <GroupSignals.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "settings.h"
#include "cameraWifi.h"
#include "cameraSetup.h"
#include "Httpd.h"
#include "app_lcd.h"
#include "cameraIlluminator.h"

#ifdef CONFIG_MDNS_ENABLED

#include "mdns.h"

#endif

#include "cameraSntp.h"

EventGroupHandle_t event_group;
const char *HOSTNAME = "achdjianCamera";

static const char * TAG = "Main";

void app_main() {
    EventBits_t uxBits;

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    event_group = xEventGroupCreate();

    settingsStartup();
    app_camera_startup();
    app_illuminator_startup();
    app_wifi_startup();
    initGDriver();

    for (;;) {
        uxBits = xEventGroupWaitBits(event_group, WIFI_CONNECTED_BIT , pdFALSE, pdFALSE, 500 / portTICK_PERIOD_MS);
        if (uxBits > 0) {
            app_sntp_startup();
            ESP_ERROR_CHECK(mdns_init());
            ESP_ERROR_CHECK(mdns_hostname_set(HOSTNAME));
            ESP_ERROR_CHECK(mdns_instance_name_set(HOSTNAME));
            mdns_txt_item_t serviceTxtData[1] = {{"path", "/"}};
            ESP_ERROR_CHECK(mdns_service_add(HOSTNAME, "_http", "_tcp", 80, serviceTxtData, 1));
            app_httpd_startup();
            ESP_LOGI(TAG, "End MAIN");
            return;
        }
    }
    while (true){

    }
}