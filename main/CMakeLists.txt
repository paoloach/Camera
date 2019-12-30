
idf_component_register(SRCS cameraWifi.c
                            Httpd.c
        cameraSetup.c
        cameraSntp.c
        cameraIlluminator.c
        cameraMain.c
                            settings.c
                            gDriverHttp.cpp
                            gdriver/HttpData.cpp
                            gdriver/HttpClient.cpp
                            gdriver/GDriverToken.cpp
                            gdriver/TokenCreation.cpp
        gdriver/gDriveMethods.cpp
        INCLUDE_DIRS "include"
                     ../components/esp32-camera/driver/include
                     ../components/esp32-camera/conversions/include
                    ../components/esp32-camera/driver/private_include
                    ../components/esp32-camera/sensors/private_include
                    ../components/esp32-camera/conversions/private_include
        PRIV_REQUIRES
            esp_netif
            esp32-camera
            nvs_flash
            mdns
            json
            esp_http_server
            esp_http_client

        )
