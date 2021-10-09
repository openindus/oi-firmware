.. _electrical_s:

Electrical specifications
==========================

Description
-----------

This section present common electrical specifications for all modules.


Digital Inputs
--------------

This feature is present on all OpenIndus modules.

.. list-table:: Digital inputs specifications
   :widths: 33 33 33
   :header-rows: 1

   * - Requirements
     - Type
     - Value
   * - Voltage input low
     - Max
     - 2V
   * - Voltage input high
     - Min
     - 4V
   * - Maximum voltage
     - 
     - 30V
   * - Impedance
     - Min
     - 4.8kOhms
   * - Voltage drop
     - Max
     - 0.33V
   * - Sampling
     - Max
     - 10kHz

Analog Inputs 0-30V
-------------------

This feature is present on OICore/OICoreLite and OIDiscrete modules.

.. list-table:: Analog inputs 0-30V specifications
   :widths: 33 33 33
   :header-rows: 1

   * - Requirements
     - Type
     - Value
   * - Resolution
     - 
     - 12bits
   * - Full scale
     - 
     - 0 to 30V
   * - Maximum voltage
     - 
     - 30V
   * - Impedance
     - Min
     - 4.8kOhms
   * - Voltage drop
     - Max
     - 0.33V
   * - Sampling
     - Max
     - 500Hz

Industrial Analog Inputs
------------------------

This feature is present on OIMixed module.

.. list-table:: Industrial Analog inputs specifications
   :widths: 33 33 33
   :header-rows: 1

   * - Requirements
     - Type
     - Value
   * - Resolution
     - 
     - 16bits
   * - Sampling
     - Max
     - 500Hz
   * - **Voltage mode**
     - 
     - 
   * - Full scale
     - 
     - +/- 0.64V to +/- 10.24V
   * - Impedance
     - Min
     - 0.85MOhms
   * - Maximum voltage
     - 
     - +/- 20V
   * - **Current mode**
     - 
     - 
   * - Full scale
     - 
     - 0 to 24mA
   * - Impedance
     - Nominal
     - 106Ohms
   * - Maximum current
     - 
     - 24.39mA
   * - Maximum voltage
     - 
     - 2.56V

Digital outputs
---------------

This feature is present on all OpenIndus modules.

.. list-table:: Digital outputs specifications
   :widths: 33 33 33
   :header-rows: 1

   * - Requirements
     - Type
     - Value
   * - Voltage input low
     - Max
     - 2V
   * - Voltage input high at 0.5A
     - Min
     - Vcc - 0.4V
   * - Voltage input high at 5A
     - Min
     - Vcc - 0.6V
   * - Maximum voltage
     - 
     - 30V
   * - Maximum current (for each output)
     - 
     - 4A
   * - Maximum current (total)
     - 
     - 8A
   * - Peak current
     - 
     - 13.5A
   * - ON/OFF frequency
     - Max
     - 10kHz

Industrial Analog outputs
--------------------------

This feature is on OIMixed module.

.. list-table:: Industrial Analog outputs specifications
   :widths: 33 33 33
   :header-rows: 1

   * - Requirements
     - Type
     - Value
   * - Resolution
     - 
     - 12bits
   * - Full scale (Voltage mode)
     - 
     - 0 to 10V
   * - Full scale (Current mode)
     - 
     - 0 to 24mA
   * - Short-circuit current
     - Min
     - 30mA
