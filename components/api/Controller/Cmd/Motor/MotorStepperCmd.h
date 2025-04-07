/**
 * @file MotorStepperCmd.h
 * @brief Stepper motor command functions
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#if defined(MODULE_MASTER)

#include "Slave.h"
#include "Master.h"
#include "MotorStepper.h"
#include "MotorStepperParam.h"

class MotorStepperCmd
{
public:
    MotorStepperCmd(Controller* controller);

    /**
     * @brief Attach a limit switch to the specified motor
     * The first limit switch attached will be used for homing.
     * Limit switches will be used for stopping motor when they will be reached
     *
     * @param motor
     * @param dinNum
     * @param logic
     */
    void attachLimitSwitch(MotorNum_t motor, DIn_Num_t din, Logic_t logic = ACTIVE_HIGH);

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
     * @brief Set acceleration
     *
     * @param motor
     * @param acc step/s2
     */
    void setAcceleration(MotorNum_t motor, float acc);

    /**
     * @brief Set deceleration
     *
     * @param motor
     * @param dec step/s2
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
     * @brief Set the absolute position
     *
     * @param motor
     * @param position position in step
     */
    void setPosition(MotorNum_t motor, int32_t position);

    /**
     * @brief Stop the motor
     *
     * @param motor Motor num
     * @param mode Mode [SOFT_STOP, HARD_STOP, SOFT_HIZ, HARD_HIZ]
     */
    void stop(MotorNum_t motor, MotorStopMode_t mode = SOFT_HIZ);

    /**
     * @brief Go to position
     *
     * @param motor
     * @param position Absolute position
     * @param microStep Default to false. If true, position will be read as microstep position
     */
    void moveAbsolute(MotorNum_t motor, uint32_t position, bool microStep = false);

    /**
     * @brief Move
     *
     * @param motor
     * @param position Absolute position, can be positive or negative
     * @param microStep Default to false. If true, position will be read as microstep position
     */
    void moveRelative(MotorNum_t motor, int32_t position, bool microStep = false);

    /**
     * @brief Stepper Run command
     *
     * @param[in] motor Motor num
     * @param[in] direction Movement direction (FORWARD, REVERSE)
     * @param[in] speed in step/s
     * @return None
     */
    void run(MotorNum_t motor, MotorDirection_t direction, float speed);

    /**
     * @brief Wait for motor to finish is action
     * If motor is moving with a 'run' command, wait will return as soon as the required speed is
     * reach.
     *
     * @param motor
     */
    void wait(MotorNum_t motor);

    /**
     * @brief Run task to perform homing. Please set a limit switch before performing a homing.
     * The motor will run in reverse until it reach the limit switch. Then it will move at min speed
     * until it move out of the sensor. The position will be set at the home position.
     *
     * @param motor
     * @param speed
     */
    void homing(MotorNum_t motor, float speed);

    /**
     * @brief Set the advanced parameter
     *
     * @param motor Motor num
     * @param advParam Advanced parameter
     * @param value Value
     * @return int 0 if success, -1 if error
     */
    int setAdvancedParam(MotorNum_t motor, AdvancedParameter_t advParam, void* value);

    /**
     * @brief Get the advanced parameter
     *
     * @param motor Motor num
     * @param advParam Advanced parameter
     * @param value Value
     * @return int 0 if success, -1 if error
     */
    int getAdvancedParam(MotorNum_t motor, AdvancedParameter_t advParam, void* value);

    /**
     * @brief Get the supply voltage
     *
     * @return float Supply voltage in V
     */
    float getSupplyVoltage(void);

    /**
     * @brief Attach a flag interrupt for the given motor
     * 
     * @param callback Callback function
     */
    void attachFlagInterrupt(void (*callback)(MotorNum_t, MotoStepperFlag_t));

    /**
     * @brief Detach flag interrupt callback for the given motor
     *
     */
    void detachFlagInterrupt(void);

private:
    Controller* _controller;
    QueueHandle_t _motorWaitEvent[MOTOR_MAX];
    void (*_flagIsrCallback)(MotorNum_t, MotoStepperFlag_t);

protected:

    friend class Core;

    /* Command line interface */
    static void _registerCLI(void);
};

#endif