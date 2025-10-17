/* EE 419 Lab 5
 * Connects to AWS and publishes NFC tags as they are detected
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
    esp_log_level_set("MQTTClient", ESP_LOG_DEBUG);

    // system-level init
    ESP_ERROR_CHECK(init());

    // configure the rgb led
    ESP_ERROR_CHECK(led_config());

    // configure the NFC scanner
    ESP_ERROR_CHECK(nfcScanner_config());

    // connect to WiFi
    ESP_ERROR_CHECK(wifi_init());
    ESP_LOGD(TAG, "Connected to wifi");

    // connect to mqtt broker
    ESP_ERROR_CHECK(mqtt_init());
    ESP_LOGD(TAG, "Configured mqtt connection"); 
    
    // start with LED off
    ESP_ERROR_CHECK(led_on(YELLOW));

    for (;;)
    {
        static NFCTag prevTag = {.uidInt = 0xffffffff};

        // try to read an NFC tag
        NFCTag tag = nfcScanner_readTag();

        // is this a new tag?
        if (tag.uidInt != prevTag.uidInt)
        {
            ESP_LOGD(TAG, "Found tag %x %x %x %x (%x)", tag.uid[3], tag.uid[2], tag.uid[1], tag.uid[0], tag.uidInt);

            mqtt_publish_tag(tag);

            // remember this tag for next time
            prevTag = tag;
        }

        // wait a bit
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
