.. OICore

OICore
======

Description
-----------

OICore is the master module. 
It controls all others module by sending them command. 
This is the module you have to program and should not care about adding software to other modules.

I/O example
------------

.. code-block:: cpp

    #include "OpenIndus.h"
    #include "Arduino.h"

    void setup()
    {
        Core.ledOn(LED_CYAN);
    }

    void loop()
    {
        /* Blink Output */
        Core.digitalWrite(STOR1, HIGH_LEVEL);
        delay(500);
        Core.digitalWrite(STOR1, LOW_LEVEL);
        delay(500);
    }

RTC example
-----------

.. literalinclude:: ../../../../examples/core_rtc_alarm.cpp
    :language: cpp

Basic functions
---------------

.. doxygenfunction:: OICore::digitalWrite
.. doxygenfunction:: OICore::digitalRead
.. doxygenfunction:: OICore::analogRead
.. doxygenfunction:: OICore::attachInterrupt
.. doxygenfunction:: OICore::detachInterrupt

RTC functions
-------------

.. doxygenfunction:: OICore::time
.. doxygenfunction:: OICore::now
.. doxygenfunction:: OICore::enableRTCAlarm
.. doxygenfunction:: OICore::disableRTCAlarm
.. doxygenfunction:: OICore::setRTCAlarm(time_t time)
.. doxygenfunction:: OICore::setRTCAlarm(DateTime datetime)
.. doxygenfunction:: OICore::attachRTCAlarm
.. doxygenfunction:: OICore::detachRTCAlarm