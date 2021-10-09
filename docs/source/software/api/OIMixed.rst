.. OIMixed

OIMixed
=======

Description
-----------

OIMixed module enables to drive digital inputs and outputs from a 9V to 30V alimentation.
Analogs inputs supports voltage reading from 0 to 10V and current reading from 0 to 20mA.
Analog outputs enable to generate signals for industrial actuators using 0-10V or 4-20mA.

| ETOR is the name for inputs.
| STOR is the name for outputs.
| EANA is the name for analog inputs.
| SANA is the name for analog outputs.

.. note:: Some ETORs and STORs are paired on the same pins. More precisions in :ref:`OIMixed hardware section<OImixed_s>`. If an ETOR and a STOR are paired you can still use them at the same time.


Example
-------

Coming soon !
No example yet.


Functions
---------

.. doxygenfunction:: OIMixed::digitalWrite
.. doxygenfunction:: OIMixed::analogWrite
.. doxygenfunction:: OIMixed::digitalRead
.. doxygenfunction:: OIMixed::getStorCurrent
.. doxygenfunction:: OIMixed::analogRead(Eana_t ana)
.. doxygenfunction:: OIMixed::analogRead(Eana_t ana, Units_t units)
.. doxygenfunction:: OIMixed::analogReadMode
.. doxygenfunction:: OIMixed::analogReadResolution
.. doxygenfunction:: OIMixed::getAnalogReference
.. doxygenfunction:: OIMixed::setAnalogMode
.. doxygenfunction:: OIMixed::getAnalogMode
.. doxygenfunction:: OIMixed::voltageWrite(Sana_t sana, uint32_t value)
.. doxygenfunction:: OIMixed::voltageWrite(Sana_t sana, float value, Units_t units)
.. doxygenfunction:: OIMixed::setCurrentMode
.. doxygenfunction:: OIMixed::getCurrentMode
.. doxygenfunction:: OIMixed::currentWrite(Sana_t sana, uint32_t value)
.. doxygenfunction:: OIMixed::currentWrite(Sana_t sana, float value, Units_t units)
.. doxygenfunction:: OIMixed::attachInterrupt
.. doxygenfunction:: OIMixed::detachInterrupt

