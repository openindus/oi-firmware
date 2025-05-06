.. _i2cttl_s:

I2C (TTL 5V)
================

Description
-----------

This features is present on :ref:`OI-Core<OI-Core>` and :ref:`OI-CoreLite<OI-CoreLite>`.
It offer the possibility to communicate with any external device through I2C. The voltage level is 5V.

Characteristics
---------------

.. list-table:: Digital inputs specifications
   :widths: 33 33 33
   :header-rows: 1
   :align: center

   * - Requirements
     - Value
     - Remark
   * - I2C logic voltage level
     - 5V
     - 
   * - Maximum speed
     - 1MHz
     - 


Code examples
-------------

The example code above demonstrates how to get the temperature from an I2C LM75 sensor.

.. literalinclude:: ../../examples/I2CTemperatureSensor.cpp
    :language: cpp


Software API
------------

As you can see, you can use the I2C as you would do on any Arduino based board. For full documentation, check the `official documentation <https://docs.espressif.com/projects/arduino-esp32/en/latest/api/i2c.html>`_

.. note::
    By default, I2C will be initialized with correct gpio pins. If you need to do it manually, SDA gpio pin is 36 and SCL is 37 but you can use macro **'SDA'** and **'SCL'** with are already defined in pins_arduino.h.