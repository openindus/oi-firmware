.. OIDiscrete

OIDiscrete - Module to control logical inputs and outputs
=========================================================

Description
-----------

This API is used to control digital Inputs and Outputs from 9V to 30V of OIDiscrete module.

| ETOR is the name for Inputs.
| STOR is the name for Outputs.
| ANA is the name for Analog Inputs.
| STOR_SENSOR is the name for output current sensors.

.. note:: Some ETORs and STORs are paired on the same pins. More precisions in :ref:`OIDiscrete hardware section<OIdiscrete_s>`. If an ETOR and a STOR are paired you can still use them at the same time.


Example
-------

.. code-block:: cpp

    #include "OpenIndus.h"
    #include "Arduino.h"

    void setup()
    {
        Discrete.ledOn(LED_CYAN);
    }

    void loop()
    {
        /* Blink Output */
        Discrete.digitalWrite(STOR1, HIGH_LEVEL);
        delay(500);
        Discrete.digitalWrite(STOR1, LOW_LEVEL);
        delay(500);
    }

Functions
---------

.. doxygenfunction:: OIDiscrete::digitalWrite
.. doxygenfunction:: OIDiscrete::analogWrite
.. doxygenfunction:: OIDiscrete::digitalRead
.. doxygenfunction:: OIDiscrete::analogRead
.. doxygenfunction:: OIDiscrete::attachInterrupt
.. doxygenfunction:: OIDiscrete::detachInterrupt