# Product Requirements Document: The Scout - Phase 3 (Advanced)

## 1. Phase Goal: Implement a Safe and Selective Factory Reset

The primary goal of this phase is to implement a safe, reliable, and user-friendly factory reset mechanism. This feature must selectively erase only network credentials while preserving user settings, and it must take absolute priority over all other device operations.

## 2. Module Interactions & Dependencies
This phase involves implementing the logic within the **`DeviceManager`** module. This module will need to:
* Interact with the **`FeedbackManager`** module (defined in `PRD_Phase1_Feedback.md`) to provide clear user feedback.
* Interact with the **`Preferences`** library to selectively erase the "credentials" namespace.

---

## 3. User Stories & Acceptance Criteria

### User Story 1: The "New Router" Scenario
**As a user** who has changed my home WiFi network, I need a simple and safe way to erase the old network details from my Scout, so I can set it up on my new network without losing my other settings.

### User Story 2: The "Hand-Me-Down" Scenario
**As a user** who is giving my Scout to a friend, I want a secure and reliable way to wipe all of my personal information (my WiFi password) from the device before I hand it over, while keeping the device's settings intact for them.

### Acceptance Criteria

#### A. Trigger & System Priority
* [ ] **Trigger**: The factory reset is initiated by pressing and holding the "Factory Reset Button" (defined in `HARDWARE_SPEC.md`) for **exactly 5 consecutive seconds**.
* [ ] **Cancellation**: If the button is released at any point *before* the 5-second timer completes, the reset process **must** be immediately and completely cancelled. The device should return to its previous operational state.
* [ ] **System Priority**: The factory reset process **must** take absolute priority. If the button is pressed, any ongoing process (like a WiFi connection attempt) **must** be paused or ignored while the device monitors the button hold.

#### B. User Feedback
* [ ] **While Button is Held**: For the duration of the 5-second hold, the **Activity LED (`PIXEL_ACTIVITY`)** must **flash green fast** (e.g., `SMOOTH_FLASH_FAST` with `HEARTHGUARD_GREEN`). The buzzer must remain silent.
* [ ] **On Cancellation**: If the button is released early, the Activity LED must immediately turn off. No sound should be played.
* [ ] **On Success**: Exactly at the 5-second mark, the Activity LED must stop flashing and turn off. Immediately after, the device **must** play the `playResetConfirm()` sound from the `BuzzerController` to signal success.

#### C. The Reset Action
* [ ] **Selective Wipe**: Upon a successful 5-second trigger, the device **must** erase **only the "credentials" namespace** from the `Preferences` library. This includes the WiFi SSID, password, and the unique Device ID.
* [ ] **Preserve Settings**: The "settings" namespace in the `Preferences` library, which contains user preferences like LED brightness and stealth mode (managed by the `FeedbackManager`), **must not be touched**.
* [ ] **Reboot**: Immediately after erasing the credentials and playing the confirmation sound, the device **must** perform a full system restart.

#### D. Expected Outcome
* [ ] After the device reboots, it will find no credentials in memory. As per the logic defined in `PRD_Phase2_WiFi.md`, it will automatically enter the **Captive Portal / AP Mode**, ready for a fresh setup, but it will retain the user's last-saved brightness and stealth mode settings.

---

### Important Architectural Note for Other Phases

This phase introduces the concept of separating `Preferences` into two distinct namespaces:

1.  **`credentials`**: For WiFi SSID, password, and Device ID. This namespace is managed by the `WifiHandler` (`PRD_Phase2_WiFi.md`).
2.  **`settings`**: For user-configurable options like LED brightness and stealth mode. This namespace is managed by the `FeedbackManager` (`PRD_Phase1_Feedback.md`).

This separation is crucial and must be reflected in the implementation of those other phases.