# ESP Weather Station

ESP32 project which gets weather data using Open-Meteo API and displays it on an OLED display alongside the location, small icon representing the weather and current time.

## Features

- upon boot, ESP tries to connect to the provided WiFi and set up local time to current time in CET timezone
- displays current location, time and weather with updates every minute
- upon WiFi disconnecting alerts the user and tries to reconnnect
- if an HTTP error occured it is also displayed to the user

![image](docs/esp_weather.png)


![image](docs/wroover.png)

## How it works

### Main program

Main program consists of two FreeRTOS task. `watchdogTask` is used for monitoring WiFi connection and in the event of WiFi disconnecting attempts reconnecting while informing the user. `displayTask` is responsible for fetching the weather data and displaying it to the user.

Since both tasks need to interact with display and WiFi it is important to make them syncronised so that they don't access the same instance at the same time. For synchronisation the **FreeRTOS task notifications** are used.

### SSD1306

Project uses a custom SSD1306 driver with custom I2C communication. The driver is quite barebones allowing the user to print a string at a desired line and a bitmap at a desired position. If you wish to display more complex things like animations, specific text positioning or extended font it is recommended to use an external library.

### Network

Project connects to your WiFi network and performs the following:
- sets up local time using SNTP 

> [!IMPORTANT]
> Project uses CET timezone, if you with to change it you can do so in `wifi.cpp` in `setCurrentTime` function.

- gets coordinates for the input location using [Open Meteo Geocoding API](https://open-meteo.com/en/docs/geocoding-api)
- gets weather information for your location using [Open Meteo Forecast API](https://open-meteo.com/en/docs$0)

## Prerequisites

### Hardware

- ESP32
  - any board with WiFi support should work but the project was tested using [ESP32-C6-MINI-1](https://soldered.com/products/nula-mini-esp32-c6?srsltid=AfmBOooTO54ZQxV41nlYx_qWr5AFgaa8gkWDSMmL5zmYyqjJGM4XuMaw$0) and ESP32-WROVER-E boards
- [SSD1306 display](https://soldered.com/products/display-oled-i2c-white-0-96-ssd1306?variant=62540973343069$0)
  - if you wish to display data using OLED display, alternatively, you can use the serial monitor instead of the display
  - both the [QWIIC cable](https://docs.soldered.com/qwiic) and regular wiring are supported, see [wiring](#wiring) for deatils
  - current wiring is like so:

| ESP32 | SSD1306 |
|-------|---------|
| IO6   | SDA     |
| IO7   | SCL     |
| 3.3V  | 3.3V    |
| GND   | GND     |

### Software

- ESP-IDF v5.0 or above
  - for installation instructions check out the [official documentation](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html#installation)

## Installation

### Wiring

If using QWIIC cable simply connect the ESP and OLED display with it. Otherwise, refer to your ESP documentation and wire accordingly.

### Building and running the project

If not already position yourself in `src` folder using:
```shell
cd src
```

Set your ESP target using:
```shell
idf.py set-target
# eg
# idf.py set-target esp32c6
```

Before flashing the firmware, configure the project using:

```shell
idf.py menuconfig
```

Here you need to configure three things:

1. **I2C Configuration**  
   - If you are using the same setup as described above (ESP32-C6-MINI-1 with QWIIC), you can leave the defaults. Otherwise, set the pins according to your wiring.

2. **WiFi Configuration**  
   - Enter the SSID and password of your network.

3. **Weather Location**  
   - Set the desired location to display weather for. 

> [!IMPORTANT]
> Input the location in ALL CAPS as the current SSD1306 driver only supports capital letters!

After configuration, build the project:
```shell
idf.py build
```
Flash the firmware:
```shell
idf.py -p PORT flash
```

> [!TIP]
> If you don't know which port your ESP is connected to refer to [official documentation](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/establish-serial-connection.html).

And if you wish you can open the serial monitor:
```shell
idf.py -p PORT monitor
```
Alternatively, you can run this one command to perform everything:
```shell
idf.py -p PORT build flash monitor
```
