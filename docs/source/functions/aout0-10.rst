.. _aout0-10_s:

Analog Output 0-10V or 4-20mA
=============================

Description
-----------

This feature is present on :ref:`OI-Mixed` module (x2).

With these outputs, you can generate any voltage or current within the specified range. 
These outputs are especially suitable for applications with actuator controlled by voltage or current.

It's feasible to integrate both types of generator, accommodating both current and voltage output, on the same board.

Characteristics
---------------

Voltage mode
************

.. list-table:: Voltage output specifications
   :widths: 33 33 33
   :header-rows: 1
   :align: center

   * - Requirements
     - Value
     - Remark
   * - Resolution
     - 14 bits
     - 
   * - Minimum output voltage
     - -10.25V
     - 
   * - Maximum output voltage
     - 10.25V
     - 
   * - Precision
     - ± 60mV
     - worst case
   * - Step
     - 1.28mV
     -
   * - Minimum load
     - 1kΩ
     -  
   * - Output impedance
     - 7mΩ
     - 

Output is protected against short-circuit.


Current mode
************

.. list-table:: Current output specifications
   :widths: 33 33 33
   :header-rows: 1
   :align: center

   * - Requirements
     - Value
     - Remark
   * - Resolution
     - 14 bits
     - 
   * - Minimum output current
     - 0mA
     - 
   * - Maximum output current
     - 24mA
     - 
   * - Precision
     - ±20µA
     - worst case
   * - Step
     - 1.46µA
     -
   * - Maximum load
     - 500Ω
     - 
   * - Output impedance
     - 100MΩ
     - 

Output is protected against open-circuit.


Code examples
-------------

The example code above demonstrates how to set a current on output AOUT_1 and AOUT_2.

.. literalinclude:: ../../examples/AOUTMixedCurrent.cpp
    :language: cpp

The example code above demonstrates how to set a voltage on output AOUT_1 and AOUT_2.

.. literalinclude:: ../../examples/AOUTMixedVoltage.cpp
    :language: cpp

Software API
------------

.. doxygenclass:: AnalogOutputs
   :members: