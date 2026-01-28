# DistrictCon Year 1 Badge - Hackable Firmware

A minimal, hackable firmware framework for the DistrictCon 2026 badge hardware. This provides basic access to all hardware components.

## Hardware Features

- **Microcontroller**: RP2350
- **Display**: 240x240 GC9A01 round LCD
- **LEDs**: 12x WS2812B NeoPixel RGB LEDs
- **Buttons**: 6 tactile buttons (Up, Down, Left, Right, A, B)
- **Microphone**: Analog MEMS microphone
- **IR**: Infrared receiver

## Pin Mapping

|  Component  |   Pin   |
|-------------|---------|
| LCD DC      | GPIO 0  |
| LCD BL      | GPIO 1  |
| LCD SCL     | GPIO 2  |
| LCD SDA     | GPIO 3  |
| LCD RST     | GPIO 4  |
| LCD CS      | GPIO 5  |
| BTN B       | GPIO 6  |
| BTN A       | GPIO 7  |
| SAO I2C SDA | GPIO 8  |
| SAO I2C SCL | GPIO 9  |
| SAO GPIO1   | GPIO 10 |
| SAO GPIO2   | GPIO 11 |
| IR Receiver | GPIO 16 |
| Sniffy      | GPIO 19 |
| BTN Right   | GPIO 20 |
| BTN Down    | GPIO 21 |
| BTN Up      | GPIO 22 |
| BTN Left    | GPIO 23 |
| NeoPixels   | GPIO 24 |
| Mic         | GPIO 26 (A1) |
| Divider 1   | GPIO 28 |
| Divider 2   | GPIO 29 |

## Getting Started

### Prerequisites

- [PlatformIO](https://platformio.org/) installed
- USB-C cable to connect the badge

### Building

```bash
pio run -e discoone
```

### Flashing

If there is a different firmware, or no firmware on the badge, then:

1. Hold the DFU button on the badge
2. Connect USB cable
3. Release DFU (badge appears as USB drive)

Otherwise, just

4. Run:

```bash
pio run -e discoone --target upload
```

Or manually copy `.pio/build/discoone/firmware.uf2` to the USB drive.

## Code Structure

```
DistrictConYear1Badge/
├── platformio.ini         # PlatformIO configuration
├── src/
│   ├── Firmware.ino       # Main entry point (Arduino IDE Compatible)
│   ├── pins.h             # Pin definitions
│   ├── LGFX_config.hpp    # Display configuration
│   └── disco_logo_white.h # DistrictCon logo
```

## Example Usage

The included `Firmware.ino` demonstrates:

- **Display**: Drawing the DistrictCon logo and text using LovyanGFX
- **LEDs**: Rainbow fade animation cycling through RGB colors
- **Buttons**: Interactive brightness control and debug output
- **Microphone**: Sampling audio level and displaying as a bar graph
- **IR Receiver**: Detecting and printing IR signals to serial
- **Dual Core**: Display rendering on Core 0, LED/button handling on Core 1

### Interactive Controls

- **Up/Down buttons**: Adjust LCD brightness (0-100 in steps of 10)
- **Left/Right buttons**: Adjust NeoPixel brightness (0-255 in steps of 16)
- **A/B buttons**: Available for custom functionality

## Libraries Used in this

- **LovyanGFX**: Display driver ([GitHub](https://github.com/lovyan03/LovyanGFX))
- **Adafruit NeoPixel**: LED control ([GitHub](https://github.com/adafruit/Adafruit_NeoPixel))
- **ButtonFever**: Button debouncing ([GitHub](https://github.com/mickey9801/ButtonFever))
- **IRremote**: IR communication ([GitHub](https://github.com/z3t0/IRremote))

## Hardware API Reference

### Display (LovyanGFX)

```cpp
lcd.fillScreen(color);                    // Fill with color
lcd.drawString("text", x, y);             // Draw text
lcd.drawPng(data, size, x,y);             // Draw PNG image
screen_buffer.pushSprite(x, y);           // Push sprite to display
```

### NeoPixels

```cpp
leds.setPixelColor(index, color);         // Set single LED
leds.show();                              // Update LEDs
leds.Color(r, g, b);                      // Create color value (0-255)
leds.setBrightness(brightness);           // Set overall brightness (0-255)
```

### Buttons

```cpp
btn_up.onPress(handler);                  // Register press handler
btn_up.read();                            // Poll button (call in loop())
```

### Microphone

```cpp
uint16_t val = analogRead(PIN_MIC);       // Read microphone (0-1023)
```

### IR Receiver

```cpp
if (IrReceiver.decode()) {
  uint32_t cmd = IrReceiver.decodedIRData.command;  // Get command
  IrReceiver.resume();                              // Ready for next
}
```

## License

This framework is provided as a starting point for hacking on the DistrictCon badge hardware. Use it however you like!
