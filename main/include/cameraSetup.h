#ifndef _APP_CAMERA_H_
#define _APP_CAMERA_H_

#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22


#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_LED_ILLUMINATOR_ENABLED
void app_illuminator_startup();
void app_illuminator_shutdown();
#endif

void app_camera_startup();
void app_camera_reset();
void app_camera_shutdown();

#ifdef __cplusplus
}
#endif

#endif