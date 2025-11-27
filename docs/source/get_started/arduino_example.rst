.. _arduino_example-index:

Arduino & External libraries
============================

Use Arduino code
----------------

If you are a beginner with Arduino programming, you will find a lot of example codes on the internet. You can use them with adaptation in the code source.

You will also find some example codes using Arduino libraries under the folder "component/arduino/libraries/.../examples".


Example: Use Ticker to control timing
*************************************

Here, we will show you how to adapt the Ticker example to run it on an OICore.
Ticker is a library that allows you to use timer in a simple manner. The library exposes an API that wraps low level timer settings with simplicity.

The example code for Arduino is the following:

.. code-block:: cpp

    #include <Arduino.h>
    #include <Ticker.h>

    // attach a LED to GPIO 21
    #define LED_PIN 21

    Ticker blinker;
    Ticker toggler;
    Ticker changer;
    float blinkerPace = 0.1;       //seconds
    const float togglePeriod = 5;  //seconds

    void change() {
        blinkerPace = 0.5;
    }

    void blink() {
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }

    void toggle() {
        static bool isBlinking = false;
        if (isBlinking) {
            blinker.detach();
            isBlinking = false;
        } else {
            blinker.attach(blinkerPace, blink);
            isBlinking = true;
        }
        digitalWrite(LED_PIN, LOW);  //make sure LED is on after toggling (pin LOW = LED ON)
    }

    void setup() {
        pinMode(LED_PIN, OUTPUT);
        toggler.attach(togglePeriod, toggle);
        changer.once(30, change);
    }

    void loop() {}


What do you need to modify to use it on an OICore?

* First, include "OpenIndus.h" and declare an OICore
* Change the LED pin with DOUT_1 (or any other output pin)
* Change all calls to `digitalRead` and `digitalWrite` with `core.digitalRead` and `core.digitalWrite`
* Remove the call to `pinMode` because it is not needed
* And that's it! You can now run your build and flash your code!

Here is the full modified code:

.. code-block:: cpp

    #include "OpenIndus.h"
    #include <Arduino.h>
    #include <Ticker.h>

    OICore core;

    // attach a LED to GPIO 21
    #define LED_PIN DOUT_1

    Ticker blinker;
    Ticker toggler;
    Ticker changer;
    float blinkerPace = 0.1;       //seconds
    const float togglePeriod = 5;  //seconds

    void change() {
        blinkerPace = 0.5;
    }

    void blink() {
        core.digitalWrite(LED_PIN, !core.digitalRead(LED_PIN));
    }

    void toggle() {
        static bool isBlinking = false;
        if (isBlinking) {
            blinker.detach();
            isBlinking = false;
        } else {
            blinker.attach(blinkerPace, blink);
            isBlinking = true;
        }
        core.digitalWrite(LED_PIN, LOW);  //make sure LED is on after toggling (pin LOW = LED ON)
    }

    void setup() {
        toggler.attach(togglePeriod, toggle);
        changer.once(30, change);
    }

    void loop() {}


.. warning:: Be careful not to use directly the Arduino function `digitalWrite` and `digitalRead` because the behavior is undefined and your code will not work as expected.


Add external library to the code
--------------------------------

Arduino libraries
*****************

You can use many libraries written for Arduino or for ESP32. Depending on the library, we will have to do small modifications or a full reintegration.

You can find a lot of Arduino libraries on the `official Arduino libraries website <https://docs.arduino.cc/libraries/>`_.
Most of the time, you can adapt them by adding a `CMakeLists.txt file <https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/api-guides/build-system.html#component-cmakelists-files>`_.


Espressif libraries
*******************

You can also find libraries written with esp-idf framework on the `ESP-IDF components website <https://components.espressif.com/>`_.
These libraries can be added to the project using `ESP-IDF Component manager <https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/api-guides/tools/idf-component-manager.html>`_.
