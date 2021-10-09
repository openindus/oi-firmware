.. OICAN

OICAN
=====


Description
-----------

CAN functions. This feature is only available on OICore module. 
Please check out :ref:`OICore pinout<OIcore_pinout_s>` to see CAN interfaces on OICore module.

CAN Standard Example
--------------------

.. literalinclude:: ../../../../examples/can_standard.cpp
    :language: cpp

CAN Extended Example
--------------------

.. literalinclude:: ../../../../examples/can_extended.cpp
    :language: cpp

Functions
---------

.. doxygenfunction:: OICAN::begin
.. doxygenfunction:: OICAN::end
.. doxygenfunction:: OICAN::write
.. doxygenfunction:: OICAN::available
.. doxygenfunction:: OICAN::read
.. doxygenfunction:: OICAN::setStandardFilter
.. doxygenfunction:: OICAN::setExtendedFilter

.. doxygenstruct:: OICanMessage_t
   :members:
