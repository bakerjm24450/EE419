/* EE 419 Lab 2
 * Connects to WiFi, synchronizes the time, controls an RGB LED, scans for NFC tags, runs simple webserver
*/

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <esp_err.h>
#include <time.h>

#include "RGBLED.h"
#include "WiFi.h"

static const char *TAG = "main";

void app_main(void)
{
    esp_log_level_set(TAG, ESP_LOG_INFO);

    // configure the rgb led
    led_config();

    // connect to WiFi
    wifi_init();

    for (;;)
    {

        // print the current time
        time_t now = 0;
        struct tm timeinfo = {0};
        time(&now);
        localtime_r(&now, &timeinfo);
        ESP_LOGI(TAG, "\nTime: %s", asctime(&timeinfo));

        // cycle thru the colors
        led_on(WHITE);
        ESP_LOGI(TAG, "White");
        vTaskDelay(pdMS_TO_TICKS(500));

        led_on(RED);
        ESP_LOGI(TAG, "Red");
        vTaskDelay(pdMS_TO_TICKS(500));

        led_on(GREEN);
        ESP_LOGI(TAG, "Green");
        vTaskDelay(pdMS_TO_TICKS(500));

        led_on(BLUE);
        ESP_LOGI(TAG, "Blue");
        vTaskDelay(pdMS_TO_TICKS(500));

        led_on(MAGENTA);
        ESP_LOGI(TAG, "Magenta");
        vTaskDelay(pdMS_TO_TICKS(500));

        led_on(CYAN);
        ESP_LOGI(TAG, "Cyan");
        vTaskDelay(pdMS_TO_TICKS(500));

        led_on(YELLOW);
        ESP_LOGI(TAG, "Yellow");
        vTaskDelay(pdMS_TO_TICKS(500));

        led_off();
        ESP_LOGI(TAG, "Off");
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
