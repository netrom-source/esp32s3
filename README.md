# ESP32-S3 Rotary Writer

Minimal text editor for the ESP32-S3 running in [Wokwi](https://wokwi.com/).
The project uses an SSD1306 OLED for output and a KY-040 rotary encoder for
navigation. Text is stored on the internal flash using SPIFFS.

## Hardware

Connections are defined in `diagram.json`:

| Peripheral | Pins |
|------------|------|
| SSD1306 I2C | SDA=GPIO8, SCL=GPIO9 |
| KY-040 encoder | CLK=GPIO4, DT=GPIO5, SW=GPIO6 (with internal pull-ups) |
| microSD (SPI) | CS=GPIO10, MOSI=GPIO11, MISO=GPIO12, SCK=GPIO13 |

The SD card is present only to satisfy the wiring requirements; saving uses
SPIFFS (`/spiffs/note.txt`).

## Features

* Status bar with file name, cursor position and dirty/Hemingway indicators
* Rotary encoder rotation scrolls text; short press toggles cursor mode
* Long press opens the menu: **New**, **Open** (loads dummy text), **Save**,
  **Settings** (font size + Hemingway mode) and **Exit**
* Hemingway mode disables Backspace
* Autosave every 30 seconds

Text input is provided through the serial monitor.

## Build / Run

```sh
idf.py build
```

Open the project in Wokwi and press **Run**. The editor appears on the OLED and
is controlled with the encoder.

