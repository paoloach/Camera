#include <stdint.h>
#include "driver/ledc.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "app_illuminator.h"


static const char *TAG = "Illuminator";
static const int LED_PIN=4;
static const int LED_CHANNEL=1;
static const int LED_MAX_INTENSITY=255;

void app_illuminator_startup() {

  gpio_config_t conf = { 0 };
  conf.pin_bit_mask = GPIO_SEL_4;
  conf.mode = GPIO_MODE_OUTPUT;
  gpio_config(&conf);


  ledc_timer_config_t ledc_timer = {
    .duty_resolution = LEDC_TIMER_8_BIT,            // resolution of PWM duty
    .freq_hz         = 1000,                        // frequency of PWM signal
    .speed_mode      = LEDC_LOW_SPEED_MODE,  // timer mode
    .timer_num       = LEDC_TIMER_1,        // timer index
    .clk_cfg = LEDC_AUTO_CLK
  };
  ledc_channel_config_t ledc_channel = {
    .channel    = LED_CHANNEL,
    .duty       = 0,
    .gpio_num   = LED_PIN,
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .hpoint     = 0,
    .timer_sel  = LEDC_TIMER_1
  };

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