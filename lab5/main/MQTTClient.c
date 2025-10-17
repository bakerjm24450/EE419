// Routines for MQTT communication

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <esp_err.h>
#include <esp_check.h>
#include <esp_log.h>
#include <esp_wifi.h>
#include <mqtt_client.h>
#include <esp_mac.h>
#include <cJSON.h>
#include <time.h>

#include "MQTTClient.h"
#include "RGBLED.h"
#include "NFCScanner.h"

static const char *TAG = "MQTTClient";

// handle to client
static esp_mqtt_client_handle_t client = NULL;

// Config values
static const char *MQTT_BROKER = CONFIG_MQTT_BROKER;
static const uint32_t MQTT_PORT = CONFIG_MQTT_PORT;
static const int MQTT_QOS = CONFIG_MQTT_QOS;
static const char *THING_NAME = CONFIG_MQTT_NAME;

// mqtt message topics
static const char *STATUS_TOPIC = "ee419/" CONFIG_MQTT_NAME "/status";
static const char *CMD_TOPIC = "ee419/" CONFIG_MQTT_NAME "/cmd";

// event group to wait for connection
#define MQTT_CONNECTED_BIT BIT0
static EventGroupHandle_t mqtt_event_group;

// x509 certificates stored externally
extern const uint8_t device_cert_pem_start[] asm("_binary_device_cert_pem_start");
extern const uint8_t device_cert_pem_end[] asm("_binary_device_cert_pem_end");
extern const uint8_t device_key_pem_start[] asm("_binary_device_private_key_start");
extern const uint8_t device_key_pem_end[] asm("_binary_device_private_key_end");
extern const uint8_t server_cert_pem_start[] asm("_binary_AmazonRootCA1_pem_start");
extern const uint8_t server_cert_pem_end[] asm("_binary_AmazonRootCA1_pem_end");

// forward declarations
static void mqtt_event_handler(void *handler_args, esp_event_base_t base,
                               int32_t event_id, void *event_data);
static esp_err_t mqtt_publish(const char *topic, const char *payload);
static esp_err_t mqtt_parse_msg(esp_mqtt_event_handle_t event_data);

/*
 * @brief Initialize MQTT client and connect to broker
 *
 * Also determines cmd and status topics, taking MAC addr into account.
 *
 */
esp_err_t mqtt_init()
{
    // create an event group to use for sync
    mqtt_event_group = xEventGroupCreate();
    ESP_RETURN_ON_FALSE(mqtt_event_group != NULL, ESP_ERR_NO_MEM, TAG, "Cannot create MQTT event group");

    // initialize config struct
    static const char *alpn_protos[] = {"x-amzn-mqtt-ca", NULL};
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker = {
            .address = {
                .hostname = MQTT_BROKER,
                .transport = MQTT_TRANSPORT_OVER_SSL,
                .port = MQTT_PORT,
            },
            .verification = {
                .certificate = (const char *)server_cert_pem_start,
                .certificate_len = 0,
                .alpn_protos = alpn_protos,
            },
        },
        .credentials = {
            .username = NULL,
            .client_id = THING_NAME,
            .set_null_client_id = false,
            .authentication = {
                .password = NULL,
                .certificate = (const char *)device_cert_pem_start,
                .certificate_len = 0,
                .key = (const char *)device_key_pem_start,
                .key_len = 0,
                .key_password = NULL,
                .key_password_len = 0,
                .use_secure_element = false,
                .ds_data = NULL,
            },
        },
        .session.protocol_ver = MQTT_PROTOCOL_V_5,
    };

    // initialize the mqtt client
    client = esp_mqtt_client_init(&mqtt_cfg);
    ESP_RETURN_ON_FALSE(client != NULL, ESP_FAIL, TAG, "Cannot initialize mqtt client");

    // register event handler
    ESP_RETURN_ON_ERROR(
        esp_mqtt_client_register_event(client,
                                       ESP_EVENT_ANY_ID,
                                       mqtt_event_handler,
                                       NULL),
        TAG,
        "Cannot register MQTT event handler");

    // start mqtt client
    ESP_LOGD(TAG, "Connecting to broker %s", MQTT_BROKER);
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_mqtt_client_start(client));

    // wait until we're connected
    ESP_RETURN_ON_FALSE(xEventGroupWaitBits(mqtt_event_group, MQTT_CONNECTED_BIT,
                                            pdFALSE, pdFALSE, pdMS_TO_TICKS(30000)) &
                            MQTT_CONNECTED_BIT,
                        ESP_FAIL, TAG, "Timeout connecting to MQTT broker");

    return ESP_OK;
}

/*
 * @brief Publish an NFC tag to MQTT broker as a JSON msg.
 * The message will have the following format:
 * {
 *   "timestamp" : "Fri Oct 16 14:15L52 2025",
 *   "tag" : 3748571
 * }
 *
 * @param tag NFC tag value to publish
 */
esp_err_t mqtt_publish_tag(NFCTag tag)
{
    time_t rawtime;

    // get current time
    time(&rawtime);

    // build the message
    cJSON *msg_json = cJSON_CreateObject();

    ESP_RETURN_ON_FALSE(cJSON_AddStringToObject(msg_json, "timestamp", ctime(&rawtime)) != NULL,
                        ESP_FAIL, TAG, "Error building JSON message");

    ESP_RETURN_ON_FALSE(cJSON_AddNumberToObject(msg_json, "tag", tag.uidInt) != NULL,
                        ESP_FAIL, TAG, "Error building JSON message");

    char *msg = cJSON_Print(msg_json);

    // publish the message
    ESP_RETURN_ON_ERROR(mqtt_publish(STATUS_TOPIC, msg), TAG, "Could not publish message");

    // clean up
    cJSON_Delete(msg_json);
    free(msg);

    return ESP_OK;
}

/*
 * @brief Publish a message to MQTT broker
 *
 * @param topic MQTT topic to publish.
 * @param payload Body of message to be published
 *
 */
static esp_err_t mqtt_publish(const char *topic, const char *payload)
{
    ESP_LOGI(TAG, "Publishing topic: %s, payload %s", topic, payload);

    // publish to broker
    if (client != NULL)
    {
        ESP_RETURN_ON_FALSE(esp_mqtt_client_publish(
                                client,
                                topic,
                                payload,
                                0,
                                MQTT_QOS,
                                0) >= 0,
                            ESP_FAIL,
                            TAG,
                            "Cannot publish MQTT message");

        return ESP_OK;
    }
    else
    {
        ESP_LOGW(TAG, "Attempting to publish before connection to broker");

        return ESP_FAIL;
    }
}

/*
 * @brief Event handler registered to receive MQTT and other events
 *
 *  This function is called by the MQTT client event loop.
 *
 * @param handler_args user data registered to the event.
 * @param base Event base for the handler(always MQTT Base in this example).
 * @param event_id The id for the received event.
 * @param event_data The data for the event, esp_mqtt_event_handle_t.
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    // get handle to client
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;

    switch ((esp_mqtt_event_id_t)event_id)
    {
    // successfully connected to broker
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");

        // subscribe to command messages
        ESP_LOGI(TAG, "Subscribing to %s", CMD_TOPIC);

        int msg_id = esp_mqtt_client_subscribe(client, CMD_TOPIC, MQTT_QOS);
        if (msg_id < 0)
        {
            ESP_LOGW(TAG, "Cannot subscribe to MQTT topic %s", CMD_TOPIC);
        }
        break;

    // successfully subscribed
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGD(TAG, "MQTT_EVENT_SUBSCRIBED");

        // signal the event group since we're now ready to go
        xEventGroupSetBits(mqtt_event_group, MQTT_CONNECTED_BIT);
        
        break;

    // successfully published
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGD(TAG, "MQTT_EVENT_PUBLISHED");
        break;

    // received msg
    case MQTT_EVENT_DATA:
        ESP_LOGD(TAG, "MQTT_EVENT_DATA -- received msg");

        // parse the message
        ESP_ERROR_CHECK_WITHOUT_ABORT(mqtt_parse_msg(event));
        break;

    // handle any errors
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        break;

    // ignore all others
    default:
        break;
    }
}

// Parse a received message and handle it as needed. The message payload is a JSON
// packet with the following format:
//
// {
//   "timestamp" : "Fri Jul 18 14:15L52 2025",
//   "color" : 255
// }
//
static esp_err_t mqtt_parse_msg(esp_mqtt_event_handle_t event_data)
{
    // parse the message
    cJSON *msg = cJSON_ParseWithLength(event_data->data, event_data->data_len);
    ESP_RETURN_ON_FALSE(msg != NULL, ESP_FAIL, TAG, "Error parsing JSON msg");

    ESP_LOGD(TAG, "Received mqtt msg %d: %s", event_data->msg_id, cJSON_Print(msg));

    // get color for LED
    const cJSON *color = cJSON_GetObjectItemCaseSensitive(msg, "color");

    // what is the new color?
    if (cJSON_IsNumber(color))
    {
        // change color of LED
        ESP_RETURN_ON_ERROR(led_on((uint32_t)color->valueint), TAG, "Error setting LED");
    }

    // clean up
    cJSON_Delete(msg);
    return ESP_OK;
}