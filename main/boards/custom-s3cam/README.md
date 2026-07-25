# Custom S3 Cam Board (custom-s3cam)

This directory contains the board configuration and implementation for a custom ESP32-S3 development board with camera and display support for the XiaoZhi voice assistant.

## Features

* **Chip:** ESP32-S3 (16MB Flash)
* **Audio:** Simplex I2S audio (Mic and Speaker)
* **Display:** 128x160 ST7735 SPI LCD
* **Camera:** Supported (e.g., OV2640/OV5640)
* **Status LED:** Configured
* **Buttons:** Boot button for Wi-Fi config and chat toggling

## Pinout

### Audio (I2S Simplex)
* **Mic:** SCK: `GPIO40`, WS: `GPIO39`, DIN: `GPIO41`
* **Speaker:** BCLK: `GPIO2`, LRCK: `GPIO21`, DOUT: `GPIO47`

### Camera
* **D0-D7:** `GPIO11`, `GPIO9`, `GPIO8`, `GPIO10`, `GPIO12`, `GPIO18`, `GPIO17`, `GPIO16`
* **XCLK:** `GPIO15`
* **PCLK:** `GPIO13`
* **VSYNC:** `GPIO6`
* **HREF:** `GPIO7`
* **SDA (SIOD):** `GPIO4`
* **SCL (SIOC):** `GPIO5`

### Display (ST7735)
* **SDA:** `GPIO43`
* **SCL:** `GPIO44`
* **CS:** `GPIO45`
* **DC:** `GPIO38`
* **RST:** `GPIO1`
* **Backlight:** `GPIO42`

### Other
* **Boot Button:** `GPIO0`
* **Built-in LED:** `GPIO48` (NeoPixel, controllable via MCP `self.neopixel.*`)
* **MCP Lamp Test:** `GPIO14` (External LED/Relay, controllable via MCP `self.lamp.*`)

## MCP Chatbot Controls
This board comes with two exposed MCP features allowing you to control hardware directly via the chatbot:
1. **NeoPixel (GPIO48):** "Turn your LED red", "Blink your light 3 times".
   * **Note on Manual Mode:** When you use the chatbot to change the NeoPixel color, it enters **Manual Mode** to prevent the normal device status indicators (like turning green when speaking) from overwriting your command. To return the LED to its normal automatic status behavior, ask the chatbot to: "Reset your LED to automatic mode" (this calls `self.neopixel.reset_mode`).
2. **Lamp Test (GPIO14):** "Turn on the lamp", "Is the lamp turned on?".

## How to Build

To build the firmware for this board, you can use the release script:

```bash
python3 scripts/release.py main/boards/custom-s3cam --name custom-s3cam
```

## Additional Notes

* The camera frame buffer is allocated in PSRAM, so an ESP32-S3 module with PSRAM is required.
* The display uses `SPI3_HOST` for fast rendering.
