| Supported Targets | ESP32 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- |

# EE 419 Lab 5

In this lab, the ESP32 will communicate with an MQTT broker running on AWS, implementing
the Candle prop for the VMI ECE Escape Room. On boot, the ESP32 will register with the AWS
broker, who responds with an initial count value and an NFC tag to search for. The ESP32 
will flash the count pattern on the RGB LED. If the desired tag is detected, the RGB LED 
is set to green. If any other tag, or no tag at all, is detected, the LED is set to red.

## Configure the Project

Open the project configuration menu (`idf.py menuconfig`).

In the `EE 419 Configuration` menu:

* Confirm the GPIO numbers used for the red, green, and blue LEDs
* Confirm the GPIO numbers for the SPI communication with the PN532
* Set the WiFi SSID and password
* Set the AWS enpoint

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

