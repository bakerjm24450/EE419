// Manage the WiFi connection

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_err.h>
#include <esp_check.h>
#include <nvs_flash.h>
#include <esp_netif_sntp.h>
#include <esp_sntp.h>

#include "WiFi.h"

// tag for log messages
static const char *TAG = "WiFi";

// SSID and password set through menuconfig
#define WIFI_SSID CONFIG_WIFI_SSID
#define WIFI_PASSWORD CONFIG_WIFI_PASSWORD

// FreeRTOS event group used to wait for connection
#define WIFI_CONNECTED_BIT BIT0
static EventGroupHandle_t wifi_event_group;

// Event handler for WiFi- and LWiP-related events
static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    // WiFi events
    if (event_base == WIFI_EVENT)
    {
        if (event_id == WIFI_EVENT_STA_START)
        {
            // connect to ap
            esp_wifi_connect();
        }
        else if (event_id == WIFI_EVENT_STA_DISCONNECTED)
        {
            // try to reconnect
            ESP_LOGI(TAG, "Disconnected, attempting to reconnect");
            esp_wifi_connect();
        }
    }

    // LWiP events
    else if (event_base == IP_EVENT)
    {
        if (event_id == IP_EVENT_STA_GOT_IP)
        {
            // we're connected
            ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
            ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));

            // signal event group
            xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
        }
    }
}

// Configure WiFi and connect
// Returns ESP_OK if connection succeeds, otherwise returns an error code
esp_err_t wifi_init()
{
    // create event group to use for sync
    wifi_event_group = xEventGroupCreate();
    ESP_RETURN_ON_FALSE(wifi_event_group != NULL, ESP_ERR_NO_MEM, TAG, "Cannot create WiFi event group");

    // initialize TCP/IP stack
    ESP_RETURN_ON_ERROR(esp_netif_init(), TAG, "Cannot initialize TCP/IP stack");

    // create default wifi sta (this call aborts if it fails)
    esp_netif_create_default_wifi_sta();

    // initialize wifi system using default settings
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_RETURN_ON_ERROR(esp_wifi_init(&cfg), TAG, "Cannot initialize WiFi system");

    // register event handler
    ESP_RETURN_ON_ERROR(esp_event_handler_instance_register(WIFI_EVENT,
                                                            ESP_EVENT_ANY_ID,
                                                            &event_handler,
                                                            NULL,
                                                            NULL),
                        TAG,
                        "Cannot register WiFi event handler");
    ESP_RETURN_ON_ERROR(esp_event_handler_instance_register(IP_EVENT,
                                                            IP_EVENT_STA_GOT_IP,
                                                            &event_handler,
                                                            NULL,
                                                            NULL),
                        TAG,
                        "Cannot register IP event handler");

    // configure the STA
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASSWORD,
            .threshold.authmode = WIFI_AUTH_WPA_WPA2_PSK,
        },
    };

    ESP_RETURN_ON_ERROR(esp_wifi_set_mode(WIFI_MODE_STA), TAG, "Cannot set WiFi mode to STA");
    ESP_RETURN_ON_ERROR(esp_wifi_set_config(WIFI_IF_STA, &wifi_config), TAG, "Cannot configure WiFi STA");

    // start the wifi
    ESP_RETURN_ON_ERROR(esp_wifi_start(), TAG, "Cannot start WiFi");

    // wait until we're connected
    ESP_RETURN_ON_FALSE(xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT,
                                            pdFALSE, pdFALSE, pdMS_TO_TICKS(30000)) &
                            WIFI_CONNECTED_BIT,
                        ESP_FAIL, TAG, "Timeout connecting to WiFi");

    // connected successfully
    ESP_LOGI(TAG, "WiFi connected");               

    // connect to SNTP server
    esp_sntp_config_t sntp_config = ESP_NETIF_SNTP_DEFAULT_CONFIG(CONFIG_SNTP_SERVER);
    ESP_RETURN_ON_ERROR(esp_netif_sntp_init(&sntp_config), TAG, "Cannot configure SNTP");

    // wait for SNTP connection
    ESP_RETURN_ON_ERROR(esp_netif_sntp_sync_wait(pdMS_TO_TICKS(30000)), TAG, "Failed to update system time");

    // set our timezone
    setenv("TZ", CONFIG_SNTP_TIME_ZONE, 1);
    tzset();
    
    return ESP_OK;
}
