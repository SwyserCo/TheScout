# Product Requirements Document: The Scout - Phase 3 (Comprehensive)

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
**As a user** who is giving my Scout to a friend, I want a secure and reliable way to wipe all of my personal information (my WiFi password) from the device before I hand it over.

### Acceptance Criteria

#### A. Trigger & System Priority
* [ ] **Trigger**: The factory reset is initiated by pressing and holding the "Factory Reset Button" (defined in `HARDWARE_SPEC.md`) for **exactly 5 consecutive seconds**. This **must** be measured with a non-blocking `millis()` timer.
* [ ] **Cancellation**: If the button is released at any point *before* the 5-second timer completes, the reset process **must** be immediately and completely cancelled. The device should return to its previous operational state with no settings erased.
* [ ] **System Priority**: The factory reset process **must** take absolute priority. If the button is pressed, any ongoing process (like a WiFi connection attempt) **must** be paused or ignored while the device monitors the button hold.

#### B. User Feedback
* [ ] **While Button is Held**: For the duration of the 5-second hold, the **Activity LED (`PIXEL_ACTIVITY`)** must **blink green fast** (`BLINK` with `HEARTHGUARD_GREEN`, fast interval). The buzzer must remain silent.
* [ ] **On Cancellation**: If the button is released early, the Activity LED must immediately turn off. No sound should be played.
* [ ] **On Success**: Exactly at the 5-second mark, the Activity LED must stop flashing and turn off. Immediately after, the device **must** play the `playConfirm()` sound from the `BuzzerController`.

#### C. The Reset Action
* [ ] **Selective Wipe**: Upon a successful 5-second trigger, the device **must** erase **only the "credentials" namespace** from the `Preferences` library.
* [ ] **Preserve Settings**: The "settings" namespace in the `Preferences` library (managed by the `FeedbackManager`) **must not be touched**.
* [ ] **Reboot**: Immediately after erasing the credentials and playing the confirmation sound, the device **must** perform a full system restart.

#### D. Expected Outcome
* [ ] After the device reboots, it will find no credentials in memory and will automatically enter the **Captive Portal / AP Mode** (as defined in `PRD_Phase2_WiFi.md`).

---

## 4. Technical Requirements & Implementation Strategy

### Instructions for Copilot:
1.  **Adhere to SRP**: The logic for handling the factory reset should be encapsulated in a dedicated `DeviceManager` class.
2.  **Integrate Phase 1 Modules**: You must use the `FeedbackManager` to provide all user feedback.
3.  **Code Structure**: Create a `DeviceManager.h` and `DeviceManager.cpp` inside the `setup/` directory.
4.  **Verification**: After generating the code, run `pio run`. The build **must** succeed.