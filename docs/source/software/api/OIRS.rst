.. OIRS

OIRS
======

Description
-----------

RS232/RS485 functions. This feature is only available on OICore module. 
Please check out :ref:`OICore pinout<OICore_pinout_s>` to see RS interfaces on OICore module.

Examples
--------

RS485 Receiver
**************

.. literalinclude:: ../../../../examples/RS485-Receiver.cpp
    :language: cpp

RS485 Sender
************

.. literalinclude:: ../../../../examples/RS485-Sender.cpp
    :language: cpp

Functions
---------

.. doxygenfunction:: OIRS::begin
.. doxygenfunction:: OIRS::end
.. doxygenfunction:: OIRS::available
.. doxygenfunction:: OIRS::availableForWrite
.. doxygenfunction:: OIRS::read(void)
.. doxygenfunction:: OIRS::read(uint8_t *buffer, size_t size)
.. doxygenfunction:: OIRS::write(uint8_t byte)
.. doxygenfunction:: OIRS::write(uint8_t* data, size_t len)