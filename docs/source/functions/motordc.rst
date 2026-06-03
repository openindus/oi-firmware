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

PID Position Control
--------------------

The OI-Dc module supports closed-loop position control using a quadrature encoder and a PID controller.
This feature allows you to command a motor to move to an absolute target position (in encoder pulses) and hold it.

**How it works:**

1. Connect a quadrature encoder to two digital inputs (e.g., DIN_1 and DIN_2).
2. The encoder is automatically attached during module initialization.
3. Use ``moveTo()`` to command the motor to a target position.
4. The PID controller continuously adjusts the motor speed to minimize the position error.
5. Use ``stop()`` to immediately brake the motor and disable position control.

.. note::
  PID parameters (Kp, Ki, Kd, output limits, integral limits) can be tuned at runtime using ``setPidParams()``.

**Wiring example:**

.. code-block:: none

    OI-DC Module — PID Position Control
    =====================================

    Power Supply (24V)
         +
         |
    +----|-------------------------+
    |  9V-30V                     |
    |   VIN        OI-DC          |
    |                             |
    |   HB1_1 ─── Motor A         |
    |   HB1_2 ─── Motor B         |
    |                             |
    |   DIN_1 ─── Encoder A       |
    |   DIN_2 ─── Encoder B       |
    |                             |
    |   GND                       |
    +----|-------------------------+
         |
         -
        GND

The example code below demonstrates closed-loop PID position control:

.. literalinclude:: ../../examples/DcMotorPidCtrl.cpp
    :language: cpp

Software API
------------

.. doxygenclass:: MotorDc
   :members:

.. doxygenclass:: MotorDcPidCtrl
   :members:
