Hello Copilot.

We are beginning a new, clean development session for the "HearthGuard: The Scout" firmware. It is critical that you **forget all previous files, code, and instructions**. We are starting fresh from a definitive set of project documents.

Your work will be governed by the following four documents, which you must treat as the **single source of truth**:

1.  **`Master_PRD.md`**: Defines the high-level roadmap, core libraries, and project structure.
2.  **`ARCHITECTURE.md`**: Defines the software architecture, module APIs, and data/control flow.
3.  **`HARDWARE_SPEC.md`**: Defines all physical pin assignments and hardware interfaces.
4.  **`copilot-instructions.md`**: Defines the mandatory technical rules, coding standards, and our workflow.

**Our Workflow is Non-Negotiable and Must Be Followed Exactly:**

We will build the firmware **one phase at a time**. After you generate the code for a given phase, you will **STOP**. I will then build, test, and review the code. We will have one or more **improvement cycles** to fix issues and refine the implementation. I will give you the explicit command to proceed to the next phase when the current one is complete.

**The Development Roadmap:**

* **Phase 0: Project Scaffolding & Core Architecture**
    * *Reference File:* `PRD_Phase0_Scaffolding.md`
* **Phase 1: User Feedback Modules (LED & Buzzer)**
    * *Reference File:* `PRD_Phase1_Feedback.md`
* **Phase 2: WiFi Setup & Connectivity**
    * *Reference File:* `PRD_Phase2_WiFi.md`
* **Phase 3: Factory Reset**
    * *Reference File:* `PRD_Phase3_FactoryReset.md`
* **Phase 4: Sensor Integration & Data Acquisition**
    * *Reference File:* `PRD_Phase4_Sensors.md`
* **Phase 5: MQTT Integration & Home Assistant Discovery**
    * *Reference File:* `PRD_Phase5_MQTT.md`

Let's begin.

Our first task is **Phase 0: Project Scaffolding & Core Architecture**.

Please generate all the files and skeleton code required to complete this phase, as described in `PRD_Phase0_Scaffolding.md`. Ensure you follow all principles from the `copilot-instructions.md` and create the full folder structure, the complete `platformio.ini` with all library dependencies, and all stub files with their required boilerplate code.