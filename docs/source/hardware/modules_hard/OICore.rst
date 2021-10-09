.. _OIcore_s:

OICore
======

Description
-----------

The OICore is the master module. It controls all others module by sending them command. 
This is the module you have to program and should not care about adding software to other modules.

The Core module is declined in two models:

* OICoreLite: Master module with many interfaces to communicate with most used medium in industrial sector such as Wifi, Bluetooth, RS485/RS232, CAN and others. 
* OICore : A more complete version with USB host and Ethernet features.


The following table show you the modules specifications. 

.. list-table:: OICore module hardware characteristics
   :widths: 65 35
   :header-rows: 1

   * - Specifications
     - Value
   * - CPU
     - ESP32-S1
   * - **I/O**
     - 
   * - Digital inputs
     - 4
   * - Analog inputs 0-30V
     - 2
   * - Digital outputs with current sensors
     - 4
   * - **Field bus**
     - 
   * - CAN (50kbps - 1 Mbps)
     - 1 
   * - RS232/RS485
     - 1 (Half-Duplex)
   * - **Interfaces**
     - 
   * - USB B 2.0
     - 1
   * - Bluetooth 4.2
     - 1
   * - Ethernet 10 Mbps
     - 1
   * - Wifi 802.11 b/g/n 2.4 GHz
     - 1
   * - µSD Card 32Gb
     - 1
   * - USB Host 2.0 
     - 1
   * - I2C
     - 1
   * - 5V Output 500mA
     - 1
   * - **Supply**
     - 
   * - Supply Voltage
     - 9-30V
   * - Consumption (all outputs disabled)
     - < 1W
   * - **Mechanical**
     - 
   * - Temperature range
     - -20 °C... +70°C
   * - Package
     - BOI23 (75.97x106.81x27mm)

.. list-table:: OICoreLite module hardware characteristics
   :widths: 65 35
   :header-rows: 1

   * - Specifications
     - Value
   * - CPU
     - ESP32-S1
   * - **I/O**
     - 
   * - Digital inputs
     - 4
   * - Analog inputs 0-30V
     - 2
   * - Digital outputs
     - 4
   * - Digital outputs max current
     - 5A
   * - **Field bus**
     - 
   * - CAN (50kbps - 1 Mbps)
     - 1
   * - RS232/RS485 
     - 1 (Half-Duplex)
   * - **Interfaces**
     - 
   * - USB B 2.0
     - 1 
   * - Bluetooth 4.2
     - 1
   * - Wifi 802.11 b/g/n 2.4 GHz
     - 1
   * - µSD Card 32Gb
     - 1
   * - I2C
     - 1
   * - 5V Output 500mA
     - 1
   * - **Supply**
     - 
   * - Supply Voltage
     - 9-30V
   * - Consumption (all outputs disabled)
     - < 1W
   * - **Mechanical**
     - 
   * - Temperature range
     - -20 °C... +70°C
   * - Package
     - BOI13 (75.97x53.81x27mm)

.. _OIcore_pinout_s:

Pinout
------

The pinout is common for Core and CoreLite and is as follow : 

.. image:: ../../_static/mapping_core.png
    :width: 700
    :alt: OICore mapping
    :align: center

| Digital inputs and outputs are regroup in pairs. You can use them both at the same time.
| RS232 and RS485 are on the same pins. You can not use them all at once. 
| **RS_TX** and **RS_RX** stand for RS232 configuration. 
| **A-** and **B+** stand for RS485. 
