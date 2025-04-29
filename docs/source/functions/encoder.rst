.. _encoder_s:

Encoder
=======

Description
-----------

Encoders are available on the following modules:

* :ref:`OI-Stepper` (x2)

**Main characteristics**

* Accurate measurement of rotation count, angular position, and speed.

.. warning:: Encoders use digital inputs DIN_1 to DIN_4 of the OI-Stepper module. When you use an encoder, the corresponding inputs are no longer available as standard digital inputs.

.. image:: ../_static/encoder_connection.png
    :width: 500
    :alt: Example of connection for an encoder
    :align: center

|

Characteristics
---------------

.. list-table:: Encoder specifications
   :widths: 33 33 33
   :header-rows: 1
   :align: center

   * - Requirements
     - Value
     - Remark
   * - Voltage
     - 5 - 30V
     - 
   * - Maximum number of encoders
     - 2
     - 

Code examples
-------------

The example code below demonstrates how to use an encoder with the OI-Stepper module:

.. literalinclude:: ../../../examples/Encoder.cpp
    :language: cpp

Software API
------------

.. doxygenclass:: Encoder
    :members:
