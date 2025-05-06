.. _rtc_s:

Real Time Clock
===============

Description
-----------

This is a feature present on :ref:`OI-Core<OI-Core>`/:ref:`OI-CoreLite<OI-CoreLite>` (x1).

The Real Time Clock (RTC) is power from an internal battery. The lifetime of the internal 3V battery is over 60 years.
The time derivation is less than ±20 ppm @ 25°C.  

Code examples
-------------

.. literalinclude:: ../../examples/RTCAlarm.cpp
    :language: cpp


Software API
------------

.. doxygenclass:: RTClock
   :members: