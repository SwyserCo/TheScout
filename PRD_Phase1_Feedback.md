# Product Requirements Document: The Scout - Phase 1

## 1. Phase Goal: Implement Modular Feedback System

The primary goal of this phase is to create a clean, modular feedback system composed of three classes:
1.  **`LedController`**: Manages the WS2812B addressable LEDs and the simple charging status LED.
2.  **`BuzzerController`**: Manages the passive buzzer.
3.  **`FeedbackManager`**: A top-level manager that orchestrates the two controllers and handles persistent settings.

## 2. Core Features & Architecture

### User Story
**As a developer**, I need a clean, modular system to control LEDs and the buzzer independently, with global settings for brightness and stealth mode that persist across reboots.

### Acceptance Criteria

#### A. `LedController` Class (`feedback/LedController.h` & `.cpp`)
1.  **Hardware & Library**: Manages a chain of **two (2) WS2812B LEDs** on `IO03` and `IO45` and one **simple LED** on `IO42`. It **must** use the `Adafruit NeoPixel` library for the WS2812B LEDs.
2.  **Color Palette & Pixels**: The header file must define the standard color palette (`HEARTHGUARD_GREEN`, etc.) and pixel names (`PIXEL_SYSTEM`, `PIXEL_ACTIVITY`).
3.  **Non-Blocking Operation**: All pattern methods must be non-blocking, managed by an `update()` method.
4.  **Required Methods**:
    * `void begin()`: Initializes the NeoPixel object and sets up the simple LED pin.
    * `void update()`: Must be called in the main loop to drive animations.
    * `void setBrightness(uint8_t brightness)`: Sets the WS2812B brightness.
    * `void setStealthMode(bool enabled)`: A flag to disable all LED output.
    * `void setPixelColor(uint8_t pixelIndex, uint32_t color)`
    * `void blinkPixel(uint8_t pixelIndex, uint32_t color, uint16_t interval)`
    * `void setChargeLed(bool on)`: Controls the simple charging LED on IO42.

#### B. `BuzzerController` Class (`feedback/BuzzerController.h` & `.cpp`)
1.  **Tone Generation via PWM**: **Must** use the ESP32's `ledc` peripheral to drive the passive buzzer on pin `IO41`.
2.  **Thematic Sound Events**:
    | Event Name          | Thematic Sound Description                             |
    | ------------------- | ------------------------------------------------------ |
    | `playFanfare()`     | A short, bright, two-note trumpet fanfare for success. |
    | `playHornBlast()`   | A single, low, resonant horn blast for failure.        |
    | `playLutePluck()`   | A single, clear note for confirmation (reset).         |
    | `playClink()`       | A very short, metallic "clink" for interaction.        |
3.  **Required Methods**: `begin()`, `update()`, `setStealthMode(bool enabled)`, and the four sound event methods above.

#### C. `FeedbackManager` Class (`feedback/FeedbackManager.h` & `.cpp`)
1.  **Composition**: It must contain private instances of `LedController` and `BuzzerController`.
2.  **Persistent Settings**: It is responsible for saving and loading `brightness` and `stealthMode` settings using the `Preferences` library.
3.  **Global Control & Delegation**: It must provide `begin()`, `update()`, `setBrightness()`, and `setStealthMode()` methods, and delegate all specific LED and buzzer calls to its internal controller instances.

---

## 3. Technical Requirements & Implementation Strategy

### Instructions for Copilot:
1.  **Adhere to SRP**: Follow the Single Responsibility Principle. The three classes created must each have a single, clear responsibility.
2.  **Use Specified Libraries**: You **must** use `Adafruit NeoPixel` for LEDs and `Preferences` for storage.
3.  **Code Structure**: Create three separate pairs of `.h` and `.cpp` files inside the `feedback/` directory.
4.  **Verification**: After generating the code, run `pio run`. The build **must** succeed.
