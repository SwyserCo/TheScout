# Product Requirements Document: The Scout - Phase 0

## 1. Phase Goal: Project Scaffolding & Core Architecture

The primary goal of this phase is to establish the complete, compilable skeleton of the firmware project. This involves creating the entire directory structure, all necessary configuration files, all core libraries, and all class files with pre-populated boilerplate code. The project **must** compile successfully at the end of this phase.

## 2. Module Interactions & Dependencies
This foundational phase has no dependencies on other code modules. Its purpose is to **create the structure and files** that all subsequent phases (`PRD_Phase1` through `PRD_Phase5`) will depend on. It will use the **`Master_PRD.md`** for the folder structure and library list, and the **`HARDWARE_SPEC.md`** for pin definitions.

---

## 3. Core Features & Architecture

### User Story
**As a developer**, I need a complete and correct project skeleton generated automatically, so I can immediately start implementing features without any manual setup or configuration.

### Acceptance Criteria

#### A. `platformio.ini` Configuration
* [ ] Create the `platformio.ini` file.
* [ ] It must define the correct `[env:esp32s3]` with the `platform`, `board`, and `framework`.
* [ ] It must set the `monitor_speed` to `115200`.
* [ ] It must contain a `lib_deps` section that lists **all** of the "Core Libraries & Dependencies" specified in the `Master_PRD.md`.

#### B. Directory and File Structure
* [ ] Create the complete directory structure as defined in the `Master_PRD.md`.
* [ ] Create all the `.h` and `.cpp` files for every class mentioned in the folder structure.

#### C. Boilerplate Code Generation
* [ ] **Header Files (`.h`):** Every class header file created **must** include the following boilerplate:
    * `#pragma once`
    * An include for `<Arduino.h>`
    * A basic class definition (e.g., `class FeedbackManager { ... };`)
    * A public constructor, a `void begin();` method, and a `void update();` method.
* [ ] **Source Files (`.cpp`):** Every class source file created **must** include the header file for its class and provide empty implementations for the constructor, `begin()`, and `update()` methods. Each stub method should contain a `Serial.println("ClassName::methodName() called");` for immediate debugging feedback.

#### D. Configuration Files (`include/config/`)
* [ ] **`Pins.h`**: Create this file. It must contain `#define` statements for every hardware pin listed in the "Pin Assignments" table of the **`HARDWARE_SPEC.md`** document.
* [ ] **`Settings.h`**: Create this file. It must contain the following:
    * A global debug flag: `#define DEBUG 1`
    * A placeholder for the firmware version: `#define FIRMWARE_VERSION "0.1.0"`

#### E. Main Application Skeleton (`src/main.cpp`)
* [ ] **Includes**: `main.cpp` must `#include` all the manager/handler header files.
* [ ] **Object Instantiation**: It must create global instances of each of the main manager classes.
* [ ] **State Machine**: It must define the `enum class SystemState` and initialize a `currentState` variable to `SystemState::BOOTING`.
* [ ] **`setup()` function**:
    * Must initialize the Serial monitor using `Serial.begin(115200)`. The speed **must** match the `monitor_speed` from `platformio.ini`.
    * Must call the `begin()` method on each of the instantiated manager objects.
* [ ] **`loop()` function**:
    * Must contain a `switch` statement for the `SystemState`.
    * Must call the `update()` method for each of the instantiated manager objects on every loop.

---

## 4. Technical Requirements & Implementation Strategy

### Instructions for Copilot:
1.  **Follow the Blueprint**: Adhere strictly to the folder structure and file creation requirements defined in the `Master_PRD.md`.
2.  **Generate Boilerplate**: The goal is to create the complete, compilable structure. Do not implement any real logic beyond the required boilerplate and stub methods with serial prints.
3.  **Verification**: After generating all the files and code, you must verify it by running the command `pio run`. The build **must** succeed without any errors.