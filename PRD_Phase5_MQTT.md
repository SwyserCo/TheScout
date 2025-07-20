# Product Requirements Document: The Scout - Phase 5

## 1. Phase Goal: Implement Factory Reset Functionality

The primary goal of this phase is to implement a safe and reliable factory reset mechanism using the dedicated physical button. This feature allows a user to erase all stored configuration and return the device to its out-of-the-box state, ready for a fresh setup.

## 2. Core Features & Logic

### A. User Story
**As a user**, if my device can't connect to WiFi (e.g., I've changed my router) or I want to reconfigure it, I need a simple way to erase its settings and start over.

### B. Trigger Mechanism
1.  **Button & Pin**: The factory reset is initiated using the "Factory Reset Button" connected to `IO02` (as defined in `Master_PRD.md`).
2.  **Activation**: The user must **press and hold** the button for **10 consecutive seconds**. A short, accidental press must not trigger the reset.
3.  **Non-Blocking Check**: The firmware must check the button's state in the main `loop()` without blocking other operations.

### C. User Feedback During Reset Process
The `Feedback` module must be used to provide clear, unambiguous feedback to the user during the reset attempt.

1.  **While Button is Held**:
    * **System LED (IO09)**: Should blink rapidly in an "alert" color, like orange or red.
    * **Buzzer**: Should remain silent to avoid being annoying.
2.  **Upon Successful Reset (after 10 seconds)**:
    * **System LED (IO09)**: Should flash a confirmation pattern, e.g., three quick green blinks.
    * **Buzzer**: Must play the distinct `playResetChime()` from the `Feedback` module.

### D. Reset Action
Once the 10-second hold is confirmed, the device must perform the following actions:

1.  **Erase Preferences**: Completely clear the entire `Preferences` namespace, deleting the stored WiFi credentials and the unique `DeviceID`.
2.  **Reboot**: Immediately trigger a system reboot.

### E. Expected Outcome
After the device reboots, it will find no stored credentials in `Preferences`. As per the logic defined in Phase 2, it will automatically enter the **Captive Portal / AP Mode**, allowing the user to perform a fresh setup.

## 3. Technical Requirements & Implementation Strategy

### Instructions for Copilot:
1.  **Implement the Logic Flow**: Adhere strictly to the trigger, feedback, and action logic defined above. The 10-second timer must be accurate and non-blocking.
2.  **Integrate Phase 1 Modules**: You must `#include` and use the `Feedback` module to provide all user feedback as specified.
3.  **Code Structure**: This logic can be implemented in a new `DeviceManager` module or added to an appropriate existing module. The button-checking logic should reside in the main `loop()`.
4.  **Verification**: After generating the code, you must verify it by running the command `pio run`. The build **must** succeed without any errors. The final test is to hold the reset button for 10 seconds and confirm that the device provides the correct feedback, reboots, and enters AP mode.