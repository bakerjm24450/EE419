idf_component_register(SRCS "app_main.c" "RGBLED.c" "WiFi.c"
                       PRIV_REQUIRES "esp_psram" "esp_driver_ledc" "esp_wifi" "nvs_flash" "esp_timer"
                       INCLUDE_DIRS ".")
