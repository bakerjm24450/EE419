/*
* Routines to control an RGB LED
*/

#pragma once

#include <stdint.h>
#include <esp_err.h>

// Standard colors
extern const uint32_t BLACK;
extern const uint32_t BLUE;
extern const uint32_t CYAN;
extern const uint32_t GREEN;
extern const uint32_t MAGENTA;
extern const uint32_t RED;
extern const uint32_t YELLOW;
extern const uint32_t WHITE;

// Initialize the RGB LED
// Returns ESP_OK if successful, error code otherwise
esp_err_t led_config();

// Turn the LED on with specified color
// color = RGB value (8 bits each for red, green, blue. Upper 8-bits unused)
// Returns ESP_OK if successful, error code otherwise
esp_err_t led_on(uint32_t color);

// Turn the LED off
// Returns ESP_OK if successful, error code otherwise
esp_err_t led_off();
