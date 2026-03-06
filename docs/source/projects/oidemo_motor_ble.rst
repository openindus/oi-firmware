.. _Demo Motors Kit BLE:

Demo Motors Kit - Advanced BLE project
=======================================

1. Introduction
---------------

Who is this tutorial for?
~~~~~~~~~~~~~~~~~~~~~~~~~

This tutorial is intended for developers who have already completed the :ref:`Demo Motors Kit<Demo Motors Kit>`
basic tutorial and want to go further by adding **Bluetooth Low Energy (BLE)** connectivity to the project.
It covers a multi-file C++ project structure and shows how to integrate the NimBLE Arduino library into
an OpenIndus project. By the end of this tutorial, you will be able to communicate with the android app
downloaded in the previous tutorial, control the motors remotely, and monitor their state in real time.

Prerequisites
~~~~~~~~~~~~~

* Completion of the :ref:`Demo Motors Kit<Demo Motors Kit>` basic tutorial
* Familiarity with multi-file C++ projects (headers, source files, includes)
* Basic knowledge of Git (cloning a repository)
* The OpenIndus VS Code extension installed - see :ref:`Environment Installation<get_started_oivscodeextension>`
* Basic understanding of BLE concepts (GATT services and characteristics, notifications, read/write operations)

What will you learn?
~~~~~~~~~~~~~~~~~~~~

* How to structure a multi-file embedded C++ project
* How to add a third-party Arduino library (NimBLE) to an OpenIndus project via ``CMakeLists.txt``
* How to implement a BLE GATT server to expose motor state and receive remote commands
* How to control motor speed and trigger buttons remotely from the Android app

------------

2. Get the project from GitHub
-------------------------------

Rather than building the BLE project from scratch, you can clone it directly from the OpenIndus
GitHub repository. Open a terminal and run:

.. code-block:: bash

    git clone https://github.com/openindus/motor-demo-kit.git


Then open the folder in VS Code with the OpenIndus extension active. The extension will detect
the project and let you compile and flash it directly.

.. note::
    Make sure you have Git installed on your machine. If you do not, download it from
    `git-scm.com <https://git-scm.com/>`_.


------------

3. Adding the NimBLE library to the project
--------------------------------------------

The BLE functionality relies on the **NimBLE-Arduino** library. Unlike the standard Arduino
libraries bundled with OpenIndus, this one must be declared explicitly in the build system.

Download the library with git submodules
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The NimBLE-Arduino library is not copied into the repository: it is referenced as a **Git submodule**.
A submodule is a pointer to a specific commit of an external repository. This means the project source
tree stays lean and the library can be updated independently, while everyone working on the project uses
exactly the same library version.

After cloning, the ``NimBLE-Arduino/`` directory exists but is empty. You must initialise and fetch the
submodule explicitly:

.. code-block:: bash

    git submodule update --init --recursive

* ``--init`` registers any submodule that has not been set up yet in your local clone.
* ``--recursive`` also initialises submodules nested inside submodules (NimBLE-Arduino has none, but it is good practice to always use it).

Once this command completes, ``NimBLE-Arduino/src/`` is populated with the library source and the project
is ready to build.

Referencing the library in CMakeLists.txt
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Open the ``main/CMakeLists.txt`` file:

.. code-block:: cmake

    FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/main/*.*)

    idf_component_register(SRCS ${app_sources}
                           PRIV_REQUIRES openindus arduino bt NimBLE-Arduino
                           INCLUDE_DIRS "")

The key addition compared to a basic project is ``bt NimBLE-Arduino`` in the ``PRIV_REQUIRES`` list:

* ``bt`` - the ESP-IDF Bluetooth stack, required as a low-level dependency
* ``NimBLE-Arduino`` - the NimBLE Arduino wrapper library that provides the C++ BLE API used in ``ble.cpp``

``FILE(GLOB_RECURSE app_sources ...)`` automatically collects all ``.cpp`` and ``.h`` files found
in the ``main/`` folder so any new file you add to that directory is compiled automatically.

Root CMakeLists.txt - registering the submodule directory
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The root ``CMakeLists.txt`` at the project root tells ESP-IDF where to find extra components:

.. code-block:: cmake

    cmake_minimum_required(VERSION 3.16.0)

    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
    set(EXTRA_COMPONENT_DIRS components/submodules)

    idf_build_set_property(MINIMAL_BUILD ON)
    project(kit_controle_moteur)

The critical line is ``set(EXTRA_COMPONENT_DIRS components/submodules)``. ESP-IDF scans every
directory listed in ``EXTRA_COMPONENT_DIRS`` and treats each sub-folder that contains a
``CMakeLists.txt`` as a buildable component. This is how ``NimBLE-Arduino`` is discovered and
made available to the rest of the project.

``idf_build_set_property(MINIMAL_BUILD ON)`` reduces build time by skipping ESP-IDF components
that are not explicitly required by any component in the dependency graph.

NimBLE-Arduino component wrapper
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

As explained earlier, the NimBLE-Arduino library is included as a **Git submodule** (the actual library source sits in
a ``components/NimBLE-Arduino/`` folder at the repository root). To integrate it with ESP-IDF without
touching the submodule itself, a thin wrapper component lives in
``components/submodules/NimBLE-Arduino/``:

.. code-block:: cmake

    # Collect all .cpp files from the submodule source tree
    file(GLOB_RECURSE srcs "../../NimBLE-Arduino/src/*.cpp")

    # Register as an ESP-IDF component
    idf_component_register(
        SRCS ${srcs}
        INCLUDE_DIRS "../../NimBLE-Arduino/src"
        REQUIRES bt nvs_flash arduino
    )

    # NimBLE compile-time flags
    target_compile_definitions(${COMPONENT_LIB} PUBLIC
        CONFIG_BT_NIMBLE_ENABLED=1
        CONFIG_NIMBLE_CPP_IDF=1
    )

The relative path ``../../NimBLE-Arduino/src`` walks up two levels from the wrapper folder
(``components/submodules/NimBLE-Arduino/``) to reach the actual library source at the repository
root. This keeps the component registration entirely outside of the submodule directory so that
pulling library updates with ``git submodule update`` never overwrites build configuration.

The overall folder layout is therefore:

.. code-block:: text

    project/
    ├── CMakeLists.txt                          ← sets EXTRA_COMPONENT_DIRS
    ├── main/
    │   ├── CMakeLists.txt                      ← requires NimBLE-Arduino
    │   ├── main.cpp
    │   ├── ble.cpp / ble.h
    │   ├── commands.cpp / commands.h
    │   └── system_definition.h
    └── components/
        ├── openindus/                          ← standard OpenIndus component (untouched)
        ├── arduino/                            ← standard OpenIndus Arduino component (untouched)
        ├── NimBLE-Arduino/                     ← git submodule (untouched)
        |   └── src/
        └── submodules/
            └── NimBLE-Arduino/
                └── CMakeLists.txt              ← wrapper (owned by the project)

------------

4. Project structure
---------------------

The BLE project is split into four files under the ``main/`` directory, each with a distinct
responsibility:

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - File
     - Role
   * - ``system_definition.h``
     - Pin name constants and shared data structures (``MotorStatus_t``, ``SetAdvancedParamArgs_s``)
   * - ``commands.h`` / ``commands.cpp``
     - Hardware abstraction layer: motor control, LED tasks, button events, OI module declarations
   * - ``ble.h`` / ``ble.cpp``
     - BLE GATT server: service and characteristic setup, notification loop, remote command callbacks
   * - ``main.cpp``
     - Arduino entry point: calls``setup()`` / ``loop()``, wires together ``commands`` and ``ble`` layers

This separation keeps ``main.cpp`` clean and readable. Adding new hardware features only requires
changes to ``commands.cpp``, and extending the BLE interface only requires changes to ``ble.cpp``.

system_definition.h - shared constants and types
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

All pin assignments and shared data types live in a single header included by every other file.
This avoids duplicating the definitions across ``commands.cpp`` and ``ble.cpp``:

.. code-block:: cpp

    // Output LEDs
    const DOut_Num_t LEFT_LED_OUT  = DOUT_4;
    const DOut_Num_t RIGHT_LED_OUT = DOUT_3;

    // Input buttons
    const DIn_Num_t LEFT_BUTTON  = DIN_2;
    const DIn_Num_t RIGHT_BUTTON = DIN_1;

    // Motor channels
    const MotorNum_t MOTOR_LEFT  = MOTOR_2;
    const MotorNum_t MOTOR_RIGHT = MOTOR_1;

    // Limit switches
    const DIn_Num_t LEFT_MOTOR_HOME_SWITCH  = DIN_3;
    const DIn_Num_t LEFT_MOTOR_END_SWITCH   = DIN_4;
    const DIn_Num_t RIGHT_MOTOR_HOME_SWITCH = DIN_1;
    const DIn_Num_t RIGHT_MOTOR_END_SWITCH  = DIN_2;

    // Shared types used by both commands and BLE layers
    typedef struct MotorStatus {
        int32_t position;
        float   speed;
    } MotorStatus_t;

    typedef struct SetAdvancedParamArgs {
        uint8_t pwmDec;
        uint8_t pwmInt;
        uint8_t vsComp;
        uint8_t stepMode;
        float   kvalHold;
        float   kvalRun;
        float   kvalAcc;
        float   kvalDec;
        float   intSpeed;
        float   fnSlopeAcc;
        float   fnSlopeDec;
        float   stSlope;
    } SetAdvancedParamArgs_s;

``MotorStatus_t`` bundles the position (in steps) and the current speed into a single struct so
it can be sent as a single BLE notification payload. ``SetAdvancedParamArgs_s`` packs all of the
advanced motor driver registers that the app can read or update over BLE.

commands.cpp - hardware abstraction layer
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``commands.cpp`` owns all interactions with the OpenIndus hardware. The OI module objects are
declared here, not in ``main.cpp``:

.. code-block:: cpp

    OICore   core;
    OIStepper stepper;

This keeps ``main.cpp`` free of hardware detail. All other files access the hardware exclusively
through the functions declared in ``commands.h``.

Key state variables managed by commands:

.. code-block:: cpp

    volatile bool buttonEvent         = false; // set by ISR, read by loop()
    volatile bool bleLeftButtonPressed  = false; // set by BLE write callback
    volatile bool bleRightButtonPressed = false;
    volatile float bleLeftMotorSpeed  = 100;   // steps/s, updated over BLE
    volatile float bleRightMotorSpeed = 100;

All four variables are ``volatile`` because they are written from interrupt contexts (hardware ISR
or BLE callback tasks running on a separate RTOS task) and read from the Arduino main loop.

``isLeftButtonPressed()`` merges both the physical and the virtual BLE button states with a
logical OR so the motor reacts to either source:

.. code-block:: cpp

    bool isLeftButtonPressed() {
        return core.digitalRead(LEFT_BUTTON) || bleLeftButtonPressed;
    }

``runMotor()`` picks the BLE-commanded speed before calling the stepper:

.. code-block:: cpp

    void runMotor(MotorNum_t motor, MotorDirection_t direction) {
        float speed = (motor == MOTOR_LEFT) ? bleLeftMotorSpeed : bleRightMotorSpeed;
        stepper.setMaxSpeed(motor, speed);
        stepper.run(motor, direction, speed);
    }

main.cpp - entry point
~~~~~~~~~~~~~~~~~~~~~~

``setup()`` calls the init functions from ``commands.cpp`` and ``ble.cpp`` in order:

.. code-block:: cpp

    void setup() {
        Serial.begin(115200);

        initMotorsParameters();  // configure voltage, step mode, ramps
        initLEDsTasks();         // create FreeRTOS tasks for LED blinking
        initMotors();            // homing sequence + startup animation
        attachInterruptions();   // register button ISRs
        bleSetup();              // start NimBLE GATT server
    }

``loop()`` is intentionally minimal. It handles the button event (physical or BLE-triggered) and
then calls ``bleloop()`` to send BLE notifications:

.. code-block:: cpp

    void loop() {
        if (isButtonEvent()) {
            clearEvent();
            stopLeftLedBlinking();
            stopRightLedBlinking();

            bool leftAction  = isLeftButtonPressed();
            bool rightAction = isRightButtonPressed();

            if (leftAction && !rightAction) {
                runMotor(MOTOR_LEFT, motor1_dir);
                stopMotor(MOTOR_RIGHT, SOFT_HIZ);
                motor2_dir = reverse_motdir(motor2_dir);
                startLeftLedBlinking();
            } else if (!leftAction && rightAction) {
                runMotor(MOTOR_RIGHT, motor2_dir);
                stopMotor(MOTOR_LEFT, SOFT_HIZ);
                motor1_dir = reverse_motdir(motor1_dir);
                startRightLedBlinking();
            } else {
                stopMotor(MOTOR_LEFT,  SOFT_HIZ);
                stopMotor(MOTOR_RIGHT, SOFT_HIZ);
            }
        }
        bleloop();
        delay(50);
    }

------------

5. BLE GATT server
-------------------

The BLE layer exposes a single GATT service with UUID ``A100`` that groups all motor-related
characteristics. It is set up in ``bleSetup()`` and maintained by ``bleloop()``.

Service and characteristics overview
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 15 45 15 25

   * - UUID
     - Description
     - Type
     - Properties
   * - ``A100``
     - Motors service
     - Service
     - —
   * - ``A101``
     - Left button state
     - ``uint8_t``
     - Read / Write / Notify
   * - ``A102``
     - Right button state
     - ``uint8_t``
     - Read / Write / Notify
   * - ``A103``
     - Left motor state (position + speed)
     - ``MotorStatus_t``
     - Read / Notify
   * - ``A104``
     - Right motor state (position + speed)
     - ``MotorStatus_t``
     - Read / Notify
   * - ``A105``
     - Left motor home switch state
     - ``uint8_t``
     - Read / Notify
   * - ``A106``
     - Left motor end switch state
     - ``uint8_t``
     - Read / Notify
   * - ``A107``
     - Right motor home switch state
     - ``uint8_t``
     - Read / Notify
   * - ``A108``
     - Right motor end switch state
     - ``uint8_t``
     - Read / Notify
   * - ``A109``
     - Left motor target speed (steps/s)
     - ``float``
     - Write
   * - ``A10A``
     - Right motor target speed (steps/s)
     - ``float``
     - Write
   * - ``A10B``
     - Advanced motor parameters
     - ``SetAdvancedParamArgs_s``
     - Read / Write

The **Write** property on the button characteristics (``A101``, ``A102``) is what allows the
Android app to simulate a button press remotely: writing ``1`` sets ``bleLeftButtonPressed = true``
and triggers the same event path as a physical button press.

bleSetup() - GATT server initialization
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``bleSetup()`` initializes the NimBLE stack, creates the server, then builds each characteristic
and assigns its callback handler before starting the service and advertising:

.. code-block:: cpp

    void bleSetup(void) {
        NimBLEDevice::init("NimBLE");
        NimBLEDevice::setSecurityAuth(BLE_SM_PAIR_AUTHREQ_SC);
        pServer = NimBLEDevice::createServer();
        pServer->setCallbacks(&serverCallbacks);

        NimBLEService* pDemoIosService = pServer->createService(SERVICE_UUID_MOTORS);

        // Left button - readable, writable (remote press) and notifiable
        NimBLECharacteristic* pButtonCharacteristic = pDemoIosService->createCharacteristic(
            CHAR_UUID_LEFT_BUTTON,
            NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::WRITE,
            sizeof(uint8_t)
        );
        pButtonCharacteristic->setCallbacks(&leftButtonChrCallback);

        // ... (other characteristics follow the same pattern)

        pDemoIosService->start();

        NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
        pAdvertising->setName("OI Motors Kit");
        pAdvertising->addServiceUUID(pDemoIosService->getUUID());
        pAdvertising->start();
    }

The device advertises itself as **"OI Motors Kit"** — the name you will see when the Android app
scans for nearby devices.

Characteristic callbacks
~~~~~~~~~~~~~~~~~~~~~~~~

Each writable characteristic has a dedicated callback class that inherits from
``NimBLECharacteristicCallbacks``. The pattern is always the same: extract the raw bytes from the
characteristic value and call the appropriate function from ``commands.h``.

**Remote button press** (``LeftButtonCharacteristicCallbacks``):

.. code-block:: cpp

    class LeftButtonCharacteristicCallbacks : public CharacteristicCallbacks {
        void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override {
            std::string dscVal = pCharacteristic->getValue();
            bool value = (!dscVal.empty()) ? (bool)dscVal[0] : false;
            setLeftButtonPressed(value);   // triggers buttonEvent = true in commands.cpp
        }
    };

**Remote speed update** (``LeftMotorSetSpeedCharacteristicCallbacks``):

.. code-block:: cpp

    class LeftMotorSetSpeedCharacteristicCallbacks : public CharacteristicCallbacks {
        void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override {
            std::string dscVal = pCharacteristic->getValue();
            float value = 0;
            if (dscVal.size() >= sizeof(float)) {
                value = *(float*)dscVal.data();   // raw 4-byte IEEE 754 float
            }
            setMotorSpeed(MOTOR_LEFT, value);     // stored in bleLeftMotorSpeed
        }
    };

The speed is transmitted as a raw 4-byte IEEE 754 float (little-endian). The Android application
must pack it the same way before writing the characteristic.

bleloop() - sending notifications
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``bleloop()`` is called every 50 ms from ``loop()``. It checks whether any client is connected and,
for each characteristic, compares the current value with the last notified value. A notification is
sent only when something actually changed, keeping the BLE traffic minimal:

.. code-block:: cpp

    void bleloop() {
        if (pServer->getConnectedCount()) {
            NimBLEService* pSvc = pServer->getServiceByUUID(SERVICE_UUID_MOTORS);
            if (pSvc) {
                // Example: left motor state
                NimBLECharacteristic* pChr = pSvc->getCharacteristic(CHAR_UUID_LEFT_MOTOR_STATE);
                if (pChr) {
                    static MotorStatus_t lastLeftMotorStatus{};
                    MotorStatus_t motorStatus = getMotorStatus(MOTOR_LEFT);
                    if (!areStatusEqual(motorStatus, lastLeftMotorStatus)) {
                        lastLeftMotorStatus = motorStatus;
                        pChr->setValue((uint8_t*)&motorStatus, sizeof(MotorStatus_t));
                        pChr->notify();
                    }
                }
                // ... (identical pattern for every other characteristic)
            }
        }
    }

The ``static`` keyword inside the ``if`` block means each ``last*`` variable retains its value
between calls to ``bleloop()`` without needing a global variable.

Connection and security callbacks
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``ServerCallbacks`` handles connection lifecycle events. On connect, it requests updated
connection parameters to balance latency and power consumption. On disconnect, it restarts
advertising so the device is immediately discoverable again:

.. code-block:: cpp

    void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override {
        pServer->updateConnParams(connInfo.getConnHandle(), 24, 48, 0, 180);
    }

    void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override {
        NimBLEDevice::startAdvertising();
    }

------------

6. Adapting the project to your own needs
------------------------------------------

This project is designed to be a starting point. Here are the most common customisations:

* **Add a new BLE characteristic** - declare its UUID in ``ble.h``, create it in ``bleSetup()``,
  add its logic to ``bleloop()``, and expose a helper function in ``commands.h`` / ``commands.cpp``.

* **Add a new hardware function** - implement it in ``commands.cpp`` and declare it in ``commands.h``.
  ``main.cpp`` and ``ble.cpp`` can then call it without knowing any hardware details.

* **Rename the BLE device** - change ``"OI Motors Kit"`` in ``pAdvertising->setName(...)`` inside
  ``bleSetup()`` to any name you like.

* **Change motor parameters** - edit the ``initMotorsParameters()`` function in ``commands.cpp``.
  The advanced parameters can also be updated at runtime by the Android app through characteristic ``A10B``.
