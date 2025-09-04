| Supported Targets | ESP32 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- |

# EE 419 Lab 2

Building on Lab 1, this lab adds a PN532 NFC scanner, connected via the SPI interface. It also
runs a simple webserver to display some status information about the system.

## Configure the Project

First, set the target to match the ESP32 variant that you are using (ESP32, ESP32S3, or ESP32S2).

Next, open the project configuration menu (`idf.py menuconfig`).

In the `EE 419 Configuration` menu:

* Confirm that the GPIO numbers used for the red, green, and blue LEDs match your system
* Confirm that the pins for the SPI interface match your system
* Set the WiFi SSID and password

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

