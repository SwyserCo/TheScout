# Product Requirements Document: The Scout - Phase 3

## 1. Phase Goal: Implement Factory Reset Functionality

The primary goal of this phase is to implement a safe and reliable factory reset mechanism using the dedicated physical button.

## 2. Core Features & Logic

### A. User Story
**As a user**, if my device can't connect to WiFi or I want to reconfigure it, I need a simple way to erase its settings and start over.

### B. Trigger Mechanism
1.  **Button & Pin**: The factory reset is initiated using the "Factory Reset Button" connected to `IO02`.
2.  **Activation**: The user must **press and hold** the button for **10 consecutive seconds**.
3.  **Non-Blocking Check**: The firmware must check the button's state in the main `loop()` without blocking.

### C. User Feedback During Reset Process
The `FeedbackManager` must be used to provide clear feedback:

1.  **While Button is Held**:
    * **System LED (PIXEL_SYSTEM)**: Should blink rapidly with `HEARTHGUARD_ORANGE`.
    * **Buzzer**: Should play the `playClink()` sound once when the button is first pressed.
2.  **Upon Successful Reset (after 10 seconds)**:
    * **System LED (PIXEL_SYSTEM)**: Should flash a confirmation pattern (e.g., three quick green blinks).
    * **Buzzer**: Must play the distinct `playLutePluck()` sound.

### D. Reset Action
Once the 10-second hold is confirmed, the device must:

1.  **Erase Preferences**: Completely clear the entire `Preferences` namespace.
2.  **Reboot**: Immediately trigger a system reboot.

### E. Expected Outcome
After the device reboots, it will find no stored credentials and will automatically enter **Captive Portal / AP Mode**.

---

## 3. Technical Requirements & Implementation Strategy

### Instructions for Copilot:
1.  **Adhere to SRP**: The logic for handling the factory reset should be encapsulated in a dedicated `DeviceManager` class.
2.  **Integrate Phase 1 Modules**: You must use the `FeedbackManager` to provide all user feedback.
3.  **Code Structure**: Create a `DeviceManager.h` and `DeviceManager.cpp` inside the `setup/` directory.
4.  **Verification**: After generating the code, run `pio run`. The build **must** succeed.
