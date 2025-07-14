#pragma once
#include <Adafruit_NeoPixel.h>

class StatusLEDController {
public:
  StatusLEDController(uint8_t systemLEDPin, uint8_t activityLEDPin);
  void begin();
  void flashSystemRed(int times);
  void flashSystemGreen(int times);
  void setSystemColor(uint8_t r, uint8_t g, uint8_t b);
  void clearSystem();

private:
  Adafruit_NeoPixel systemLED;
  Adafruit_NeoPixel activityLED;
};
