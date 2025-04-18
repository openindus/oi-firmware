.. _can_s:

CAN
===

Description
-----------

This features is present on :ref:`OI-Core<OI-Core>`/:ref:`OI-CoreLite<OI-CoreLite>` (x1).

This is a standard CAN 2.0B bus.

A termination resistor of 120Ω is built into the module between CAN_H and CAN_L.

The CAN controller adheres to the standard ISO 11989-1, while the CAN transceiver complies with standards ISO-11898-2 and ISO-11898-5.
The bus speed can be set up to 1Mbit/sec (user-selectable in code).
The CAN functionality is managed internally via SPI, but there's no need to worry as a ready-to-use API is already implemented to assist you.

Code examples
-------------

The example code above demonstrates how to use CAN.

.. literalinclude:: ../../examples/CANStandard.cpp
    :language: cpp

The example code above demonstrates how to use CAN extended.

.. literalinclude:: ../../examples/CANExtended.cpp
    :language: cpp

Software API
------------

.. doxygenclass:: CAN
   :members: