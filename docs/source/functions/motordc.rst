.. _motordc_s:

DC Motor
========

Description
-----------

DC motor drivers are available on the following module:

* :ref:`OI-Dc` (x4)

**Main characteristics**

* Bidirectional control: Supports both forward and reverse rotation.
* Variable speed control: PWM-based speed regulation with duty cycle from 0% to 100%.
* Current monitoring: Real-time current measurement for each motor.
* Fault detection: Hardware fault pin monitoring for driver protection.
* High current capability: Designed to drive DC motors with appropriate current ratings.

The OI-Dc module uses H-bridge motor drivers to control DC motors. Each motor driver has two control inputs (IN1 and IN2) that are controlled via PWM to regulate motor speed and direction, and a disable pin for enabling/disabling the motor.

.. note:: 
  The motor drivers operate at a fixed PWM frequency of 5 kHz, which provides a good balance between motor performance and switching losses.

Characteristics
---------------

.. list-table:: DC motor driver specifications
   :widths: 33 33 33
   :header-rows: 1
   :align: center

   * - Parameter
     - Value
     - Remark
   * - Number of motors
     - 4
     - Independent control
   * - Power supply voltage
     - 9-30V DC
     - Same as module supply
   * - PWM frequency
     - 5 kHz
     - Fixed
   * - PWM resolution
     - 13 bits
     - 8192 steps
   * - Direction control
     - Bidirectional
     - Forward/Reverse
   * - Current monitoring
     - Yes
     - Per motor channel

Code examples
-------------

The example code below demonstrates how to control a DC motor with the OI-Dc module:

.. literalinclude:: ../../examples/DcMotorSimple.cpp
    :language: cpp

This example demonstrates basic motor control operations:

1. **Starting a motor**: Use `run()` with motor number, direction (FORWARD/REVERSE), and duty cycle (0-100%).
2. **Reading current**: Use `getCurrent()` to monitor motor load.
3. **Stopping a motor**: Use `stop()` to disable the motor driver.

.. note::
  The duty cycle parameter in the `run()` function represents the percentage of maximum speed. A value of 100% corresponds to full speed, while 0% effectively stops the motor (though using `stop()` is preferred for this purpose).

Software API
------------

.. doxygenclass:: MotorDc
   :members:
