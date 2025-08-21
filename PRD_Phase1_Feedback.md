# Product Requirements Document: The Scout - Phase 1 (Modular)

## 1. Phase Goal: Implement Modular Feedback System

The primary goal of this phase is to create a clean, modular feedback system composed of three classes:
1.  **`LedController`**: Manages the WS2812B addressable LEDs.
2.  **`BuzzerController`**: Manages the passive buzzer.
3.  **`FeedbackManager`**: A top-level manager that orchestrates the two controllers and handles persistent settings.

## 2. Core Features & Architecture

### User Story
**As a developer**, I need a clean, modular system to control LEDs and the buzzer independently, with global settings for brightness and stealth mode that persist across reboots.

### Acceptance Criteria

#### A. `LedController` Class (`feedback/LedController.h` & `.cpp`)
This class is solely responsible for the LEDs.

1.  **Hardware & Library**: Manages **two (2) WS2812B LEDs** and **must** use the `Adafruit NeoPixel` library.
2.  **Color Palette & Pixels**: The header file must define the standard color palette (`GUARDIAN_GREEN`, etc.) and pixel names (`PIXEL_SYSTEM`, `PIXEL_ACTIVITY`).
3.  **Non-Blocking Operation**: All pattern methods must be non-blocking, managed by an `update()` method.
4.  **Required Methods**:
    * `void begin()`: Initializes the NeoPixel object.
    * `void update()`: Must be called in the main loop to drive animations.
    * `void setBrightness(uint8_t brightness)`: Sets the LED brightness.
    * `void setStealthMode(bool enabled)`: A flag to disable all LED output.
    * `void setPixelColor(uint8_t pixelIndex, uint32_t color)`
    * `void blinkPixel(uint8_t pixelIndex, uint32_t color, uint16_t interval)`
    * `void pulsePixel(uint8_t pixelIndex, uint32_t color, uint16_t speed)`
    * `void turnOff()`

#### B. `BuzzerController` Class (`feedback/BuzzerController.h` & `.cpp`)
This class is solely responsible for the buzzer.

1.  **Tone Generation via PWM**: **Must** use the ESP32's `ledc` peripheral to drive the passive buzzer on pin `IO40`.
2.  **Non-Blocking Operation**: Playing chimes must be non-blocking, managed by an `update()` method.
3.  **Required Methods**:
    * `void begin()`: Initializes the ledc channel.
    * `void update()`: Must be called in the main loop to handle note durations.
    * `void setStealthMode(bool enabled)`: A flag to disable all buzzer output.
    * `void playSuccessChime()`
    * `void playFailureTone()`
    * `void playResetChime()`

#### C. `FeedbackManager` Class (`feedback/FeedbackManager.h` & `.cpp`)
This is the main public-facing class that the rest of the application will interact with.

1.  **Composition**: It must contain private instances of `LedController` and `BuzzerController`.
2.  **Persistent Settings**: It is responsible for saving and loading settings.
    * **`loadSettings()`**: Called in `begin()`, this method loads brightness and stealth mode from the `Preferences` library and passes them to the `LedController` and `BuzzerController`.
    * **`saveSettings()`**: Called whenever a setting is changed, this method saves the current brightness and stealth mode to `Preferences`.
3.  **Global Control Methods**:
    * `void begin()`: Calls `begin()` on both the Led and Buzzer controllers.
    * `void update()`: Calls `update()` on both the Led and Buzzer controllers.
    * `void setBrightness(uint8_t brightness)`: Calls the corresponding method on the `LedController` and then saves the setting.
    * `void setStealthMode(bool enabled)`: Calls the corresponding method on both controllers and then saves the setting.
4.  **Delegated Methods**: It must provide public methods that simply call the corresponding methods on the `LedController` and `BuzzerController` instances (e.g., `manager.pulsePixel(...)` would call `ledController.pulsePixel(...)`).

---

## 3. Technical Requirements & Implementation Strategy

### Instructions for Copilot:
1.  **Strategize First**: Outline your plan, e.g., "I will create three classes. The `FeedbackManager` will be the main entry point. It will hold instances of the other two controllers and manage saving/loading settings. The main `loop()` will only need to call `feedbackManager.update()`."
2.  **Use Specified Libraries**: You **must** use `Adafruit NeoPixel` for LEDs and `Preferences` for storage.
3.  **Code Structure**: Create three separate pairs of `.h` and `.cpp` files inside the `feedback/` directory as described.
4.  **Testing**: In `main.cpp`, create a single instance of `FeedbackManager`. Use it to test the functionality, such as setting brightness, playing a chime, and then enabling stealth mode to confirm all feedback stops.
5.  **Verification**: After generating the code, you must verify it by running the command `pio run`. The build **must** succeed without any errors. The final test is to confirm all settings persist after a power cycle.