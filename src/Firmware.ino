/*
  DistrictCon Year 1 Badge: Hackable Firmware
  
  This is a minimal firmware that demonstrates how to use the badge hardware:
  - 240x240 GC9A01 round display
  - 12x NeoPixel RGB LEDs
  - 6 buttons (Up, Down, Left, Right, A, B)
  - Microphone (analog input)
  - IR receiver
  
  Build with PlatformIO:
    pio run -e discoone --target upload
    pio device monitor

  If building in the Arduino IDE, make sure to install the following libraries:
  - Adafruit NeoPixel by Adafruit
  - ButtonFever by mickey9801
  - IRremote by z3t0
  - LovyanGFX by lovyan03
*/

#include <Arduino.h>
#include "pins.h"
#include "LGFX_config.hpp"
#include <stdint.h>

// Button handling library
#include <BfButton.h>

// IR library
#define IR_RECEIVE_PIN PIN_IR_RECV
#define NO_LED_FEEDBACK_CODE 1
#define SEND_PWM_BY_TIMER
#include <IRremote.hpp>

// Logo image
#include "disco_logo_white.h"

// Hardware Objects
LGFX lcd;
LGFX_Sprite screen_buffer(&lcd);

#include <Adafruit_NeoPixel.h>
#define NUMPIXELS 12
Adafruit_NeoPixel leds(NUMPIXELS, PIN_LEDS, NEO_GRB + NEO_KHZ800);

// Button Objects
BfButton btn_up(BfButton::STANDALONE_DIGITAL, BTN_UP, true, LOW);
BfButton btn_down(BfButton::STANDALONE_DIGITAL, BTN_DOWN, true, LOW);
BfButton btn_left(BfButton::STANDALONE_DIGITAL, BTN_LEFT, true, LOW);
BfButton btn_right(BfButton::STANDALONE_DIGITAL, BTN_RIGHT, true, LOW);
BfButton btn_a(BfButton::STANDALONE_DIGITAL, BTN_A, true, LOW);
BfButton btn_b(BfButton::STANDALONE_DIGITAL, BTN_B, true, LOW);

// Microphone Variables
uint8_t micLevel = 0;
unsigned long lastMicSample = 0;

// Brightness control
uint8_t lcdBrightness = 100;  // 0-100 (steps of 10)
uint8_t ledBrightness = 32;   // 0-255 (steps of 16)

// LED Rainbow Fade state
uint32_t fadeColor1 = 0xFF0000;  // Red
uint32_t fadeColor2 = 0x00FF00;  // Green
uint16_t fadeStep = 0;
const uint16_t fadeTotalSteps = 32;
const uint16_t fadeDelay = 100;
unsigned long lastLedUpdate = 0;

// Returns the Red component of a 32-bit color
inline uint8_t Red(uint32_t color) {
  return (color >> 16) & 0xFF;
}

// Returns the Green component of a 32-bit color
inline uint8_t Green(uint32_t color) {
  return (color >> 8) & 0xFF;
}

// Returns the Blue component of a 32-bit color
inline uint8_t Blue(uint32_t color) {
  return color & 0xFF;
}

// LED Rainbow Fade Update
void updateLedFade() {
  if (millis() - lastLedUpdate < fadeDelay) {
    return;  // Not time to update yet
  }
  
  lastLedUpdate = millis();
  
  // Calculate interpolated color
  uint8_t red = ((Red(fadeColor1) * (fadeTotalSteps - fadeStep)) + (Red(fadeColor2) * fadeStep)) / fadeTotalSteps;
  uint8_t green = ((Green(fadeColor1) * (fadeTotalSteps - fadeStep)) + (Green(fadeColor2) * fadeStep)) / fadeTotalSteps;
  uint8_t blue = ((Blue(fadeColor1) * (fadeTotalSteps - fadeStep)) + (Blue(fadeColor2) * fadeStep)) / fadeTotalSteps;
  
  // Set all LEDs to the interpolated color
  uint32_t color = leds.Color(red, green, blue);
  for (uint8_t i = 0; i < NUMPIXELS; i++) {
    leds.setPixelColor(i, color);
  }
  leds.show();
  
  // Increment step
  fadeStep++;
  if (fadeStep >= fadeTotalSteps) {
    fadeStep = 0;
    
    // Cycle to next color pair
    if (fadeColor1 == 0xFF0000) {  // Red -> Green
      fadeColor1 = 0x00FF00;  // Green
      fadeColor2 = 0x0000FF;  // Blue
    } else if (fadeColor1 == 0x00FF00) {  // Green -> Blue
      fadeColor1 = 0x0000FF;  // Blue
      fadeColor2 = 0xFF0000;  // Red
    } else {  // Blue -> Red
      fadeColor1 = 0xFF0000;  // Red
      fadeColor2 = 0x00FF00;  // Green
    }
  }
}

// Button Handler
void btn_pressHandler(BfButton* btn, BfButton::press_pattern_t pattern) {
  uint8_t btnId = btn->getID();
  
  if (pattern == BfButton::SINGLE_PRESS) {
    Serial.print("Button pressed: ");
    switch(btnId) {
      case BTN_UP:
        Serial.println("UP");
        lcdBrightness = min(100, lcdBrightness + 10);
        lcd.setBrightness(lcdBrightness);
        Serial.printf("LCD Brightness: %d\n", lcdBrightness);
        break;
        
      case BTN_DOWN:
        Serial.println("DOWN");
        lcdBrightness = (lcdBrightness > 10) ? lcdBrightness - 10 : 0;
        lcd.setBrightness(lcdBrightness);
        Serial.printf("LCD Brightness: %d\n", lcdBrightness);
        break;
        
      case BTN_LEFT:
        Serial.println("LEFT");
        ledBrightness = (ledBrightness > 16) ? ledBrightness - 16 : 0;
        leds.setBrightness(ledBrightness);
        leds.show();
        Serial.printf("LED Brightness: %d\n", ledBrightness);
        break;
        
      case BTN_RIGHT:
        Serial.println("RIGHT");
        ledBrightness = min(255, ledBrightness + 16);
        leds.setBrightness(ledBrightness);
        leds.show();
        Serial.printf("LED Brightness: %d\n", ledBrightness);
        break;
        
      case BTN_A: Serial.println("A"); break;
      case BTN_B: Serial.println("B"); break;
    }
  }
}

// Microphone Sampling
void sampleMicrophone() {
  uint16_t mn = 1024;
  uint16_t mx = 0;

  for (uint16_t i = 0; i < 1000; ++i) {
    uint16_t val = analogRead(PIN_MIC);
    mn = min(mn, val);
    mx = max(mx, val);
  }

  micLevel = mx - mn;
}

// Display Update
void updateDisplay() {
  screen_buffer.fillScreen(0x000000);  // Black background
  
  // Draw DistrictCon logo (PNG embedded as byte array)
  screen_buffer.drawPng(disco_logo_white_png, disco_logo_white_png_len, 120 - 160/2, 30);
  
  // Draw text
  screen_buffer.setTextColor(0xFFFFFF, 0x000000);  // White text with black background
  screen_buffer.setTextDatum(TC_DATUM);
  screen_buffer.drawString("DisCo Year 1", 120, 10);
  screen_buffer.drawString("Press buttons!", 120, 220);
  
  // Draw microphone level bar
  screen_buffer.drawString("Mic Level:", 120, 200);
  int barWidth = map(micLevel, 0, 255, 0, 100);
  screen_buffer.fillRect(70, 210, barWidth, 5, 0x07E0);  // Green bar
  
  // Push to display
  screen_buffer.pushSprite(0, 0);
  lcd.display();
}

// Setup (Core 0)
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n=== DistrictCon Year 1 ===");
  Serial.println("Example Badge Code (DIY Build)");
  
  // Initialize display
  lcd.init();
  lcd.setBrightness(lcdBrightness);
  lcd.setRotation(0);
  lcd.clear();
  
  // Create sprite buffer
  screen_buffer.createSprite(240, 240);
  screen_buffer.setColorDepth(16);
  
  Serial.println("Setup complete!");
}

// Setup (Core 1)
void setup1() {
  // Initialize LEDs
  leds.begin();
  leds.setBrightness(ledBrightness);
  leds.clear();
  leds.show();

  // Initialize buttons
  btn_up.onPress(btn_pressHandler);
  btn_down.onPress(btn_pressHandler);
  btn_left.onPress(btn_pressHandler);
  btn_right.onPress(btn_pressHandler);
  btn_a.onPress(btn_pressHandler);
  btn_b.onPress(btn_pressHandler);
  
  // Initialize IR receiver
  IrReceiver.begin(PIN_IR_RECV, false);
}

// Main Loop (Core 0)
void loop() {
  // Update display
  updateDisplay();
  
  delay(20);  // ~50 FPS
}

// Main Loop (Core 1)
void loop1() {
  // Update LED animation
  updateLedFade();
  
  // Update buttons
  btn_up.read();
  btn_down.read();
  btn_left.read();
  btn_right.read();
  btn_a.read();
  btn_b.read();
  
  // Sample microphone periodically
  if (millis() - lastMicSample > 100) {
    sampleMicrophone();
    lastMicSample = millis();
  }
  
  // Check for IR signals
  if (IrReceiver.decode()) {
    Serial.print("IR received: Protocol=");
    Serial.print(IrReceiver.decodedIRData.protocol);
    Serial.print(" Address=0x");
    Serial.print(IrReceiver.decodedIRData.address, HEX);
    Serial.print(" Command=0x");
    Serial.println(IrReceiver.decodedIRData.command, HEX);
    IrReceiver.resume();
  }
}
