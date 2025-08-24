# Product Requirements Document: The Scout - Phase 1 (Comprehensive)

## 1. Phase Goal: Implement Advanced, Modular Feedback System

The primary goal of this phase is to create a clean, modular, and highly configurable feedback system. This involves creating three distinct classes (`LedController`, `BuzzerController`, `FeedbackManager`) that provide a rich set of non-blocking visual and audible cues, with settings that persist across reboots.

## 2. Module Interactions & Dependencies
This phase involves implementing the three core feedback modules. The `FeedbackManager` will depend on the **`Preferences`** library. The `LedController` will depend on the **`Adafruit NeoPixel`** library. All pin definitions **must** be used from the `Pins.h` file, which is populated from **`HARDWARE_SPEC.md`**.

---

## 3. User Stories & Acceptance Criteria

### User Story 1: Device Status Indication
**As a user**, I want clear, intuitive, and aesthetically pleasing visual feedback from the device's LEDs, so I can understand its status at a glance without it being distracting or ambiguous.

### User Story 2: Audible Cues
**As a user**, I want short, distinct, and thematic audible cues for important events, so I can get confirmation of actions without needing to look at the device.

### User Story 3: Configuration Persistence
**As a user**, I want the device to remember my preferred brightness and stealth mode settings, so I don't have to reconfigure it every time it powers on.

### Acceptance Criteria

#### A. `BuzzerController` Class (`feedback/BuzzerController.h` & `.cpp`)
* [ ] **Power-On Sound**: The `begin()` method of this class must play a single, short "boot" beep (150ms duration) to confirm the device has powered on.
* [ ] **Thematic Sound Abstraction**: The specific notes and durations for each sound **must** be defined as constants. Use simple, distinct tones for now.
    * `Success`: Two quick, rising notes.
    * `Failure`: A single, low, longer note.
    * `Confirm`: A single, sharp, medium note.
    * `Interaction`: A very short, high "tick" note.
* [ ] **Required Methods**:
    * `void begin()`: Initializes the `ledc` peripheral and plays the boot sound.
    * `void update()`: Manages non-blocking playback.
    * `void setStealthMode(bool enabled)`: Disables all sound output if true.
    * `void playSuccess()`, `void playFailure()`, `void playConfirm()`, `void playInteraction()`

#### B. `LedController` Class (`feedback/LedController.h` & `.cpp`)
* [ ] **Hardware Note**: As per `HARDWARE_SPEC.md`, the two WS2812B LEDs are on separate pins. The `LedController` **must** manage **two separate instances** of the `Adafruit_NeoPixel` class.
* [ ] **Animation Engine**: The `update()` method must drive a non-blocking animation engine. All fade effects must be a smooth "breathing" (sine-wave) effect with a fixed fade duration of 50ms.
* [ ] **Animation Type Definition**: An `enum class LedAnimation` must be defined: `SOLID`, `BLINK`, `PULSE`.
* [ ] **Color Palette & Pixels**: The header file must define the standard color palette and pixel names (`PIXEL_SYSTEM`, `PIXEL_ACTIVITY`).
    * `HEARTHGUARD_GREEN`: (0, 255, 0)
    * `HEARTHGUARD_RED`: (255, 0, 0)
    * `HEARTHGUARD_BLUE`: (0, 0, 255)
    * `HEARTHGUARD_ORANGE`: (255, 165, 0)
    * `HEARTHGUARD_YELLOW`: (255, 255, 0)
* [ ] **Required Methods**:
    * `void begin()`: Initializes both NeoPixel objects. LEDs are off by default.
    * `void update()`: Must be called in the main loop to drive all animations.
    * `void setBrightness(uint8_t brightness)`
    * `void setStealthMode(bool enabled)`
    * `void startAnimation(uint8_t pixelIndex, uint32_t color, LedAnimation animation, uint16_t interval)`: Starts a new animation. The `interval` parameter defines the "off" time for `BLINK` or the full cycle time for `PULSE`.
    * `void turnOff()`

#### C. `FeedbackManager` Class (`feedback/FeedbackManager.h` & `.cpp`)
* [ ] **Composition**: It must contain private instances of `LedController` and `BuzzerController`.
* [ ] **Automatic Persistence**:
    * **Namespace**: All `Preferences` operations **must** use the `"settings"` namespace.
    * **`loadSettings()`**: Called in `begin()`, this method loads brightness and stealth mode from `Preferences`.
    * **`saveSettings()`**: This method **must be called automatically** from within `setBrightness()` and `setStealthMode()`.
* [ ] **Delegated Methods**: It must provide a clean public interface that delegates all calls to the appropriate internal controller.

---

## 4. Technical Requirements & Implementation Strategy

### Instructions for Copilot:
1.  **Adhere to SRP**: Follow the Single Responsibility Principle. The three classes created must each have a single, clear responsibility.
2.  **Use Specified Libraries**: You **must** use `Adafruit NeoPixel` for LEDs and `Preferences` for storage.
3.  **Implement Animations**: The LED animations must be smooth and non-blocking.
4.  **Verification**: After generating the code, run `pio run`. The build **must** succeed. Test that brightness and stealth settings persist after a reboot.