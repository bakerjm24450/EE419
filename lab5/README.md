| Supported Targets | ESP32 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- |

# EE 419 Lab 5

In this lab, the ESP32 will communicate with an MQTT broker running on AWS. Whenever 
an NFC tag is detected by the ESP32, it will publish an MQTT message with the tag ID.
The ESP32 will also receive MQTT messages that contain a color value, which is used
to change the color of the RGB LED.

## Configure the Project

Open the project configuration menu (`idf.py menuconfig`).

In the `EE 419 Configuration` menu:

* Confirm the GPIO numbers used for the red, green, and blue LEDs
* Confirm the GPIO numbers for the I2C or SPI communication with the PN532
* Set the WiFi SSID and password
* Set the AWS endpoint
* Set the AWS Thing name

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

