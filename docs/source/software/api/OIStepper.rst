.. _OIstepper_h:

OIStepper - Module to control stepper motor
===========================================

Description
-----------

OIStepper is our module to drive two stepper motors synchronously.
The idea of this module API is to give you full control of our motors with all functions available on our controller.
Follow the `Example` section and our :ref:`Projects section<projects-index>` to see complete project based on stepper motors.

Example
-------

.. code-block:: cpp

    #include "OpenIndus.h"
    #include "Arduino.h"

    void setup()
    {
        Serial.begin(115200);

        Stepper.ledOn(LED_CYAN);

        /* Config sensor and move to this position */
        Stepper.attachLimitSwitch(ETOR1, DEVICE1);
        Stepper.cmdGoUntil(DEVICE1, ACTION_RESET, FORWARD, 33554);
    }

    void loop()
    {
        delay(1000);
    }

Functions
---------

.. doxygenfunction:: OIStepper::getEtorLevel
.. doxygenfunction:: OIStepper::attachEtorInterrupt
.. doxygenfunction:: OIStepper::attachBusyInterrupt
.. doxygenfunction:: OIStepper::attachErrorHandler
.. doxygenfunction:: OIStepper::attachFlagInterrupt
.. doxygenfunction:: OIStepper::checkBusyHw
.. doxygenfunction:: OIStepper::checkStatusHw
.. doxygenfunction:: OIStepper::cmdGetStatus
.. doxygenfunction:: OIStepper::cmdGoHome
.. doxygenfunction:: OIStepper::cmdGoMark
.. doxygenfunction:: OIStepper::cmdGoTo
.. doxygenfunction:: OIStepper::cmdGoToDir
.. doxygenfunction:: OIStepper::cmdGoUntil
.. doxygenfunction:: OIStepper::cmdHardHiZ
.. doxygenfunction:: OIStepper::cmdHardStop
.. doxygenfunction:: OIStepper::cmdMove
.. doxygenfunction:: OIStepper::cmdReleaseSw
.. doxygenfunction:: OIStepper::cmdResetDevice
.. doxygenfunction:: OIStepper::cmdResetPos
.. doxygenfunction:: OIStepper::cmdRun
.. doxygenfunction:: OIStepper::cmdSoftHiZ
.. doxygenfunction:: OIStepper::cmdSoftStop
.. doxygenfunction:: OIStepper::cmdStepClock
.. doxygenfunction:: OIStepper::fetchAndClearAllStatus
.. doxygenfunction:: OIStepper::getMark
.. doxygenfunction:: OIStepper::getPosition
.. doxygenfunction:: OIStepper::isDeviceBusy
.. doxygenfunction:: OIStepper::readStatusRegister
.. doxygenfunction:: OIStepper::releaseReset
.. doxygenfunction:: OIStepper::reset
.. doxygenfunction:: OIStepper::selectStepMode
.. doxygenfunction:: OIStepper::sendQueuedCommands
.. doxygenfunction:: OIStepper::setHome
.. doxygenfunction:: OIStepper::setMark
.. doxygenfunction:: OIStepper::startStepClock
.. doxygenfunction:: OIStepper::stopStepClock
.. doxygenfunction:: OIStepper::waitForAllDevicesNotBusy
.. doxygenfunction:: OIStepper::waitWhileActive
.. doxygenfunction:: OIStepper::cmdSetParam
.. doxygenfunction:: OIStepper::cmdGetParam
.. doxygenfunction:: OIStepper::setAnalogValue
.. doxygenfunction:: OIStepper::getAnalogValue
