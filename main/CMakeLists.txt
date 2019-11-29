set(COMPONENT_SRCS
        app_wifi.c
        app_settings.c
        app_httpd.c
        app_camera.c
        app_illuminator.c
        app_main.c)

set(COMPONENT_REQUIRES
        esp32-camera
    nvs_flash
    mdns
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
