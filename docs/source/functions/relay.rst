.. _relay_s:

Relay
=====

Mechanical Relay
****************

Description
-----------

Mechanical relays are present on :ref:`OI-RelayHP` module.

Characteristics
---------------

.. list-table:: Relay specifications
   :widths: 33 33 33
   :header-rows: 1
   :align: center

   * - Requirements
     - Value
     - Remark
   * - Maximum voltage (AC)
     - 400V (RMS)
     - 
   * - Maximum voltage (DC)
     - 30V
     - 
   * - Maximum current
     - 6A
     - 
   * - Internal fuse protection
     - 6.3A
     - on 'COM' input
   * - Isolation
     - 1500VDC
     - 


Static Relay
************

Description
-----------

Static relays are present on :ref:`OI-RelayLP` module. You can use them to switch alternative voltage.

Characteristics
---------------

.. list-table:: Static Relay specifications
   :widths: 33 33 33
   :header-rows: 1
   :align: center

   * - Requirements
     - Value
     - Remark
   * - Maximum voltage (AC)
     - 400V (RMS)
     - 
   * - Maximum current
     - 1A
     - 
   * - Internal fuse protection
     - 1A
     -
   * - Isolation
     - 5000VDC
     - 

Code examples
*************

The example code above demonstrates how to set the relay on an :ref:`OI-RelayHP` module. 

.. note::
    To use this example with an :ref:`OI-RelayLP` module, simply replace ``OIRelayHP`` with ``OIRelayLP`` in the class declaration.

.. literalinclude:: ../../examples/Relay.cpp
    :language: cpp

Software API
************

Both relay types can be controlled using the :cpp:class:`Relays` class.

.. doxygenclass:: Relays
   :members: