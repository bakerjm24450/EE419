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
    // configure the ledc timers
    ledc_timer_config_t timer_config = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .timer_num = LEDC_TIMER_0, // red uses timer 0
        .freq_hz = 4000,
        .clk_cfg = LEDC_AUTO_CLK,
        .deconfigure = false};
    ESP_ERROR_CHECK(ledc_timer_config(&timer_config));

    timer_config.timer_num = LEDC_TIMER_1; // green uses timer 1
    ESP_ERROR_CHECK(ledc_timer_config(&timer_config));

    timer_config.timer_num = LEDC_TIMER_2; // blue uses timer 2
    ESP_ERROR_CHECK(ledc_timer_config(&timer_config));

    // configure the PWM channels
    ledc_channel_config_t channel_config = {
        .gpio_num = RED_PIN, // gpio pin
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0, // red
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0, // red
        .duty = 0,                 // color value
        .hpoint = 0,
        .sleep_mode = LEDC_SLEEP_MODE_NO_ALIVE_NO_PD,
        .flags = {0}};
    ESP_ERROR_CHECK(ledc_channel_config(&channel_config));

    // green
    channel_config.gpio_num = GREEN_PIN;
    channel_config.channel = LEDC_CHANNEL_1;
    channel_config.timer_sel = LEDC_TIMER_1;
    ESP_ERROR_CHECK(ledc_channel_config(&channel_config));

    // blue
    channel_config.gpio_num = BLUE_PIN;
    channel_config.channel = LEDC_CHANNEL_2;
    channel_config.timer_sel = LEDC_TIMER_2;
    ESP_ERROR_CHECK(ledc_channel_config(&channel_config));

    ESP_LOGD(TAG, "RGBLED configured");
}

// Turn the LED on with specified color
// color = RGB value (8 bits each for red, green, blue. Upper 8-bits unused)
void led_on(uint32_t color)
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;

    // figure out rgb values
    red = (color >> 16) & 0xff;
    green = (color >> 8) & 0xff;
    blue = color & 0xff;

    // set duty cycles for all three leds based on colors
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, red));
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, green));
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, blue));

    // update for changes to take effect
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2));

    ESP_LOGD(TAG, "RGBLED color (%x, %x, %x)", (int) red, (int) green, (int) blue);
}

// Turn the LED off
void led_off()
{
    // off is same as setting color to (0,0,0)
    led_on(0);
}