/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file CommandStepper.h
 * 
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#if defined(MODULE_MASTER)

#include "Command.h"
#include "MotorStepper.h"

class CommandStepper
{

public:

    CommandStepper(Command* control);
    
    /**
     * @brief Attach a limit switch to the specified motor
     * The first limit switch attached will be used for homing.
     * Limit switches will be used for stopping motor when they will be reached
     * 
     * @param motor 
     * @param dinNum
     * @param logic 
     */
    void attachLimitSwitch(MotorNum_t motor, DIn_Num_t din, Logic_t logic=ACTIVE_HIGH);
    
    /**
     * @brief Detach a limit switch to the specified motor
     * 
     * @param motor 
     * @param dinNum
     */
    void detachLimitSwitch(MotorNum_t motor, DIn_Num_t din);

    /**
     * @brief Set the Step Resolution. Default resolution is 1/16 step
     * 
     * @param motor 
     * @param res 
     */
    void setStepResolution(MotorNum_t motor, MotorStepResolution_t res);
    
    
    /**
     * @brief Set acceleration speed
     * 
     * @param motor 
     * @param speed step/s
     */
    void setAcceleration(MotorNum_t motor, float acc);
    
    /**
     * @brief Set deceleration speed
     * 
     * @param motor 
     * @param speed step/s
     */
    void setDeceleration(MotorNum_t motor, float dec);
    
    
    /**
     * @brief Set max speed
     * 
     * @param motor 
     * @param speed step/s
     */
    void setMaxSpeed(MotorNum_t motor, float speed);
    
    /**
     * @brief Set min speed
     * 
     * @param motor 
     * @param speed step/s
     */
    void setMinSpeed(MotorNum_t motor, float speed);
    
    /**
     * @brief Set full step speed. Above this speed, the motor will run in full step
     * 
     * @param motor 
     * @param speed step/s
     */
    void setFullStepSpeed(MotorNum_t motor, float speed);
    
    /**
     * @brief Get the current position
     * 
     * @param motor Motor num
     * @return int32_t position in step
     */
    int32_t getPosition(MotorNum_t motor);
    
    /**
     * @brief Get the current speed
     * 
     * @param motor Motor num
     * @return float speed in step/s
     */
    float getSpeed(MotorNum_t motor);
    
    /**
     * @brief Reset home position without perform homing
     * 
     * @param motor 
     */
    void resetHomePosition(MotorNum_t motor);

    /**
     * @brief Stop the motor
     * 
     * @param motor Motor num
     * @param mode Mode [SOFT_STOP, HARD_STOP, SOFT_HIZ, HARD_HIZ]
     */
    void stop(MotorNum_t motor, MotorStopMode_t mode=SOFT_HIZ);
    
    /**
     * @brief Go to position
     * 
     * @param motor
     * @param position Absolute position
     * @param microStep Default to false. If true, position will be read as microstep position 
     */
    void moveAbsolute(MotorNum_t motor, uint32_t position, bool microStep=false);
    
    /**
     * @brief Move
     * 
     * @param motor 
     * @param position Absolute position, can be positive or negative
     * @param microStep Default to false. If true, position will be read as microstep position 
     */
    void moveRelative(MotorNum_t motor, int32_t position, bool microStep=false);
    
    /**
    * @brief Stepper Run command
    * 
    * @param[in] motor Motor num
    * @param[in] direction Movement direction (FORWARD, BACKWARD)
    * @param[in] speed in step/s
    * @return None
    */
    void run(MotorNum_t motor, MotorDirection_t direction, float speed);
    
    /**
     * @brief Wait for motor to finish is action
     * If motor is moving with a 'run' command, wait will return as soon as the required speed is reach.
     * 
     * @param motor
     */
    void wait(MotorNum_t motor);
    
    /**
     * @brief Run task to perform homing. Please set a limit switch before performing a homing.
     * The motor will run in reverse until it reach the limit switch. Then it will move at min speed until it move out of the sensor.
     * The position will be set at the home position.
     * 
     * @param motor 
     * @param speed
     */
    void homing(MotorNum_t motor, float speed);

private:

    Command* _control;
    xQueueHandle _motorWaitEvent[MOTOR_MAX];
};

#endif