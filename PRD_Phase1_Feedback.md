# Product Requirements Document: The Scout - Phase 1 (Comprehensive)

## 1. Phase Goal: Implement Advanced, Modular Feedback System

The primary goal of this phase is to create a clean, modular, and highly configurable feedback system. This involves creating three distinct classes (`LedController`, `BuzzerController`, `FeedbackManager`) that provide a rich set of non-blocking visual and audible cues, with settings that persist across reboots. This module is foundational for all user-facing feedback and must be implemented with the highest level of detail and quality.

## 2. Module Interactions & Dependencies
This phase involves implementing the three core feedback modules. The `FeedbackManager` will depend on the **`Preferences`** library. The `LedController` will depend on the **`FastLED`** library. All pin definitions **must** be used from the `Pins.h` file, which is populated from **`HARDWARE_SPEC.md`**.

---

## 3. User Stories & Acceptance Criteria

### User Story 1: Device Status Indication
**As a user**, I want clear, intuitive, and aesthetically pleasing visual feedback from the device's LEDs, so I can understand its status at a glance (e.g., "Is it connecting?", "Did it fail?") without it being distracting or ambiguous.

### User Story 2: Audible Cues
**As a user**, I want short, distinct, and thematic audible cues for important events (like success or failure), so I can get confirmation of actions without needing to look at the device.

### User Story 3: Configuration Persistence
**As a user**, I want the device to remember my preferred brightness and stealth mode settings, so I don't have to reconfigure it every time it powers on.

### Acceptance Criteria

#### A. `BuzzerController` Class (`feedback/BuzzerController.h` & `.cpp`)
* [ ] **Hardware Requirement**: The buzzer is a passive electromagnetic type. It **requires a PWM signal** of a specific frequency to produce a tone. The implementation **must** use the ESP32's `ledc` (LED Control) peripheral.
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
* [ ] **Critical: Deferred Initialization**: To prevent boot issues with the ESP32, the `LedController` **must not** initialize the FastLED controllers in its constructor. It must declare a `CRGB` array for the LEDs, but the `FastLED.addLeds(...)` calls **must** be made inside the `begin()` method, after the main `setup()` has started.
* [ ] **Hardware Note**: As per `HARDWARE_SPEC.md`, the two WS2812B LEDs are on separate pins. The `LedController` **must** manage this by defining a single `CRGB` array of size 2 and making two separate `FastLED.addLeds` calls, one for each pin, mapping them to the correct index in the array (e.g., `PIXEL_SYSTEM` at index 0, `PIXEL_ACTIVITY` at index 1).
* [ ] **Animation Engine**: The `update()` method must drive a non-blocking animation engine. All fade effects must be a smooth "breathing" (sine-wave) effect with a fixed fade duration of 50ms.
* [ ] **Animation Type Definition**: An `enum class LedAnimation` must be defined: `SOLID`, `BLINK`, `PULSE`.
* [ ] **Color Palette & Pixels**: The header file must define the standard color palette and pixel names (`PIXEL_SYSTEM`, `PIXEL_ACTIVITY`).
    * The colors **must** be defined using `CRGB` structs (e.g., `const CRGB HEARTHGUARD_GREEN = CRGB::Green;`).
    * `HEARTHGUARD_GREEN`: `CRGB::Green`
    * `HEARTHGUARD_RED`: `CRGB::Red`
    * `HEARTHGUARD_BLUE`: `CRGB::Blue`
    * `HEARTHGUARD_ORANGE`: `CRGB::OrangeRed`
    * `HEARTHGUARD_YELLOW`: `CRGB::Yellow`
* [ ] **Required Methods**:
    * `void begin()`: Initializes both FastLED controllers. LEDs are off by default.
    * `void update()`: Must be called in the main loop to drive all animations and call `FastLED.show()`.
    * `void setBrightness(uint8_t brightness)`
    * `void setStealthMode(bool enabled)`
    * `void startAnimation(uint8_t pixelIndex, CRGB color, LedAnimation animation, uint16_t interval)`: Starts a new animation. The `interval` parameter defines the "off" time for `BLINK` or the full cycle time for `PULSE`.
    * `void turnOff()`

#### C. `FeedbackManager` Class (`feedback/FeedbackManager.h` & `.cpp`)
* [ ] **Composition**: It must contain private instances of `LedController` and `BuzzerController`.
* [ ] **Automatic Persistence**:
    * **Namespace**: All `Preferences` operations **must** use the `"settings"` namespace.
    * **`loadSettings()`**: Called in `begin()`, this method loads brightness and stealth mode from `Preferences`.
    * **`saveSettings()`**: This method **must be called automatically** from within `setBrightness()` and `setStealthMode()`.
* [ ] **Delegated Methods**: It must provide a clean public interface that delegates all calls to the appropriate internal controller.

---

## 4. Verification & Test Plan

To verify the successful implementation of this phase, the `main.cpp` file should be temporarily modified to execute a specific test sequence. This is not a suggestion; it is a mandatory test plan.

### `main.cpp` Test Sequence:
1.  **In `setup()`**:
    * After `feedbackManager.begin()`, add the following line:
      ```cpp
      feedbackManager.setBrightness(100); // Set brightness to ~40% for testing
      ```
2.  **In `loop()`**:
    * The main loop should contain a simple, non-blocking state machine using `millis()` to demonstrate the different features.

    ```cpp
    // Example Test Logic for main.cpp loop()
    static uint32_t lastChange = 0;
    static int testState = 0;

    if (millis() - lastChange > 5000) { // Change state every 5 seconds
      lastChange = millis();
      testState++;
      if (testState > 4) testState = 0;

      switch (testState) {
        case 0:
          Serial.println("TEST: Starting SOLID GREEN animation on System LED.");
          feedbackManager.startAnimation(PIXEL_SYSTEM, HEARTHGUARD_GREEN, LedAnimation::SOLID, 0);
          feedbackManager.playSuccess();
          break;
        case 1:
          Serial.println("TEST: Starting fast BLINK ORANGE on Activity LED.");
          feedbackManager.startAnimation(PIXEL_ACTIVITY, HEARTHGUARD_ORANGE, LedAnimation::BLINK, 200);
          break;
        case 2:
          Serial.println("TEST: Starting slow PULSE BLUE on System LED.");
          feedbackManager.startAnimation(PIXEL_SYSTEM, HEARTHGUARD_BLUE, LedAnimation::PULSE, 2000);
          feedbackManager.turnOff(PIXEL_ACTIVITY); // Turn off other LED
          break;
        case 3:
          Serial.println("TEST: Enabling Stealth Mode.");
          feedbackManager.setStealthMode(true);
          feedbackManager.playFailure(); // This should do nothing
          break;
        case 4:
          Serial.println("TEST: Disabling Stealth Mode.");
          feedbackManager.setStealthMode(false);
          break;
      }
    }
    ```

### Instructions for Copilot:
1.  **Adhere to SRP**: Follow the Single Responsibility Principle. The three classes created must each have a single, clear responsibility.
2.  **Use Specified Libraries**: You **must** use `fastled/FastLED` for LEDs and `Preferences` for storage.
3.  **Implement Test Plan**: After generating the module code, you must populate the `main.cpp` file with the test sequence described above.
4.  **Verification**: After generating all the files and code, you must verify it by running the command `pio run`. The build **must** succeed without any errors.
