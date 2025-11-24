.. _hardware_setup-index:

Hardware Setup
==============

This guide will help you set up your OpenIndus module hardware, including power supply wiring.

.. warning:: 
    Always disconnect power before wiring or modifying connections. Work in a safe environment following electrical safety standards.


Power Supply Connection
-----------------------

All OpenIndus modules can be powered through the terminal block with a voltage between 9VDC and 30VDC (typically 24V DC).

Power Supply Requirements
*************************

.. list-table:: 
   :widths: 30 70
   :header-rows: 1

   * - Specification
     - Value
   * - Voltage
     - 9V to 30V DC
   * - Current per pin
     - Maximum 8A per terminal block pin
   * - Protection
     - Short-circuit and overload protection recommended

.. warning::
    Maximum current on each pin of the terminal block is 8A. You must ensure that your system wiring is well dimensioned in accordance with the power you are using.

Wiring the Power Supply
************************

Each OpenIndus module has dedicated terminals for power supply connection on the terminal block:

1. **Locate the power terminals** on your module: pin 16 (VIN) for positive and pin 15 (GND) for ground
2. **Strip the wire** insulation over a length of 7 to 8 mm
3. **Connect the positive wire** (VIN) to **pin 16** of the terminal block
4. **Connect the ground wire** (GND) to **pin 15** of the terminal block

.. image:: ../_static/mapping_power.png
    :width: 600
    :alt: Power supply connection
    :align: center

|

.. warning:: 
    Do not use wire ferrules with these terminal blocks. 
    Ferrules with insulating collar do not fit properly in the connector, and ferrules without insulating collar get stuck inside the terminal mechanism. 
    The terminal blocks are designed for bare wire connections only.

USB Power
*********

You can power a module through its USB port for uploading code and testing, but the outputs will not function properly. USB power is only suitable for programming purposes.

Powering Multiple Modules
**************************

When using multiple modules on a rail:

* **Shared power**: A module can power other modules through the rail if they are plugged into the same physical rail. You do not need to power all modules individually.
* **Different voltages**: You can power modules with different voltages. For example, if you need to power one :ref:`OI-Discrete` with 12V and another one with 24V, the voltage on the rail will be the higher one (24V).
* **Voltage control**: If you want a lower voltage on the rail (e.g., 12V), you can switch off the power to the higher voltage module (24V) using software commands.

.. note:: 
    This does not apply to :ref:`OI-RelayHP`, which utilizes a different terminal block for managing 230VAC. 
    For :ref:`OI-RelayHP`, you need to use other modules to power it through the rail.

Secondary Power Input
*********************

Some modules, such as :ref:`OI-Discrete` and :ref:`OI-Stepper`, feature a secondary input for voltage (pins 13 and 14 on the terminal block). 

This secondary input is convenient for:

* **Chaining modules together**: Distribute power to multiple modules
* **Increasing input power**: Useful for modules with higher power requirements like :ref:`OI-Stepper`

.. warning::
    You must not exceed 8A per pin when using the secondary power input.
