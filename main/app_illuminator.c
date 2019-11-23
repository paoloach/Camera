#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "driver/ledc.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "app_illuminator.h"
#include "Pins.h"


static const char *TAG = "Illuminator";

void app_illuminator_startup() {
  gpio_set_direction(LED_LEDC_PIN,GPIO_MODE_OUTPUT);
  ledc_timer_config_t ledc_timer = {
    .duty_resolution = LEDC_TIMER_8_BIT,            // resolution of PWM duty
    .freq_hz         = 1000,                        // frequency of PWM signal
    .speed_mode      = LEDC_LOW_SPEED_MODE,  // timer mode
    .timer_num       = LED_TIMER        // timer index
  };
  ledc_channel_config_t ledc_channel = {
    .channel    = LED_CHANNEL,
    .duty       = 0,
    .gpio_num   = LED_LEDC_PIN,
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .hpoint     = 0,
    .timer_sel  = LED_TIMER
  };
  #ifdef CONFIG_LED_LEDC_HIGH_SPEED_MODE
  ledc_timer.speed_mode = ledc_channel.speed_mode = LEDC_HIGH_SPEED_MODE;
  #endif

  ESP_LOGI(TAG,"ledc_timer.clk_cfg = %d", ledc_timer.clk_cfg );
  ESP_LOGI(TAG,"ledc_timer.duty_resolution = %d", ledc_timer.duty_resolution );
  ESP_LOGI(TAG,"ledc_timer.freq_hz = %d", ledc_timer.freq_hz );
  ESP_LOGI(TAG,"ledc_timer.speed_mode = %d", ledc_timer.speed_mode );
  ESP_LOGI(TAG,"ledc_timer.timer_num = %d", ledc_timer.timer_num );


  switch (ledc_timer_config(&ledc_timer)) {
    case ESP_ERR_INVALID_ARG: ESP_LOGE(TAG, "ledc_timer_config() parameter error"); break;
    case ESP_FAIL: ESP_LOGE(TAG, "ledc_timer_config() Can not find a proper pre-divider number base on the given frequency and the current duty_resolution"); break;
    case ESP_OK: if (ledc_channel_config(&ledc_channel) == ESP_ERR_INVALID_ARG) {
        ESP_LOGE(TAG, "ledc_channel_config() parameter error");
      }
      break;
    default: break;
  }
}

void app_illuminator_shutdown() {
  ledc_stop(LEDC_LOW_SPEED_MODE,LED_CHANNEL,0);
}

void app_illuminator_set_led_intensity(uint8_t duty) {      // Turn LED On or Off
    uint8_t _duty = (duty > LED_MAX_INTENSITY) ? LED_MAX_INTENSITY : duty;
    
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LED_CHANNEL, _duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LED_CHANNEL);
    ESP_LOGI(TAG, "Set LED intensity to %d", _duty);
}