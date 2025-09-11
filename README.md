# ESP32-S3 Hemingway Editor

This project contains a minimal text editor for the ESP32-S3 using the ESP-IDF.
It is designed to run inside the [Wokwi](https://wokwi.com/) simulator and is
prepared for extension with USB-host keyboard support on real hardware.

## Features

* Start screen offering **New file** and stub **Open file** options.
* Text editor that accepts keyboard input from the serial console (and a
  placeholder USB-host driver).
* **Hemingway mode** – when enabled backspace is disabled.
* Manual save with `Ctrl+S` and automatic save every 30 seconds.
* Text is stored as `.txt` files on a FAT formatted SD card using the IDF's
  FATFS component. Filenames are generated sequentially.
* Simple display abstraction printing the text to the UART console. It can be
  replaced by a real OLED (SSD1306) driver when running on hardware.

## Building

```sh
idf.py build
```

Run the firmware in Wokwi by opening this folder in Visual Studio Code with the
Wokwi extension installed. The provided `diagram.json` and `wokwi.toml` files
instantiate an ESP32-S3, an SSD1306 display and an SD card.

## Controls

* `N` / `n` – create a new file.
* `Ctrl+S` – save current text.
* `Ctrl+E` – toggle Hemingway mode.

## USB Host

The project contains a stub (`usb_host_stub.c`) where USB keyboard handling can
be implemented. Wokwi does not currently emulate a USB host controller, so the
stub simply returns no input.
