# Product Requirements Document: The Scout - Phase 1

## 1. Phase Goal: Implement Addressable LED & Buzzer Feedback

The primary goal of this phase is to create robust, non-blocking software modules to control the onboard **WS2812B addressable LEDs** and the passive buzzer. These modules will provide the foundational, and now configurable, user feedback mechanisms required for all subsequent development phases.

## 2. Core Features: Feedback Controller

### User Story
**As a developer**, I need a reusable, non-blocking class to control addressable RGB LEDs and the buzzer, so I can easily provide clear, color-coded status feedback to the user, with options to adjust brightness or disable feedback entirely.

### Acceptance Criteria

#### A. Global Feedback Control
1.  **Stealth Mode**: The `FeedbackController` class must have a "stealth mode" feature.
    * Implement a method `void setStealthMode(bool enabled)`.
    * When stealth mode is enabled, **all** methods that produce visual (LED) or audible (buzzer) feedback must do nothing and return immediately.
    * This provides a global "mute" for all feedback.
2.  **Brightness Control**: The controller must allow for global brightness adjustment of the LEDs.
    * Implement a method `void setBrightness(uint8_t brightness)` where `0` is off and `255` is maximum brightness.
    * This method should be a wrapper around the `Adafruit_NeoPixel` library's own `setBrightness()` function.

#### B. Addressable LED Control
1.  **Hardware & Library**: The controller must manage a chain of **two (2) WS2812B LEDs** connected to a single data pin. It **must** use the `Adafruit NeoPixel` library for this functionality.
2.  **Pixel Mapping**:
    * Pixel `0` will function as the **System LED**.
    * Pixel `1` will function as the **Activity LED**.
3.  **Non-Blocking Operation**: All pattern-based methods (`blink`, `pulse`) must be non-blocking. The class must have an `update()` method that is called in the main `loop()` to manage timing and animations.
4.  **Required Methods**:
    * `void setPixelColor(uint8_t pixelIndex, uint8_t r, uint8_t g, uint8_t b)`: Set a specific pixel to a solid color.
    * `void blinkPixel(uint8_t pixelIndex, uint32_t color, uint16_t interval)`: Blink a specific pixel with a specific color and interval.
    * `void pulsePixel(uint8_t pixelIndex, uint32_t color, uint16_t speed)`: Fade a specific pixel on and off smoothly with a specific color.
    * `void turnOff()`: Turn all LEDs off.

#### C. Buzzer Control
1.  **Tone Generation via PWM**: To make the passive buzzer produce sound, it **must be driven by a PWM signal**. The controller must use the ESP32's `ledc` peripheral to generate PWM signals of specific frequencies on the buzzer pin (`IO40`).
2.  **Required Methods**:
    * `void playSuccessChime()`: Play a pleasant, ascending two-note chime.
    * `void playFailureTone()`: Play a low, descending two-note tone.
    * `void playResetChime()`: Play a short, single confirmation beep.

---

## 3. Technical Requirements & Implementation Strategy

### Instructions for Copilot:
1.  **Strategize First**: Outline your plan, e.g., "I will create a `FeedbackController` class. It will contain a boolean `stealthMode` flag. All public methods for LEDs and the buzzer will first check this flag. The class will also wrap the `setBrightness` method of the `Adafruit_NeoPixel` object."
2.  **Use Specified Library**: You **must** use the `Adafruit NeoPixel` library (PlatformIO ID: `adafruit/Adafruit NeoPixel`) to control the LEDs.
3.  **Code Structure**: Follow the modular folder structure defined in `Master_PRD.md`.
4.  **Testing**: Create a `main.cpp` that demonstrates the new features. For example, set the brightness to half (`128`), make the System LED pulse blue, then enable stealth mode and confirm all feedback stops.
5.  **Verification**: After generating the code, you must verify it by running the command `pio run`. The build **must** succeed without any errors. The final test is to physically see the LEDs and hear the buzzer respond to the new brightness and stealth mode controls.