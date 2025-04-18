.. _mechanicalrelay_s:

Mechanical Relay
================

Description
-----------

Mechanical relay are present on :ref:`OI-RelayHP` module.

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

Code examples
-------------

The example code above demonstrates how to set the relay on an :ref:`OI-RelayHP` module. 
Note that normally closed (NC) output will be disconnected from 'COM' when you set the relay value to 'HIGH'

.. literalinclude:: ../../examples/RelayHP.cpp
    :language: cpp

Software API
------------