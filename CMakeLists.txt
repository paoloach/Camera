# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(EXTRA_COMPONENT_DIRS components)

include(/home/paolo/esp2866/espressif/esp-idf/tools/cmake/project.cmake)
project(camera C CXX ASM)

#target_compile_options(__idf_esp32-camera PUBLIC
#        -DCONFIG_OV2640_SUPPORT=y
#        -DCONFIG_OV3660_SUPPORT=y
#        -DCONFIG_SCCB_HARDWARE_I2C=y
#        -DCONFIG_CAMERA_CORE0=y
#        -DCONFIG_LED_LEDC_LOW_SPEED_MODE=y
#        -DCONFIG_LED_ILLUMINATOR_ENABLED=y
#        -DCONFIG_LED_LEDC_PIN=4
#        -DCONFIG_LED_MAX_INTENSITY=255
#        -DCONFIG_LED_LEDC_LOW_SPEED_MODE=y
#        -DCONFIG_LED_LEDC_HIGH_SPEED_MODE=
#        -DCONFIG_LED_LEDC_TIMER=1
#        -DCONFIG_LED_LEDC_CHANNEL=1
#        -DCONFIG_CAMERA_MODEL_AI_THINKER=y)