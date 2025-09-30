/* EE 419 Lab 4
 * MQTT communication with local broker
 */

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <esp_err.h>
#include <esp_check.h>
#include <esp_event.h>
#include <time.h>
#include <nvs_flash.h>


#include "RGBLED.h"
#include "WiFi.h"
#include "NFCScanner.h"
#include "MQTTClient.h"

static const char *TAG = "main";

// System-level initialization
esp_err_t init()
{
    // initialize nvs (wifi config is stored in nvs)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // erase the old and try again
        ESP_RETURN_ON_ERROR(nvs_flash_erase(), TAG, "Cannot erase NVS");
        ret = nvs_flash_init();
    }
    ESP_RETURN_ON_ERROR(ret, TAG, "Cannot initialize NVS");

    // create default event loop
    ESP_RETURN_ON_ERROR(esp_event_loop_create_default(),
                        TAG, "Cannot create default event loop");

    return ESP_OK;                        
}

void app_main(void)
{
    esp_log_level_set(TAG, ESP_LOG_DEBUG);

    // system-level initi
    ESP_ERROR_CHECK(init());

    // configure the rgb led
    ESP_ERROR_CHECK(led_config());

    // configure the NFC scanner
    ESP_ERROR_CHECK(nfcScanner_config());

    // connect to WiFi
    ESP_ERROR_CHECK(wifi_init());

    // FIXME -- connect to mqtt broker

    // FIXME -- infinite loop to scan for NFC tags, publish info to broker
}
