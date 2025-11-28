.. _motorstepper_s:

Stepper Motor
=============

Description
-----------

Stepper motor drivers are available on the following module:

* :ref:`OI-Stepper` (2 channels)

The OI-Stepper board is designed to drive bipolar stepper motors up to NEMA 24 size, with a supply voltage range from 9V to 30V. Each output, powering a motor phase, supports a nominal current of up to 4.2A RMS (6A peak).

The board uses the PowerSTEP01 driver from STMicroelectronics, a high-performance, fully integrated stepper motor driver. The PowerSTEP01 provides advanced motion control, high microstepping resolution, and robust protection features.

**Protections:**
- Overcurrent protection (programmable threshold)
- Overtemperature protection (thermal shutdown)
- Undervoltage lockout (UVLO) on all power supplies
- Short-circuit protection (load and ground)
- Cross-conduction protection (prevents shoot-through)
- Open-load detection (running and standby)
- ESD protection on all pins

Software Features
-----------------
- Rich API for motion and parameter control
- Homing process with limit switch support
- VSCode extension for parameter configuration and testing

Characteristics
---------------

.. list-table:: Stepper Motor Driver Characteristics
     :widths: 30 30 40
     :header-rows: 1
     :align: center

     * - Parameter
         - Value
         - Note
     * - Supported motor size
         - Up to NEMA 24
         - 
     * - Supply voltage
         - 9V – 30V
         - Board limit (PowerSTEP01 supports up to 85V)
     * - Max current per phase
         - 4.2A RMS (6A peak)
         - 
     * - Number of channels
         - 2
         - 
     * - Microstepping
         - Up to 1/128
         - Programmable
     * - Control interface
         - SPI, Step/Dir
         - 
     * - Driver IC
         - PowerSTEP01
         - STMicroelectronics
     
Characteristics
---------------


.. list-table:: Stepper Motor Driver Characteristics
     :widths: 30 30 40
     :header-rows: 1
     :align: center

     * - Parameter
         - Value
         - Note
     * - Supported motor size
         - Up to NEMA 24
         - 
     * - Supply voltage
         - 9V – 30V
         - 
     * - Max current per phase
         - 4.2A RMS (6A peak)
         - 
     * - Number of channels
         - 2
         - 
     * - Microstepping
         - Up to 1/128
         - Programmable
     * - Driver IC
         - PowerSTEP01
         - STMicroelectronics


Operation
---------

Homing
******
The `homing` command allows you to zero the motor position using a limit switch. The switch (mechanical or inductive, PNP type) must be configured before calling `homing`.

The motor moves toward the limit switch at the desired speed. Once triggered, it moves slowly in the opposite direction until the switch is released. This position is set as zero, ensuring repeatable and accurate homing regardless of the initial position or speed.

Speed Control
*************
Speed is managed by three parameters: acceleration, maximum speed, and deceleration. These parameters finely tune the motor's motion profile and are used by the `moveRelative` and `moveAbsolute` functions.

The `run` function spins the motor at a constant speed set by the user, not exceeding the configured `maxSpeed`.

Stop Modes
**********
Several stop modes are available:

- **Soft Stop**: Decelerates to a stop and holds position
- **Hard Stop**: Stops immediately and holds position
- **Soft HiZ**: Decelerates to a stop and enters high-impedance (HiZ) mode
- **Hard HiZ**: Stops immediately and enters HiZ mode

In hold mode, the motor consumes current and may heat up depending on the holding torque. Avoid prolonged holding to prevent overheating.

In HiZ mode, the motor is unpowered and free to move. Ensure your mechanism is safe if the motor is unpowered.

Power Control
*************
Motor torque is set by several parameters:

.. list-table:: PowerSTEP01 Torque Parameters
     :widths: 40 40 20
     :header-rows: 1
     :align: center

     * - Parameter
         - Description
         - Default
     * - KVAL_ACC
         - Acceleration torque
         - 16%
     * - KVAL_DEC
         - Deceleration torque
         - 16%
     * - KVAL_RUN
         - Running torque
         - 16%
     * - KVAL_HOLD
         - Holding torque
         - 16%
     * - Back-EMF compensation
         - Coefficient for compensating back-EMF; increase to boost torque
         - 6%

.. warning::
     Do not increase torque or current parameters too quickly. Excessive values can overheat and damage the motor or driver.

Configuration
*************
Parameters can be set in two ways:
- Via the VSCode extension: parameters are stored in the module and can be exported/imported as JSON
- In code: functions allow setting speeds and other parameters during initialization

Code examples
-------------

The example code below demonstrates how to control a stepper motor with the OI-Stepper module:

.. literalinclude:: ../../examples/StepperMotor.cpp
    :language: cpp

Software API
------------

.. doxygenclass:: MotorStepper
    :members: