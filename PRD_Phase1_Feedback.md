# Product Requirements Document: The Scout - Phase 1 (Comprehensive)

## 1. Phase Goal: Implement Advanced, Modular Feedback System

The primary goal of this phase is to create a clean, modular, and highly configurable feedback system. This involves creating three distinct classes (`LedController`, `BuzzerController`, `FeedbackManager`) that provide a rich set of non-blocking visual and audible cues, with settings that persist across reboots. This module is foundational for all user-facing feedback and must be implemented with the highest level of detail and quality.

## 2. Module Interactions & Dependencies
This phase involves implementing the three core feedback modules. The `FeedbackManager` will depend on the **`Preferences`** library for saving and loading settings. The `LedController` will depend on the **`Adafruit NeoPixel`** library. The `main.cpp` file (created in `PRD_Phase0_Scaffolding.md`) will be updated to use the `FeedbackManager`. All pin definitions **must** be used from the `Pins.h` file, which is populated from **`HARDWARE_SPEC.md`**.

---

## 3. User Stories & Acceptance Criteria

### User Story 1: Device Status Indication
**As a user**, I want clear, intuitive, and aesthetically pleasing visual feedback from the device's LEDs, so I can understand its status at a glance (e.g., "Is it connecting?", "Did it fail?") without it being distracting or ambiguous.

### User Story 2: Audible Cues
**As a user**, I want short, distinct, and thematic audible cues for important events (like success or failure), so I can get confirmation of actions without needing to look at the device.

### User Story 3: Configuration Persistence
**As a user**, I want the device to remember my preferred brightness and stealth mode settings, so I don't have to reconfigure it every time it powers on, making the device feel truly mine.

### Acceptance Criteria

#### A. `BuzzerController` Class (`feedback/BuzzerController.h` & `.cpp`)
* [ ] **Power-On Sound**: The `begin()` method of this class must play a single, short "boot" beep (150ms duration) to confirm the device has powered on.
* [ ] **Thematic Sound Abstraction**: The specific notes and durations for each thematic sound **must** be defined as constants (e.g., in a private struct or array within the `.cpp` file). This is to allow for future "theming" by only changing these definitions, not the core `play...()` logic.
* [ ] **Non-Blocking Playback**: All sound events must be non-blocking. The `update()` method must handle the timing of notes and pauses without using `delay()`.
* [ ] **Required Methods**:
    * `void begin()`: Initializes the `ledc` peripheral and plays the boot sound.
    * `void update()`: Manages non-blocking playback of sound sequences.
    * `void setStealthMode(bool enabled)`: Disables all sound output if true.
    * `void playFanfare()`: Plays a short, bright, two-note trumpet fanfare for success.
    * `void playHornBlast()`: Plays a single, low, resonant horn blast for failure.
    * `void playLutePluck()`: Plays a single, clear note for confirmation (e.g., factory reset).
    * `void playClink()`: Plays a very short, metallic "clink" for acknowledging a button press.

#### B. `LedController` Class (`feedback/LedController.h` & `.cpp`)
* [ ] **Animation Engine**: The `update()` method must drive a non-blocking animation engine. All fade-in/out effects must be smooth (e.g., using a sine-wave calculation) and have a fixed duration of approximately 50ms.
* [ ] **Animation Type Definition**: An `enum class LedAnimation` must be defined in the header to represent the different animation styles: `SOLID`, `SMOOTH_FLASH_FAST`, `SMOOTH_FLASH_SLOW`, `PULSE_FAST`, `PULSE_SLOW`.
* [ ] **Color Palette & Pixels**: The header file must define the standard color palette (`HEARTHGUARD_GREEN`, etc.) and pixel names (`PIXEL_SYSTEM`, `PIXEL_ACTIVITY`).
* [ ] **Required Methods**:
    * `void begin()`: Initializes the NeoPixel object. LEDs should be off by default.
    * `void update()`: Must be called in the main loop to drive all animations.
    * `void setBrightness(uint8_t brightness)`: Sets the global LED brightness.
    * `void setStealthMode(bool enabled)`: A flag to disable all LED output.
    * `void startAnimation(uint8_t pixelIndex, uint32_t color, LedAnimation animation)`: Starts a new animation on a specific pixel. This method must contain the logic to interpret the `LedAnimation` enum and set up the correct pattern parameters.
        * `SOLID`: Turns the LED on to the specified color and stops any previous animation.
        * `SMOOTH_FLASH_FAST`: A quick fade-in/out cycle, followed by a 250ms "off" pause before repeating.
        * `SMOOTH_FLASH_SLOW`: A quick fade-in/out cycle, followed by a 900ms "off" pause before repeating.
        * `PULSE_FAST`: A continuous, rapid "breathing" effect with no pause.
        * `PULSE_SLOW`: A continuous, slow "breathing" effect with no pause.
    * `void turnOff()`: Stops all animations and turns off all LEDs.

#### C. `FeedbackManager` Class (`feedback/FeedbackManager.h` & `.cpp`)
* [ ] **Composition**: It must contain private instances of `LedController` and `BuzzerController`.
* [ ] **Automatic Persistence**:
    * **Namespace**: All `Preferences` operations **must** use the `"settings"` namespace.
    * **`loadSettings()`**: Called in `begin()`, this method loads brightness and stealth mode from the `Preferences` library and passes them to the controllers. Defaults (50% brightness, stealth off) must be used if no settings are found.
    * **`saveSettings()`**: This method **must be called automatically** from within `setBrightness()` and `setStealthMode()` to ensure all changes are saved immediately.
* [ ] **Delegated Methods**: It must provide a clean public interface that delegates all calls to the appropriate internal controller (e.g., `manager.startAnimation(...)` calls `ledController.startAnimation(...)`).

---

## 4. Technical Requirements & Implementation Strategy

### Instructions for Copilot:
1.  **Adhere to SRP**: Follow the Single Responsibility Principle. The three classes created must each have a single, clear responsibility.
2.  **Use Specified Libraries**: You **must** use `Adafruit NeoPixel` for LEDs and `Preferences` for storage.
3.  **Implement Animations**: The LED animations must be smooth and non-blocking, driven by the `update()` loop.
4.  **Verification**: After generating the code, run `pio run`. The build **must** succeed. Test by calling the new `startAnimation` methods and confirming the visual output matches the descriptions. Test that brightness and stealth settings persist after a reboot.