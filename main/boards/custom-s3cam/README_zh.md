# Custom S3 Cam Board (自定义 S3 摄像头开发板)

此目录包含了基于 ESP32-S3 并带有摄像头和屏幕的 XiaoZhi 语音助手自定义开发板的配置和实现代码。

## 主要特性

* **主控芯片:** ESP32-S3 (16MB Flash, 需支持 PSRAM)
* **音频:** 单工 I2S 音频 (麦克风和扬声器)
* **显示屏:** 128x160 ST7735 SPI LCD 屏幕
* **摄像头:** 支持 (例如 OV2640/OV5640)
* **状态指示灯:** 预留配置
* **按键:** Boot 按键用于配置 Wi-Fi 和切换对话状态

## 引脚配置

### 音频 (I2S 单工)
* **麦克风:** SCK: `GPIO40`, WS: `GPIO39`, DIN: `GPIO41`
* **扬声器:** BCLK: `GPIO2`, LRCK: `GPIO21`, DOUT: `GPIO47`

### 摄像头
* **数据引脚 (D0-D7):** `GPIO11`, `GPIO9`, `GPIO8`, `GPIO10`, `GPIO12`, `GPIO18`, `GPIO17`, `GPIO16`
* **XCLK:** `GPIO15`
* **PCLK:** `GPIO13`
* **VSYNC:** `GPIO6`
* **HREF:** `GPIO7`
* **SDA (SIOD):** `GPIO4`
* **SCL (SIOC):** `GPIO5`

### 显示屏 (ST7735)
* **MOSI:** `GPIO43`
* **CLK:** `GPIO44`
* **CS:** `GPIO45`
* **DC:** `GPIO38`
* **RST:** `GPIO1`
* **背光 (Backlight):** `GPIO42`

### 其他
* **Boot 按键:** `GPIO0`
* **板载 LED:** `GPIO48` (NeoPixel, 可通过 MCP `self.neopixel.*` 控制)
* **MCP 扩展测试灯:** `GPIO14` (外部 LED/继电器, 可通过 MCP `self.lamp.*` 控制)

## MCP 对话控制
此开发板暴露了两个 MCP 功能，允许你通过语音助手直接控制硬件：
1. **NeoPixel (GPIO48):** "把你的灯变成红色"、"闪烁你的灯 3 次"。
   * **关于手动模式的说明:** 当你使用语音助手更改 NeoPixel 的颜色时，它将进入**手动模式**，以防止设备正常的状态指示（例如说话时变绿）覆盖你的指令。要将 LED 恢复为正常的自动状态指示，请对语音助手说："重置你的 LED 为自动模式"（这会调用 `self.neopixel.reset_mode`）。
2. **测试灯/台灯 (GPIO14):** "打开台灯"、"台灯现在是开着的吗？"。

## 如何编译

要为该开发板编译固件，可以使用发布脚本：

```bash
python3 scripts/release.py main/boards/custom-s3cam --name custom-s3cam
```

## 补充说明

* 摄像头帧缓冲区分配在 PSRAM 中，因此必须使用带有 PSRAM 的 ESP32-S3 模组。
* 显示屏使用 `SPI3_HOST` 以实现快速渲染。
