/* EE 419 Lab 2
 * Connects to WiFi, synchronizes the time, controls an RGB LED, scans for NFC tags
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
    esp_log_level_set("WiFi", ESP_LOG_INFO);

    // system-level initi
    ESP_ERROR_CHECK(init());

    // configure the rgb led
    ESP_ERROR_CHECK(led_config());

    // configure the NFC scanner
    ESP_ERROR_CHECK(nfcScanner_config());

    // connect to WiFi
    ESP_ERROR_CHECK(wifi_init());

    // start with LED off
    ESP_ERROR_CHECK(led_on(BLUE));

    for (;;)
    {
        static NFCTag prevTag = {.uidInt = 0};
        static NFCTag desiredTag = {.uidInt = 0x13e8d7b3};

        // try to read an NFC tag
        NFCTag tag = nfcScanner_readTag();

        // is this a new tag?
        if (tag.uidInt != prevTag.uidInt)
        {
            ESP_LOGD(TAG, "Found tag %x %x %x %x (%x)", tag.uid[3], tag.uid[2], tag.uid[1], tag.uid[0], tag.uidInt);

            // is this the tag we're looking for?
            if (tag.uidInt == desiredTag.uidInt)
            {
                // yes, so update LED
                ESP_ERROR_CHECK(led_on(GREEN));
            }
            else if (tag.uidInt != 0)
            {
                // wrong tag, so update LED
                ESP_ERROR_CHECK(led_on(RED));
            }
            else
            {
                // no tag present, so turn led off
                ESP_ERROR_CHECK(led_off());
            }

            // remember this tag for next time
            prevTag = tag;
        }

        // wait a bit
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
