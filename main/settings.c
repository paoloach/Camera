//
// Created by paolo on 29/11/19.
//
#include <esp_log.h>
#include <nvs.h>
#include <nvs_flash.h>
#include <stddef.h>
#include <string.h>
#include <time.h>

static const char *TAG = "settings";
static const char *NVS_NAME = "settings";
static const char  * SETTING_KEY="HTTP_KEY";

static const  char  MAGIC1[16] = "CAMVERSION1.0553";

struct Settings {
  char magic[16];
  uint16_t lenClientID;
  char clientId[256];
  uint16_t lenSecretId;
  char clientSecret[256];
  uint16_t lenDeviceCode;
  char deviceCode[256];
  uint16_t lenAccessToken;
  char accessToken[256];
  uint32_t accessTokenExpire;
  time_t   accessTokenExpireTime;
  uint16_t lenRefreshToken;
  char refreshToken[256];

};

static struct Settings settings;

static void settingReset() {
  nvs_handle_t handle;

  ESP_LOGI(TAG, "Erasing settings from NVS");
  if (nvs_open(NVS_NAME, NVS_READWRITE, &handle) == ESP_OK) {
    nvs_erase_all(handle);
    nvs_close(handle);
  }
}


void settingsSave() {
  nvs_handle_t handle;
  esp_err_t ret;

  ret = nvs_open(NVS_NAME,NVS_READWRITE,&handle);
  if (ret == ESP_OK) {
    size_t size = sizeof(settings);
    memcpy(settings.magic, MAGIC1, sizeof(settings.magic));
    ret = nvs_set_blob(handle, SETTING_KEY,&settings,size);
    if (ret == ESP_OK) {
      nvs_commit(handle);
      ESP_LOGI(TAG,"Saved settings to NVS");
    } else {
      ESP_LOGE(TAG,"Error (%d) saving settings to NVS",ret);
    }
    nvs_close(handle);
  } else {
    ESP_LOGE(TAG,"Error (%d) opening settings",ret);
  }
}

void settingsStartup() {
  nvs_handle_t handle;

  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    nvs_flash_init();
  }

  ESP_LOGI(TAG, "NVS Flash Init");

  ret = nvs_open(NVS_NAME, NVS_READONLY, &handle);
  if (ret == ESP_OK) {
    size_t size = sizeof(settings);
    ret = nvs_get_blob(handle, SETTING_KEY, &settings, &size);
    if (ret == ESP_OK) {

      if (strncmp(settings.magic,MAGIC1, sizeof(settings.magic) ) != 0){
        ESP_LOGE(TAG, "WRONG Setting data for key %s", SETTING_KEY);
        settingReset();

      } else {
        ESP_LOGI(TAG, "Settings loaded from NVS");
      }

    } else {
      settingReset();
    }
    nvs_close(handle);
  } else {
    settingReset();
  }
}

void setClientId(const char * clientID, uint16_t len) {
  memcpy(settings.clientId, clientID, len);
  settings.lenClientID=len;
  settingsSave();
}
void setSecretId(const char * secretId, uint16_t len) {
  memcpy(settings.clientSecret, secretId, len);
  settings.lenSecretId=len;
  settingsSave();
}

void setDeviceCode(const char *deviceCode, uint16_t len) {
  memcpy(settings.deviceCode, deviceCode, len);
  settings.lenDeviceCode=len;
  settingsSave();
}

void setAccessToken(const char *accessToken) {
  if (accessToken == NULL){
    settings.lenAccessToken=0;
  } else {
    uint16_t len = strlen(accessToken);
    memcpy(settings.accessToken, accessToken, len);
    settings.lenAccessToken = len;
  }
  settingsSave();
}

void setAccessTokenExpire(uint32_t expire) {
  settings.accessTokenExpire=expire;
  settings.accessTokenExpireTime=expire+time(NULL);
  settingsSave();
}

void setRefreshToken(const char *refreshToken) {
  uint16_t len = strlen(refreshToken);
  memcpy(settings.refreshToken, refreshToken, len);
  settings.lenRefreshToken =len;
  settingsSave();
}

const char * clientID(void) {
  return settings.clientId;
}
const char * clientSecret(void){
  return settings.clientSecret;
}
const char * deviceCode(void){
  return settings.deviceCode;
}

const char * getAccessToken(){
  return settings.accessToken;
}

const char * getRefreshToken(){
  return settings.refreshToken;
}

uint16_t clientIDLen(void) {
  return settings.lenClientID;
}
uint16_t  clientSecretLen(void){
  return settings.lenSecretId;
}
uint16_t  deviceCodeLen(void){
  return settings.lenDeviceCode;
}
uint16_t getAccessTokenLen(void) {
  return settings.lenAccessToken;
}
uint16_t getRefreshTokenLen(void) {
  return settings.lenRefreshToken;
}

uint32_t getAccessTokenExpire() {
  return settings.accessTokenExpire;
}

time_t getAccessTokenExpireTime(){
  return settings.accessTokenExpireTime;
}