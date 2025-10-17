// Routines for MQTT communication

#pragma once

#include <esp_err.h>

#include "NFCScanner.h"

/*
 * @brief Initialize MQTT client and connect to broker
 *
*/ 
esp_err_t mqtt_init();

/*
 * @brief Publish an NFC tag to MQTT broker
 */
esp_err_t mqtt_publish_tag(NFCTag tag);