# Product Requirements Document: The Scout - Phase 0 (Comprehensive)

## 1. Phase Goal: Project Scaffolding & Core Architecture

The primary goal of this phase is to establish the complete, compilable skeleton of the firmware project. This involves creating the entire directory structure, all necessary configuration files, all core libraries, and all class files with pre-populated, non-empty boilerplate code. The project **must** compile successfully at the end of this phase.

## 2. Module Interactions & Dependencies
This foundational phase has no dependencies on other code modules. Its purpose is to **create the structure and files** that all subsequent phases (`PRD_Phase1` through `PRD_Phase5`) will depend on. It will use the **`Master_PRD.md`** for the folder structure and library list, and the **`HARDWARE_SPEC.md`** for pin definitions.

---

## 3. Core Features & Architecture

### User Story
**As a developer**, I need a complete and correct project skeleton generated automatically, with all files containing the necessary boilerplate code, so I can immediately start implementing features without any manual setup or configuration.

### Acceptance Criteria

#### A. `platformio.ini` Configuration
* [ ] Create the `platformio.ini` file.
* [ ] It must define the correct `[env:esp32s3]` with the `platform`, `board`, and `framework`.
* [ ] It must set the `monitor_speed` to `115200`.
* [ ] It must contain a `lib_deps` section that lists **all** of the "Core Libraries & Dependencies" specified in the `Master_PRD.md`.

#### B. Directory and File Structure
* [ ] Create the complete directory structure as defined in the `Master_PRD.md`.
* [ ] Create all the `.h` and `.cpp` files for every class mentioned in the folder structure.

#### C. Boilerplate Code Generation (Mandatory Template)
All generated files **must not be empty**. They must be populated with the following specific boilerplate code.

* [ ] **Header Files (`.h`):** Every class header file created (e.g., `FeedbackManager.h`) **must** follow this exact template:
    ```cpp
    #pragma once

    #include <Arduino.h>

    class FeedbackManager {
    public:
      FeedbackManager(); // Constructor
      void begin();
      void update();
    };
    ```

* [ ] **Source Files (`.cpp`):** Every class source file created (e.g., `FeedbackManager.cpp`) **must** follow this exact template, including its own header file:
    ```cpp
    #include "feedback/FeedbackManager.h" // IMPORTANT: Must include its own header

    FeedbackManager::FeedbackManager() {
      // Constructor body can be empty
    }

    void FeedbackManager::begin() {
      #ifdef DEBUG
      Serial.println("FeedbackManager::begin() called");
      #endif
    }

    void FeedbackManager::update() {
      // This method is called in the main loop
    }
    ```

#### D. Configuration Files (`include/config/`)
* [ ] **`Pins.h`**: Create this file. It must contain `#define` statements for every hardware pin listed in the "Pin Assignments" table of the **`HARDWARE_SPEC.md`** document.
* [ ] **`Settings.h`**: Create this file. It must contain the following:
    * A global debug flag: `#define DEBUG 1`. All `Serial.print()` statements in the project must be wrapped in an `#ifdef DEBUG` block.
    * A placeholder for the firmware version: `#define FIRMWARE_VERSION "0.1.0"`
* [ ] **`DataTypes.h`**: Create this file. It must define the central `SensorData` struct that will be used as the data contract between the `SensorManager` and `MqttHandler`.

#### E. Main Application Skeleton (`src/main.cpp`)
* [ ] The `main.cpp` file **must not be empty**. It must be fully populated with the complete skeleton code.
* [ ] **Includes**: `main.cpp` must `#include` all the manager/handler header files (e.g., `feedback/FeedbackManager.h`, `network/WifiHandler.h`, etc.).
* [ ] **Object Instantiation**: It must create global instances of each of the main manager classes.
* [ ] **State Machine**: It must define the `enum class SystemState` and initialize a `currentState` variable to `SystemState::BOOTING`.
* [ ] **`setup()` function**:
    * Must initialize the Serial monitor using `Serial.begin(115200)`.
    * Must call the `begin()` method on each of the instantiated manager objects.
* [ ] **`loop()` function**:
    * Must contain a `switch` statement for the `SystemState`.
    * Must call the `update()` method for each of the instantiated manager objects on every loop.

---

## 4. Technical Requirements & Implementation Strategy

### Instructions for Copilot:
1.  **Follow the Blueprint**: Adhere strictly to the folder structure and file creation requirements defined in the `Master_PRD.md`.
2.  **Generate Boilerplate Exactly**: The goal is to create the complete, compilable structure. Do not deviate from the boilerplate templates provided in Section 3.C.
3.  **Verification**: After generating all the files and code, you must verify it by running the command `pio run`. The build **must** succeed without any errors.

---

## 5. Verification Checklist (Mandatory)

After completing the implementation for this phase, you **must** end your response with the following checklist, confirming that each item has been completed.

- [ ] **`platformio.ini`:** File created and contains the correct environment and all 7 `lib_deps` entries from the `Master_PRD.md`.
- [ ] **Folder Structure:** All required subdirectories (`config`, `feedback`, `network`, `setup`, `sensors`, `utilities`) have been created inside `include/` and `src/`.
- [ ] **Config Files:** `Pins.h`, `Settings.h`, and `DataTypes.h` have been created and populated.
- [ ] **Module Files:** All 10 module files (`FeedbackManager.h/.cpp`, `LedController.h/.cpp`, etc.) have been created and contain the required, non-empty boilerplate code as per the templates.
- [ ] **`main.cpp`:** File created and contains the full skeleton code, including includes, global object instantiations, the state machine enum, and populated `setup()` and `loop()` functions.
