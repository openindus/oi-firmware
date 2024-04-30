/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file StepperInterface.h
 * @brief Functions for stepper module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"

class StepperInterface
{
public:

    /**
     * @brief Configure limit switch to perform homing
     * 
     * @param motor 
     * @param dinNum
     * @param logic 
     */
    virtual void setLimitSwitch(MotorNum_t motor, DigitalInputNum_t dinNum, DigitalInputLogic_t logic=ACTIVE_HIGH) = 0;

    /**
     * @brief Set the Step Resolution
     * 
     * @param motor 
     * @param res 
     */
    virtual void setStepResolution(MotorNum_t motor, MotorStepResolution_t res) = 0;

    /**
     * @brief Set speed
     * 
     * @param speed step/s
     */
    virtual void setSpeed(MotorNum_t motor, float speed) = 0;

    /**
     * @brief Get the current position
     * 
     * @param motor Motor num
     * @return int32_t position in step
     */
    virtual int32_t getPosition(MotorNum_t motor) = 0;

    /**
     * @brief Get the current speed
     * 
     * @param motor Motor num
     * @return float speed in step/s
     */
    virtual float getSpeed(MotorNum_t motor) = 0;

    /**
     * @brief Reset home position without perform homing
     * 
     * @param motor 
     */
    virtual void resetHomePosition(MotorNum_t motor) = 0;

    /**
     * @brief Stop the motor
     * 
     * @param motor Motor num
     * @param mode Mode [SOFT_STOP, HARD_STOP, SOFT_HIZ, HARD_HIZ]
     */
    virtual void stop(MotorNum_t motor, MotorStopMode_t mode=SOFT_HIZ) = 0;

    /**
     * @brief Go to position
     * 
     * @param motor 
     * @param position 
     * @param microStep 
     */
    virtual void moveAbsolute(MotorNum_t motor, uint32_t position, bool microStep=true) = 0;

    /**
     * @brief Move
     * 
     * @param motor 
     * @param position 
     * @param microStep 
     */
    virtual void moveRelative(MotorNum_t motor, int32_t position, bool microStep=true) = 0;

    /**
    * @brief Stepper Run command
    * 
    * @param[in] motor Motor num
    * @param[in] direction Movement direction (FORWARD, BACKWARD)
    * @param[in] speed in step/s
    * @return None
    */
    virtual void run(MotorNum_t motor, MotorDirection_t direction, float speed) = 0;

    /**
     * @brief Check if motor is running
     * 
     * @param motor 
     * @return true 
     * @return false 
     */
    virtual bool isRunning(MotorNum_t motor) = 0;

    /**
     * @brief Run task to perform homing
     * 
     * @param motor 
     * @param speed
     */
    virtual void homing(MotorNum_t motor, float speed) = 0;
};
