.. _can_s:

CAN
===

Description
-----------

This features is present on :ref:`OI-Core<OI-Core>`/:ref:`OI-CoreLite<OI-CoreLite>` (x1).

This is a standard CAN 2.0B bus.

A termination resistor of 120Ω is built into the module between CAN_H and CAN_L.

The CAN controller adheres to the standard ISO 11989-1, 
while the CAN transceiver complies with standards ISO-11898-2 and ISO-11898-5.
The bus speed can be set up to 1Mbit/sec (user-selectable in code).
The CAN functionality is managed internally via SPI, 
but there's no need to worry as a ready-to-use API is already implemented to assist you.

Code examples
-------------

The example code below demonstrates how to use CAN (standard frame).

.. literalinclude:: ../../examples/CAN.cpp
    :language: cpp

.. note::
   To use CAN Extended mode, simply modify the message identifier and the relevant flags 
   in the example above to use an extended identifier (29 bits) and enable 
   the extended frame flag in the CAN API. 
   Refer to the API documentation for the exact parameters to adjust.

Software API
------------

.. doxygenclass:: CAN
   :members:

.. doxygenstruct:: CAN_Message_t
   :members: