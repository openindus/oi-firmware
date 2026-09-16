.. _Demo Input-Output Kit BLE:

Demo Input/Output Kit - Advanced BLE project
=============================================

1. Introduction
---------------

Who is this tutorial for?
~~~~~~~~~~~~~~~~~~~~~~~~~

This tutorial is intended for developers who have already completed the :ref:`Demo Input/Output Kit<Demo Input-Output Kit>`
basic tutorial and want to go further by adding **Bluetooth Low Energy (BLE)** connectivity to the project.
It covers a multi-file C++ project structure and shows how to integrate the NimBLE Arduino library into
an OpenIndus project. By the end of this tutorial, you will be able to communicate with the Android app
downloaded in the previous tutorial, monitor sensor values remotely, and control the display mode in real time.

Prerequisites
~~~~~~~~~~~~~

* Completion of the :ref:`Demo Input/Output Kit<Demo Input-Output Kit>` basic tutorial
* Familiarity with multi-file C++ projects (headers, source files, includes)
* Basic knowledge of Git (cloning a repository)
* The OpenIndus VS Code extension installed - see :ref:`Environment Installation<get_started_oivscodeextension>`
* Basic understanding of BLE concepts (GATT services and characteristics, notifications, read/write operations)

What will you learn?
~~~~~~~~~~~~~~~~~~~~

* How to structure a multi-file embedded C++ project
* How to add a third-party Arduino library (NimBLE) to an OpenIndus project via ``CMakeLists.txt``
* How to implement a BLE GATT server to expose sensor data and receive remote commands
* How to monitor temperature, humidity, distance, RGB color and potentiometer values remotely from the Android app

------------

2. Get the project from GitHub
-------------------------------

Rather than building the BLE project from scratch, you can clone it directly from the OpenIndus
GitHub repository. Open a terminal and run:

.. code-block:: bash

    git clone https://github.com/openindus/io-demo-kit.git


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

Referencing the libraries in CMakeLists.txt
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Open the ``main/CMakeLists.txt`` file:

.. code-block:: cmake

    FILE(GLOB_RECURSE app_sources ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

    idf_component_register(SRCS ${app_sources}
                           INCLUDE_DIRS .
                           PRIV_REQUIRES openindus arduino bt NimBLE-Arduino
                           REQUIRES Adafruit-GFX-Library Adafruit_SSD1306 Adafruit_Sensor Adafruit_AM2320)

The key additions compared to a basic project are:

* ``bt NimBLE-Arduino`` in the ``PRIV_REQUIRES`` list — the ESP-IDF Bluetooth stack and the NimBLE Arduino wrapper library that provides the C++ BLE API used in ``ble.cpp``
* ``Adafruit-GFX-Library Adafruit_SSD1306 Adafruit_Sensor Adafruit_AM2320`` in the ``REQUIRES`` list — the Adafruit libraries needed for the OLED display and the temperature/humidity sensor

``FILE(GLOB_RECURSE app_sources ...)`` automatically collects all ``.cpp`` files found
in the ``main/`` folder so any new file you add to that directory is compiled automatically.

NimBLE-Arduino component wrapper
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The NimBLE-Arduino library is included directly in
a ``lib/NimBLE-Arduino/`` folder. To make it available to the ESP-IDF build system, a ``CMakeLists.txt`` file is added to the root of the NimBLE-Arduino folder with the following content:

.. code-block:: cmake

    # Collect all .cpp files from submodule
    file(GLOB_RECURSE srcs "./src/*.cpp")

    # Register ESP-IDF component
    idf_component_register(
        SRCS ${srcs}
        INCLUDE_DIRS "./src"
        REQUIRES bt nvs_flash arduino
    )

    # NimBLE specific configuration
    target_compile_definitions(${COMPONENT_LIB} PUBLIC 
        CONFIG_BT_NIMBLE_ENABLED=1
        CONFIG_NIMBLE_CPP_IDF=1
    )

The overall folder layout is therefore:

.. code-block:: text

    project/
    ├── CMakeLists.txt                          ← sets EXTRA_COMPONENT_DIRS
    ├── main/
    │   ├── CMakeLists.txt                      ← requires NimBLE-Arduino + Adafruit libs
    │   ├── main.cpp
    │   ├── ble.cpp / ble.h
    │   └── inouts.cpp / inouts.h
    ├── components/
    │   ├── openindus/                          ← standard OpenIndus component (untouched)
    │   ├── arduino/                            ← standard OpenIndus Arduino component (untouched)
    └── lib/
        ├── Adafruit_AM2320/
        ├── Adafruit_BusIO/
        ├── Adafruit_GFX_Library/
        ├── Adafruit_Sensor/
        ├── Adafruit_SSD1306/
        └── NimBLE-Arduino/                     
            ├── src/
            └── CMakeLists.txt                  ← Added to NimBLE-Arduino to register it as an ESP-IDF component

------------

4. Project structure
---------------------

The BLE project is split into three files under the ``main/`` directory, each with a distinct
responsibility:

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - File
     - Role
   * - ``inouts.h`` / ``inouts.cpp``
     - Hardware abstraction layer: OI module declarations, pin constants, sensor reads, actuator control, interrupt handlers
   * - ``ble.h`` / ``ble.cpp``
     - BLE GATT server: service and characteristic setup, notification loop, remote command callbacks
   * - ``main.cpp``
     - Arduino entry point: calls ``setup()`` / ``loop()``, wires together ``inouts`` and ``ble`` layers, OLED display logic and FreeRTOS tasks

This separation keeps ``main.cpp`` clean and readable. Adding new hardware features only requires
changes to ``inouts.cpp``, and extending the BLE interface only requires changes to ``ble.cpp``.

inouts.h - hardware abstraction interface
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``inouts.h`` declares all the functions that ``main.cpp`` and ``ble.cpp`` use to interact with
the hardware. The doxygen-style comments document each function's purpose and return type:

.. code-block:: cpp

    int get_display_mode();
    void displaymodechange(void*);
    uint8_t get_button();
    void setupIOs(void inductive_sensor_callback(void*));
    int get_inductive_sensor();
    void set_blue_led(uint8_t level);
    float get_temperature();
    float get_humidity();
    float measuredistance();
    void set_pwm_white_led(int level);
    void set_rgb_pwm(int currentColorValueRed, int currentColorValueGreen, int currentColorValueBlue);
    uint32_t get_rgb();
    int get_pot_pin_value();

By including only this header, ``ble.cpp`` and ``main.cpp`` can call any hardware function without
knowing the underlying pin assignments or OI module API details.

inouts.cpp - hardware abstraction layer
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``inouts.cpp`` owns all interactions with the OpenIndus hardware. The OI module objects are
declared here, not in ``main.cpp``:

.. code-block:: cpp

    OICore core;
    OIMixed mixed1;
    Adafruit_AM2320 th_sensor = Adafruit_AM2320();

This keeps ``main.cpp`` free of hardware detail. All other files access the hardware exclusively
through the functions declared in ``inouts.h``.

Key constants and pin assignments:

.. code-block:: cpp

    // Input pins
    const DinNum_t BUTTON_PIN = DIN_1;
    const DinNum_t INDUCTIVE_SENSOR_PIN = DIN_2;
    const AnalogInput_Num_t POTENTIOMETER_PIN = AIN_1;
    const AnalogInput_Num_t PROXIMITY_SENSOR_PIN = AIN_2;

    // Output pins
    const DoutNum_t RED_LED_PIN = DOUT_1;
    const DoutNum_t GREEN_LED_PIN = DOUT_2;
    const DoutNum_t BLUE_LED_PIN = DOUT_3;
    const DoutNum_t WHITE_LED_PIN = DOUT_4;

The ``display_mode`` variable is managed internally by ``inouts.cpp`` with a software debounce
mechanism (same approach as in the basic tutorial). It is exposed through ``get_display_mode()``
and can be advanced remotely via ``displaymodechange()``.

``setupIOs()`` configures all analog inputs, PWM outputs, and interrupt handlers in one call:

.. code-block:: cpp

    void setupIOs(void inductive_sensor_callback(void*))
    {
        // Potentiometer: voltage mode, 0-5.12 V range
        mixed1.analogInputMode(POTENTIOMETER_PIN, AIN_MODE_VOLTAGE);
        mixed1.analogInputVoltageRange(POTENTIOMETER_PIN, AIN_VOLTAGE_RANGE_0_5V12);
        // Proximity sensor: current mode (4-20 mA)
        mixed1.analogInputMode(PROXIMITY_SENSOR_PIN, AIN_MODE_CURRENT);

        // RGB LED: PWM mode at 100 Hz
        mixed1.outputMode(RED_LED_PIN, DOUT_MODE_PWM);
        mixed1.outputMode(GREEN_LED_PIN, DOUT_MODE_PWM);
        mixed1.outputMode(BLUE_LED_PIN, DOUT_MODE_PWM);
        mixed1.setPWMFrequency(RED_LED_PIN, 100);
        mixed1.setPWMFrequency(GREEN_LED_PIN, 100);
        mixed1.setPWMFrequency(BLUE_LED_PIN, 100);
        mixed1.setPWMDutyCycle(RED_LED_PIN, 50);
        mixed1.setPWMDutyCycle(GREEN_LED_PIN, 100);
        mixed1.setPWMDutyCycle(BLUE_LED_PIN, 10);

        // White LED: PWM mode at 100 Hz, initially off
        mixed1.outputMode(WHITE_LED_PIN, DOUT_MODE_PWM);
        mixed1.setPWMFrequency(WHITE_LED_PIN, 100);
        mixed1.setPWMDutyCycle(WHITE_LED_PIN, 0);

        // Interrupts
        core.attachInterrupt(BUTTON_PIN, displaymodechange, RISING_MODE, NULL);
        core.attachInterrupt(INDUCTIVE_SENSOR_PIN, inductive_sensor_callback, CHANGE_MODE, NULL);
    }

The ``measuredistance()`` function acquires multiple samples from the 4-20 mA proximity sensor,
computes the median, and clamps the result to the 10–100 mm range:

.. code-block:: cpp

    float measuredistance()
    {
        const int nSAMPLES = 10;
        static std::vector<float> rdist(nSAMPLES);

        for (int i = 0; i < nSAMPLES; i++) {
            rdist[i] = 5.625 * mixed1.analogReadMilliAmp(PROXIMITY_SENSOR_PIN) - 12.5;
            delay(5);
        }
        std::sort(rdist.begin(), rdist.end());
        size_t size = rdist.size();
        float meandistance = (size % 2 == 0)
            ? (rdist[size / 2 - 1] + rdist[size / 2]) / 2.0
            : rdist[size / 2];

        if (meandistance > 100) return 100;
        if (meandistance < 10) return 10;
        return meandistance;
    }

``set_rgb_pwm()`` applies the three duty-cycle values and packs the result into a ``0xRRGGBB``
integer that the BLE layer can read via ``get_rgb()``:

.. code-block:: cpp

    void set_rgb_pwm(int currentColorValueRed, int currentColorValueGreen, int currentColorValueBlue)
    {
        mixed1.setPWMDutyCycle(RED_LED_PIN, currentColorValueRed);
        mixed1.setPWMDutyCycle(GREEN_LED_PIN, currentColorValueGreen);
        mixed1.setPWMDutyCycle(BLUE_LED_PIN, currentColorValueBlue);

        // Convert 0-100 to 0-255 and pack into 0xRRGGBB
        uint8_t r8 = (uint8_t)((currentColorValueRed * 255) / 100);
        uint8_t g8 = (uint8_t)((currentColorValueGreen * 255) / 100);
        uint8_t b8 = (uint8_t)((currentColorValueBlue * 255) / 100);
        current_rgb_color = (r8 << 16) | (g8 << 8) | b8;
    }

main.cpp - entry point
~~~~~~~~~~~~~~~~~~~~~~

``main.cpp`` contains the OLED display logic, the FreeRTOS background tasks, and the Arduino
entry points. It includes only ``inouts.h``, ``ble.h``, and the Adafruit display headers:

.. code-block:: cpp

    #include "inouts.h"
    #include "ble.h"
    #include "Adafruit_SSD1306.h"
    #include "Fonts/FreeMono9pt7b.h"

**Global variables and display constants:**

.. code-block:: cpp

    #define PWM_MAX_VAL     100
    #define SCREEN_WIDTH    128
    #define SCREEN_HEIGHT   64
    #define OLED_RESET      -1
    #define SCREEN_ADDRESS  0x3D

    Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
    float meandistance;

``PWM_MAX_VAL`` is set to 100 to match the duty-cycle percentage scale used in this
multi-file version (as opposed to the 14-bit raw value in the single-file basic tutorial).

**Inductive sensor callback:**

.. code-block:: cpp

    void inductive_sensor_callback(void*)
    {
        static int inductive_sensor;
        inductive_sensor = get_inductive_sensor();
        if (inductive_sensor == 1)
            set_blue_led(HIGH);
        else
            set_blue_led(LOW);
    }

This interrupt callback is passed to ``setupIOs()`` and mirrors the inductive sensor state onto
the blue LED of the OI-Core, just as in the basic tutorial.

**Display helper functions** (``displaytemperature()``, ``displayhumidity()``, ``displaydistance()``)
each clear the OLED and draw a single measurement. They call ``get_temperature()``,
``get_humidity()``, and use the global ``meandistance`` variable respectively.

**FreeRTOS tasks:**

* ``managedistance()`` — continuously reads the proximity sensor via ``measuredistance()`` and
  adjusts the white LED brightness proportionally to the measured distance.
* ``rgb_pot()`` — continuously reads the potentiometer via ``get_pot_pin_value()`` and maps the
  value to an RGB color wheel, driving the RGB LED via ``set_rgb_pwm()``.

**setup():**

.. code-block:: cpp

    void setup(void)
    {
        Serial.begin(115200);
        if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
            Serial.println(F("SSD1306 allocation failed"));
            for (;;);
        }
        display.setFont((const GFXfont *)&FreeMono9pt7b);
        display.display();
        delay(2000);
        display.clearDisplay();

        setupIOs(inductive_sensor_callback);

        xTaskCreate(managedistance, "Measure distance", 20000, NULL, 1, NULL);
        xTaskCreate(rgb_pot, "rgb_pot", 10000, NULL, 2, NULL);

        blesetup();

        Serial.println(F("Setup done."));
    }

``setup()`` initializes the serial port, the OLED display, all I/O through ``setupIOs()``, spawns
the two background FreeRTOS tasks, and finally starts the BLE GATT server with ``blesetup()``.

**loop():**

.. code-block:: cpp

    void loop(void)
    {
        switch (get_display_mode()) {
            case 0:
            default:
                displaytemperature();
                break;
            case 1:
                displayhumidity();
                break;
            case 2:
                displaydistance();
                break;
        }
        bleloop();
        delay(100);
    }

``loop()`` cycles through the three display modes (temperature, humidity, distance) based on the
value returned by ``get_display_mode()``, then calls ``bleloop()`` to send BLE notifications.
The ``delay(100)`` yields the CPU to the FreeRTOS scheduler so that the background tasks get
regular execution time.

------------

5. BLE GATT server
-------------------

The BLE layer exposes a single GATT service with UUID ``A000`` that groups all I/O-related
characteristics. It is set up in ``blesetup()`` and maintained by ``bleloop()``.

Service and characteristics overview
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 15 45 15 25

   * - UUID
     - Description
     - Type
     - Properties
   * - ``A000``
     - Demo IOs service
     - Service
     - —
   * - ``0000``
     - Push button state / display mode change
     - ``uint8_t``
     - Read / Write / Notify
   * - ``0001``
     - Temperature (°C)
     - ``float``
     - Read / Notify
   * - ``0002``
     - Humidity (%)
     - ``float``
     - Read / Notify
   * - ``0003``
     - Distance (mm)
     - ``float``
     - Read / Notify
   * - ``0004``
     - RGB LED color (``0xRRGGBB``)
     - ``uint32_t``
     - Read / Notify
   * - ``0005``
     - Potentiometer value (0–100)
     - ``uint8_t``
     - Read / Notify
   * - ``0006``
     - Inductive sensor state
     - ``uint8_t`` (boolean)
     - Read / Notify

The **Write** property on the button characteristic (``0000``) is what allows the Android app to
simulate a button press remotely: writing to this characteristic calls ``displaymodechange()``
and advances the OLED display mode, just as a physical button press would.

ble.h - UUID definitions
~~~~~~~~~~~~~~~~~~~~~~~~~

All service and characteristic UUIDs are defined as macros in ``ble.h``, along with shorthand
property aliases:

.. code-block:: cpp

    #define ble_R NIMBLE_PROPERTY::READ
    #define ble_W NIMBLE_PROPERTY::WRITE
    #define ble_N NIMBLE_PROPERTY::NOTIFY

    #define SERVICE_UUID           "A000"
    #define CHAR_UUID_BUTTON       "0000"
    #define CHAR_UUID_TEMPERATURE  "0001"
    #define CHAR_UUID_HUMIDITY     "0002"
    #define CHAR_UUID_DISTANCE     "0003"
    #define CHAR_UUID_RGB          "0004"
    #define CHAR_UUID_POT          "0005"
    #define CHAR_UUID_INDUCTIVE    "0006"

    void blesetup(void);
    void bleloop(void);

blesetup() - GATT server initialization
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``blesetup()`` initializes the NimBLE stack, creates the server, then builds each characteristic
and assigns its callback handler before starting the service and advertising:

.. code-block:: cpp

    void blesetup(void) {
        NimBLEDevice::init("NimBLE");
        NimBLEDevice::setSecurityAuth(BLE_SM_PAIR_AUTHREQ_SC);
        pServer = NimBLEDevice::createServer();
        pServer->setCallbacks(&serverCallbacks);

        NimBLEService* pDemoIosService = pServer->createService(SERVICE_UUID);

        // Button - readable, writable (remote press) and notifiable
        NimBLECharacteristic* pButtonCharacteristic = pDemoIosService->createCharacteristic(
            CHAR_UUID_BUTTON,
            NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::WRITE,
            sizeof(uint8_t)
        );
        pButtonCharacteristic->setValue(get_button());
        pButtonCharacteristic->setCallbacks(&buttonChrCallbacks);

        // Temperature - read and notify only
        NimBLECharacteristic* pTemperatureCharacteristic = pDemoIosService->createCharacteristic(
            CHAR_UUID_TEMPERATURE,
            NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY,
            sizeof(float)
        );
        pTemperatureCharacteristic->setValue(get_temperature());

        // ... (humidity, distance, RGB, potentiometer, inductive follow the same pattern)

        pDemoIosService->start();

        NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
        pAdvertising->setName("OI Demo IOs");
        pAdvertising->addServiceUUID(pDemoIosService->getUUID());
        pAdvertising->enableScanResponse(true);
        pAdvertising->start();
    }

The device advertises itself as **"OI Demo IOs"** — the name you will see when the Android app
scans for nearby devices.

Each characteristic is created with:

* A **UUID** from ``ble.h``
* **Properties** that define whether it can be read, written, or notified
* A **size** matching the data type (``sizeof(float)`` for sensor values, ``sizeof(uint8_t)`` for boolean states, ``sizeof(uint32_t)`` for the RGB color)
* A **NimBLE2904** descriptor that advertises the data format to BLE clients (e.g. ``FORMAT_FLOAT32``, ``FORMAT_UINT8``, ``FORMAT_UINT32``, ``FORMAT_BOOLEAN``)

Characteristic callbacks
~~~~~~~~~~~~~~~~~~~~~~~~

The only writable characteristic is the button (``0000``). It has a dedicated callback class
that calls ``displaymodechange()`` from ``inouts.h`` to advance the display mode:

.. code-block:: cpp

    class buttonCharacteristicCallbacks : public CharacteristicCallbacks {
        void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override {
            std::string dscVal = pCharacteristic->getValue();
            Serial.printf("Button written value: %s\n", dscVal.c_str());
            displaymodechange(NULL);
        }
    } buttonChrCallbacks;

Writing any value to this characteristic triggers a display mode change, cycling through
temperature → humidity → distance, exactly as pressing the physical push button does.

All other characteristics (temperature, humidity, distance, RGB, potentiometer, inductive sensor)
are read-only from the client's perspective and use the generic ``CharacteristicCallbacks`` base
class for logging reads and subscriptions.

bleloop() - sending notifications
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``bleloop()`` is called every 100 ms from ``loop()``. It checks whether any client is connected and,
for each characteristic, compares the current value with the last notified value. A notification is
sent only when something actually changed, keeping the BLE traffic minimal:

.. code-block:: cpp

    void bleloop() {
        if (pServer->getConnectedCount()) {
            NimBLEService* pSvc = pServer->getServiceByUUID(SERVICE_UUID);
            if (pSvc) {
                // Button state
                NimBLECharacteristic* pChr = pSvc->getCharacteristic(CHAR_UUID_BUTTON);
                if (pChr) {
                    static uint8_t lastButtonState = 0;
                    uint8_t buttonState = get_button();
                    if (buttonState != lastButtonState) {
                        lastButtonState = buttonState;
                        pChr->setValue(buttonState);
                        pChr->notify();
                    }
                }

                // Temperature (notify only if >= 0.1 °C change)
                pChr = pSvc->getCharacteristic(CHAR_UUID_TEMPERATURE);
                if (pChr) {
                    static float lastTemperature = 0;
                    float temperature = get_temperature();
                    pChr->setValue(temperature);
                    if (abs(temperature - lastTemperature) >= 0.1) {
                        lastTemperature = temperature;
                        pChr->notify();
                    }
                }

                // ... (identical pattern for humidity, distance, RGB, pot, inductive)
            }
        }
    }

The ``static`` keyword inside each ``if`` block means each ``last*`` variable retains its value
between calls to ``bleloop()`` without needing a global variable.

Notification thresholds vary by characteristic type:

* **Button and inductive sensor** — notify on any state change
* **Temperature and humidity** — notify when the delta exceeds 0.1 (°C or %)
* **Distance** — notify when the delta exceeds 0.01 mm
* **RGB and potentiometer** — notify on any value change

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

* **Add a new BLE characteristic** — declare its UUID in ``ble.h``, create it in ``blesetup()``,
  add its notification logic to ``bleloop()``, and expose a helper function in ``inouts.h`` / ``inouts.cpp``.

* **Add a new hardware function** — implement it in ``inouts.cpp`` and declare it in ``inouts.h``.
  ``main.cpp`` and ``ble.cpp`` can then call it without knowing any hardware details.

* **Rename the BLE device** — change ``"OI Demo IOs"`` in ``pAdvertising->setName(...)`` inside
  ``blesetup()`` to any name you like.

* **Add a writable characteristic** — follow the ``buttonCharacteristicCallbacks`` pattern: create
  a callback class, override ``onWrite()``, extract the value, and call the appropriate function
  from ``inouts.h``.

* **Change sensor configuration** — edit the ``setupIOs()`` function in ``inouts.cpp``. For example,
  change the analog input range or PWM frequency to match new hardware.
