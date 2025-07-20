# Product Requirements Document: The Scout - Phase 1

## 1. Phase Goal: Implement User Feedback Modules

The primary goal of this phase is to create robust, non-blocking software modules to control the onboard LEDs and the passive buzzer. These modules will provide the foundational user feedback mechanisms required for all subsequent development phases.

---

## 2. Core Features: LED and Buzzer Controllers

### User Story
**As a developer**, I need reusable, non-blocking classes to control LEDs and the buzzer, so I can easily provide clear status feedback to the user throughout the device's operation.

### Acceptance Criteria

#### A. LED Controller Module
1.  **Class Structure**: Create a class named `Feedback` (or similar, like `LEDController`) to manage all LED functionality.
2.  **Initialization**: The controller should be initialized with the GPIO pins for the LEDs it will manage (e.g., `System LED` on IO09, `Activity LED` on IO48), as defined in `Master_PRD.md`.
3.  **Non-Blocking Operation**: All pattern-based methods (`blink`, `pulse`) must be non-blocking. The class must have an `update()` method that is called in the main `loop()` to manage the timing.
4.  **Required Methods**:
    * `void on(int pin)`: Turn an LED on solid.
    * `void off(int pin)`: Turn an LED off.
    * `void blink(int pin, int interval)`: Blink an LED at a specified interval.
    * `void pulse(int pin, int speed)`: Fade an LED on and off smoothly (PWM).
5.  **Pre-defined Patterns for `System LED` (IO09)**:
    * **Setup Mode**: Pulsing Blue (to be used in Phase 2).
    * **Connecting**: Blinking Yellow (to be used in Phase 2).
    * **Success**: Solid Green for 5 seconds, then off (to be used in Phase 2).

#### B. Buzzer Controller Module
1.  **Class Structure**: This functionality can be part of the same `Feedback` class or a separate `BuzzerController` class.
2.  **Tone Generation via PWM**: To make the passive buzzer produce sound, it **must be driven by a PWM signal**. The controller must use the ESP32's `ledc` peripheral to generate PWM signals of specific frequencies on the buzzer pin (`IO40`). Each frequency corresponds to a different musical note.
3.  **Non-Blocking Operation**: Playing a sequence of notes (a "chime") must be non-blocking. The class needs an `update()` method to be called in the main `loop()` to handle note durations and pauses.
4.  **Required Methods**:
    * `void playNote(int frequency, int duration)`: Play a single note for a set duration.
    * `void playSuccessChime()`: Play a pleasant, ascending two-note chime (e.g., C5 then G5).
    * `void playFailureTone()`: Play a low, descending two-note tone (e.g., G4 then C4).
    * `void playResetChime()`: Play a short, single confirmation beep.

---

## 3. Technical Requirements & Implementation Strategy

### Instructions for Copilot:
1.  **Strategize First**: Outline your plan, e.g., "I will create a `Feedback` class in its own module (`Feedback.h`/`.cpp`). It will manage both LEDs and the Buzzer. The class will have an `update()` method to be called in the main loop to ensure non-blocking operation. For the buzzer, I will configure a `ledc` channel to generate tones."
2.  **Code Structure**: Follow the modular folder structure defined in `Master_PRD.md`.
3.  **Testing**: Create a `main.cpp` that demonstrates each function. For example, in `setup()`, call `playSuccessChime()`, and in `loop()`, make the System LED pulse and the Activity LED blink. This will allow for immediate testing on the hardware.
4.  **Verification**: After generating the code, you must verify it by running the command `pio run`. The build **must** succeed without any errors. The final test is to physically see the LEDs perform their patterns and hear the buzzer play its tones as commanded.