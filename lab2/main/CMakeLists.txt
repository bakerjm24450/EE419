idf_component_register(SRCS "app_main.c" "RGBLED.c" "WiFi.c" "NFCScanner.c" "WebServer.c"
                       PRIV_REQUIRES "esp_psram" "esp_driver_ledc" "esp_wifi" "nvs_flash" "esp_timer"
                       "esp_idf_pn532" "esp_driver_gpio" "esp_event"
                       INCLUDE_DIRS ".")