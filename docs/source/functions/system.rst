.. _system_s:

System
======

Description
-----------

The OpenIndus system follows a classic Arduino execution flow, with a hardware initialization layer specific to industrial modules.

Execution Flow
~~~~~~~~~~~~~~

The system starts with the entry point ``app_main()``, which initializes and starts the system:

1. **Initialization**: ``System::init()`` initializes hardware and checks configuration
2. **Start**: ``System::start()`` launches the main task that runs your user code
3. **Execution**: The ``setup()`` and ``loop()`` functions are called automatically

System Initialization
~~~~~~~~~~~~~~~~~~~~~

The ``System::init()`` function performs the following operations:

- Initializes hardware according to the detected module (Core, Discrete, Stepper, Mixed, RelayHP, AnalogLS, Dc)
- Configures Master or Slave mode
- For Master modules: resets connected modules and performs auto-identification
- Checks for startup errors
- Indicates status via LEDs:

  - Blinking blue LED: hardware initialized successfully
  - Blinking green LED: module auto-identification successful (Master only)
  - Blinking red LED: error detected

Error Codes
~~~~~~~~~~~

The system uses the following error codes:

- ``ERROR_NONE`` (0): no error
- ``ERROR_HARDWARE_INIT`` (-1): hardware initialization failed
- ``ERROR_AUTO_ID`` (-2): module auto-identification failed (Master)
- ``ERROR_BOOT`` (-3): startup error detected

Error Handling
~~~~~~~~~~~~~~

**If an initialization error occurs, the system calls the ``handleError(errorCode)`` function.**
By default, this function simply starts the user code (``setup()`` and ``loop()``).

You can override the ``handleError()`` function to customize error handling:

.. code-block:: cpp

    void System::handleError(int errorCode)
    {
        // Your custom error handling
        if (errorCode == ERROR_HARDWARE_INIT) {
            // Handle hardware initialization error
        }
        // Default: restart after 5 seconds
        delay(5000);
        esp_restart();
    }

.. warning::
    It is recommended to override the ``handleError()`` function to implement a recovery strategy suited to your application (restart, degraded mode, etc.).

User Code
---------

As with Arduino, you write your code in two functions:

setup()
~~~~~~~

Initialization function called once at startup.

.. code-block:: cpp

    void setup(void)
    {
        // Your initialization code
        printf("Hello OpenIndus!\n");
    }

loop()
~~~~~~

Function called in an infinite loop after ``setup()``.

.. code-block:: cpp

    void loop(void)
    {
        // Your main code
        delay(1000);
    }

Complete Example
----------------

Example with custom error handling
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

    #include "OpenIndus.h"
    #include "Arduino.h"

    // Custom error handling
    void System::handleError(int errorCode)
    {
        printf("System error: %d\n", errorCode);
        printf("Restarting in 5 seconds...\n");
        delay(5000);
        esp_restart(); // Automatic restart
    }

    void setup(void)
    {
        printf("Application initialization\n");
        // Your initialization code
    }

    void loop(void)
    {
        // Your main code
        delay(1000);
    }

System API
----------

.. doxygenclass:: System
   :members:

.. note::
    The file ``Main.cpp`` contains the entry point ``app_main()`` which automatically manages system initialization. 
    You only need to implement ``setup()`` and ``loop()`` in your code.
