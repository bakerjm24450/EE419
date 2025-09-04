| Supported Targets | ESP32 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- |

# EE 419 Lab 1

For this lab, the RGB LED will cycle through a number of different colors. You will
need to supply the implementations of the functions in RGBLED.c.

## Configure the Project

First, set the target to match the ESP32 variant that you are using (ESP32, ESP32S3, or ESP32S2).

Next, open the project configuration menu (`idf.py menuconfig`).

In the `EE 419 Configuration` menu:

* Confirm that the GPIO numbers used for the red, green, and blue LEDs match your system
* Set the WiFi SSID and password

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

As you run the example, you will see the RGB LED change colors, and the monitor will display
messages similar to:

```text
I (8146) main: 
Time: Thu Sep  4 15:40:36 2025

I (8147) main: White
I (8647) main: Red
I (9147) main: Green
I (9647) main: Blue
I (10147) main: Magenta
I (10647) main: Cyan
I (11147) main: Yellow
I (11647) main: Off
I (12147) main: 
Time: Thu Sep  4 15:40:40 2025

I (12148) main: White
I (12648) main: Red
I (13149) main: Green
```
