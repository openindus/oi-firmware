.. _get_started-index:

Launch your project
===================

This guide is designed to simplify your understanding of OpenIndus modules and streamline your development process. 
It provides step-by-step instructions for setting up the hardware, the environment, and initiating coding. 

You'll discover detailed descriptions of each module in the "modules" section, along with specifications for every function in the "functions" section. 
Furthermore, the "projects" section contains comprehensive code examples that demonstrate how our system operates.

.. note:: This documentation is based on the latest stable version of the OpenIndus software. It will be updated to reflect future improvements.

OpenIndus Modules Overview
---------------------------

Let's start by understanding how OpenIndus' modules work.

Each module embeds specific hardware functions. The idea is to gather all the modules you need for your system.

For systems with multiple modules, a rail is required to interconnect them. In such setups, one module operates as the "master," while the others are designated as "slaves." 
This setup means that programming is only required for the master module; other modules do not need individual programming. 

.. note:: Currently, the "master" module must be either an :ref:`OI-Core<OI-Core>` or :ref:`OI-CoreLite<OI-CoreLite>`, but we are actively working to enable the use of any module as a "master."

For single-module setups, no rail is needed, and programming can be directly done on the module itself, which is then referred to as a "standalone" module.

Understanding these distinctions is crucial when initiating a new project, as you'll have the option to choose between "master," "standalone," and "slave" configurations.

All our modules utilize Espressif chips and can be programmed using Arduino. If you're familiar with Arduino programming, you'll find it seamless. 
If not, don't worry; programming with Arduino is straightforward, and this guide will walk you through it step by step.

The easiest way to set up everything is by using Visual Studio Code with the OpenIndus and PlatformIO extensions. 
You'll find a comprehensive installation guide in the `Environment Installation`_ section.


Environment Installation
------------------------

Visual Studio Code
******************

To begin with, install Visual Studio Code. It will be the IDE to develop with our modules.
VSCode is supported on Windows, Linux, and macOS. Please check the `installation link <https://code.visualstudio.com>`_.


.. _get_started_oivscodeextension:

Install OpenIndus VSCode Extension
**********************************

Once you have installed VSCode, you'll be able to add the OpenIndus extension.
This extension will add some commands to VSCode, such as creating a new project or retrieving information about connected modules.

* Open the **Extension** panel
* Search for **OpenIndus**
* Click on **Install** button

.. image:: ../_static/gif/install_oi_extension.gif
    :width: 800
    :alt: OpenIndus extension in VSCode
    :align: center

|

The first time it can take several minutes because VSCode will also install ESP-IDF.

Configure ESP-IDF Extension
***************************

ESP-IDF extension is the official VSCode tool to compile, flash and monitor your code. At the first installation, you need to configure it.
The official documentation can be found here: `ESP-IDF official documentation <https://docs.espressif.com/projects/vscode-esp-idf-extension/en/latest/>`_.

In this section, you will find a guide to install with default parameters. Depending on your internet connection, it can take some minutes to download and install all resources (compiler, libraries, examples, ...). 
But don't worry, once configured you won't need to do it every time.

The page to configure ESP-IDF extension should be automatically opened at launch (if not, click on the ESP-IDF logo on the left side panel and in the "Advanced" section, click on "Configure ESP-IDF Extension").

On the configuration page, do the following:

* Select "Express"
* Select the latest ESP-IDF version (currently v5.5.1)
* You can customize the installation folder
* Launch the installation and wait for it to complete

.. image:: ../_static/gif/configure_idf.gif
    :width: 800
    :alt: ESP-IDF recommended configuration
    :align: center

|

.. note:: ESP-IDF Extension downloads and configures all packages necessary (toolchain, uploader, monitor tool, ...).

Begin with your first project
-----------------------------

Start a new project
*******************

At this point, you have everything you need to begin your first program.

* Click on the OpenIndus logo on the left bar
* Click on **create a new project**.
* Choose the device you will program. We recommend starting by programming on an :ref:`OI-Core<OI-Core>` or an :ref:`OI-CoreLite<OI-CoreLite>`.
* Choose a root folder for your application.
* Choose a name for your application.
* Choose the 'type' of project. If you have a system with several modules, choose 'master' and if you have a system with only one module, choose 'standalone'.
* Choose if you want to use the Arduino Libraries (recommended) or only the ESP-IDF framework (for advanced users).
* Wait while the project is created. A new folder will open up with your new project!

.. image:: ../_static/gif/create_project_idf.gif
    :width: 800
    :alt: Create a project
    :align: center

|

Build your project
*******************

The created project prints 'Hello World!' to the serial port. You can find the main code in src/main.cpp. 
If you are familiar with Arduino, you will recognize the 'setup' and 'loop' functions.

Before uploading this code to the console, you will have to build this code.
Click on the build button on the bottom bar to build your project. 

.. image:: ../_static/gif/build_project_idf.gif
    :width: 800
    :alt: Build a project
    :align: center

|

.. note:: The first time you build a project, all source files from Espressif library are downloaded and built, don't worry it is normal behavior. Next builds will be faster.


Upload your project
*******************

Plug the device you want to program to your computer with a USB cable.
Windows should detect the chip and automatically download the driver. If you cannot see your device in the device manager, please download manually the driver from this link: `Silicon Labs driver <https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers>`_.

You have to select the correct COM port by clicking the "Select port to USE button" in the bottom bar.
Then you can upload the program to your device with the "Flash Device" button in the bottom bar. You must choose "UART" if asked for flashing method.

.. image:: ../_static/gif/upload_project_idf.gif
    :width: 800
    :alt: Upload a project
    :align: center

|

.. warning:: If you have only one device plugged into your computer, PlatformIO should detect it automatically. If you have several devices, select the correct one by changing from 'Auto' to the wanted COM port.

.. note:: You do not need to click on the build button before upload, PlatformIO will detect if the project has to be built again before uploading.

Monitor your project
********************

After a successful upload, click on the "Monitor device" button in the bottom bar to see the log output of your device.

.. image:: ../_static/gif/monitor_project_idf.gif
    :width: 800
    :alt: Monitor a project
    :align: center

|

.. _start_coding-index:

Start coding
------------

Now you are ready to create your own projects!

Read Digital Input
******************

You can change the default code in the main.cpp file. 
Below is an example of reading the value of Digital Input 1 and printing the result to the serial monitor every second.

.. code-block:: cpp

    #include "OpenIndus.h"
    #include "Arduino.h"

    // First, init the master device
    OICore core;

    void setup(void)
    {
        // put your setup code here, to run once:
        Serial.begin(115200);
        Serial.println("Hello World !");
    }

    void loop(void)
    {
        // put your main code here, to run repeatedly:
        Serial.print("Value of Digital Input 1 is ");
        Serial.println(core.digitalRead(DIN_1));
        delay(1000);
    }

Add one slave device
********************

We are now going to add an :ref:`OI-Discrete` as a slave device in your code. For that you need to connect on the same bus a Core/CoreLite and a Discrete.
Modify the main.cpp file as follows:

.. code-block:: cpp

    #include "OpenIndus.h"
    #include "Arduino.h"

    // First, init the master device
    OICore core;

    // Then add slave device here:
    OIDiscrete discrete;

    void setup(void)
    {
        // put your setup code here, to run once:
        Serial.begin(115200);
        Serial.println("Hello World !");
    }

    void loop(void)
    {
        // put your main code here, to run repeatedly:
        Serial.print("Value of Digital Input 1 of OI-Discrete is ");
        Serial.println(discrete.digitalRead(DIN_1));
        delay(1000);
    }

Add several slave devices (with auto ID)
*****************************************

If you want to add several slave devices, you have to add them in the code in the same order as they are mounted on the rail.

For example, if you have the following hardware configuration:

.. image:: ../_static/rail_example_auto_id.png
    :width: 800
    :alt: Example rail configuration
    :align: center

|

You need to instantiate boards in the following order:

.. code-block:: cpp

    OIDiscrete discrete1();  
    OIMixed mixed1();
    OIDiscrete discrete2();  

Here is a full example code for the configuration above:

.. code-block:: cpp

    #include "OpenIndus.h"
    #include "Arduino.h"

    // First, init the master device
    OICore core;

    // Then add slave device here:
    OIDiscrete discrete1();  
    OIMixed mixed1();
    OIDiscrete discrete2();  

    void setup(void)
    {
        // put your setup code here, to run once:
        Serial.begin(115200);
        Serial.println("Hello World !");
    }

    void loop(void)
    {
        // put your main code here, to run repeatedly:
        Serial.print("Value of Digital Input 1 of OI-Discrete 1 is ");
        Serial.println(discrete1.digitalRead(DIN_1));
        Serial.print("Value of Digital Input 1 of OI-Discrete 2 is ");
        Serial.println(discrete2.digitalRead(DIN_1));
        Serial.print("Value of Digital Input 1 of OI-Mixed 1 is ");
        Serial.println(mixed1.digitalRead(DIN_1));
        delay(1000);
    }

Add several slave devices (with serial number)
***********************************************

An alternative way of adding slave devices is to instantiate them with serial number.

.. image:: ../_static/rail_example_sn.png
    :width: 800
    :alt: Example rail configuration
    :align: center

|

With the configuration above, you can instantiate modules as follows without caring about the order:

.. code-block:: cpp

    OIDiscrete discrete1(0000128);
    OIDiscrete discrete2(0000273);
    OIMixed mixed1(0000235);

.. note:: 
    This way of instantiation is not scalable. If you change one module, the part number will be different and you will have to update your code. 
    The advantage is that you can place your module wherever you want on the rail.

Use Arduino code
----------------


If you are a beginner with Arduino programming, you will find a lot of example codes on the internet. You can use them with adaptation in the code source.
You will also find some example codes using Arduino libraries under the folder "component/arduino/libraries/.../examples".


Example: Use Ticker to control timing
*****************************

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
---------------------------------

You can use many libraries written for Arduino or for ESP32. Depending on the library, we will have to do small modifications or a full reintegration.

You can find a lot of Arduino libraries on the official website: https://docs.arduino.cc/libraries/
Most of the time, you can adapt them by adding a CMakeLists.txt file: https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/api-guides/build-system.html#component-cmakelists-files

You can also find libraries written with esp-idf framework here: https://components.espressif.com/
These libraries can be added to the project using ESP-IDF Component manager: https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/api-guides/tools/idf-component-manager.html
