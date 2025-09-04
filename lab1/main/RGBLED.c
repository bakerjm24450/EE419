/*
 * Routines to control an RGB LED
 */

#include <driver/gpio.h>
#include <driver/ledc.h>
#include <esp_log.h>
#include <esp_err.h>

#include "RGBLED.h"

// for log messages
static const char *TAG = "RGBLED";

// GPIO pins
static const gpio_num_t RED_PIN = CONFIG_LED_RED_PIN;
static const gpio_num_t GREEN_PIN = CONFIG_LED_GREEN_PIN;
static const gpio_num_t BLUE_PIN = CONFIG_LED_BLUE_PIN;

// standard colors
const uint32_t BLACK = 0x00000000;
const uint32_t BLUE = 0x000000ff;
const uint32_t CYAN = 0x0000ffff;
const uint32_t GREEN = 0x0000ff00;
const uint32_t MAGENTA = 0x00ff00ff;
const uint32_t RED = 0x00ff0000;
const uint32_t YELLOW = 0x00ffff00;
const uint32_t WHITE = 0x00ffffff;

// Initialize the RGB LED -- we must configure the three ledc timers
// and the three PWM channels
void led_config()
{
    // FIXME -- complete this function
}

// Turn the LED on with specified color
// color = RGB value (8 bits each for red, green, blue. Upper 8-bits unused)
void led_on(uint32_t color)
{
    // FIXME -- complete this function
}

// Turn the LED off
void led_off()
{
    // FIXME -- complete this function
}