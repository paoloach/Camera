set(COMPONENT_SRCS
#        ../components/esp32-camera/driver/camera.c
#        ../components/esp32-camera/driver/sccb.c
#        ../components/esp32-camera/driver/sensor.c
#        ../components/esp32-camera/driver/twi.c
#        ../components/esp32-camera/driver/xclk.c
#        ../components/esp32-camera/sensors/ov2640.c
#        ../components/esp32-camera/sensors/ov3660.c
#        ../components/esp32-camera/sensors/ov7725.c
#        ../components/esp32-camera/conversions/yuv.c
#        ../components/esp32-camera/conversions/to_jpg.cpp
#        ../components/esp32-camera/conversions/to_bmp.c
#        ../components/esp32-camera/conversions/jpge.cpp
#        ../components/esp32-camera/conversions/esp_jpg_decode.c
        wifi.c
        app_httpd.c
        app_camera.c
        app_illuminator.c
        app_main.c)

set(COMPONENT_REQUIRES
        esp32-camera
        nvs_flash
        esp_http_server
        )


set(COMPONENT_ADD_INCLUDEDIRS
        "include"
        ../components/esp32-camera/driver/include
        ../components/esp32-camera/conversions/include)

set(COMPONENT_PRIV_INCLUDEDIRS
        ../components/esp32-camera/driver/private_include
        ../components/esp32-camera/sensors/private_include
        ../components/esp32-camera/conversions/private_include
        )


register_component()