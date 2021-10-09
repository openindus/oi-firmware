.. OISystem

OISystem
========

Description
-----------

OISystem is the class that manage modules. You add your modules, you can initialise them or stop them with OISystem functions. 
Module is the current board you are developing on. You always have to set a module
Set a submodule if you want to communicate with another module and execute commands remotely.

By default, the system starts automatically

Example
-------

.. code-block:: cpp

    #include "OpenIndus.h"

    OIDiscrete Discrete(1);
    OIStepper Stepper(2);

    void setup()
    {
        Core.ledOn(LED_CYAN);
        Discrete.ledOn(LED_CYAN);
        Stepper.ledOn(LED_CYAN);
    }

    void loop()
    {

    }

Functions
---------

.. doxygenfunction:: OISystem::start
.. doxygenfunction:: OISystem::stop
.. doxygenfunction:: OISystem::status
