/**
 * @file MotorStepper.h
 * @brief MotorStepper class definition
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "DigitalInputs.h"
#include "Motor.h"
#include "powerSTEP01/PS01.h"

/**
 * @brief Stepper motors step modes
 *
 */
typedef enum {
    FULL_STEP  = 0,
    HALF_STEP  = 1,
    STEP_1_4   = 2,
    STEP_1_8   = 3,
    STEP_1_16  = 4,
    STEP_1_32  = 5,
    STEP_1_64  = 6,
    STEP_1_128 = 7
} MotorStepResolution_t;

typedef enum {
    FLAG_CMD_ERROR   = 0, // Command error
    FLAG_UVLO        = 1, // Undervoltage lockout
    FLAG_UVLO_ADC    = 2, // VS undervoltage lockout
    FLAG_TH_WARNING  = 3, // Thermal warning
    FLAG_TH_SHUTDOWN = 4, // Thermal shutdown
    FLAG_OCD         = 5, // Overcurrent detection
    FLAG_STALL_A     = 6, // Stall detection (A)
    FLAG_STALL_B     = 7, // Stall detection (B)
} MotoStepperFlag_t;

/**
 * @brief Motor status structure representing PowerStep01 status register
 */
typedef struct {
    uint16_t hiz : 1;           // High impedance state (bit 0)
    uint16_t busy : 1;          // Motor busy (bit 1)
    uint16_t sw_f : 1;          // Switch input status (bit 2)
    uint16_t sw_evn : 1;        // Switch turn-on event (bit 3)
    uint16_t dir : 1;           // Direction of last movement (bit 4)
    uint16_t mot_status : 2;    // Motor status (bits 5-6): 0=stopped, 1=acceleration, 2=deceleration, 3=constant speed
    uint16_t cmd_error : 1;     // Command error (bit 7)
    uint16_t stck_mod : 1;      // Step-clock mode (bit 8)
    uint16_t uvlo : 1;          // Undervoltage lockout (bit 9)
    uint16_t uvlo_adc : 1;      // VS undervoltage lockout (bit 10)
    uint16_t th_status : 2;     // Thermal status (bits 11-12): 0=normal, 1=warning, 2=bridge shutdown, 3=device shutdown
    uint16_t ocd : 1;           // Overcurrent detection (bit 13)
    uint16_t stall_a : 1;       // Stall detection on bridge A (bit 14)
    uint16_t stall_b : 1;       // Stall detection on bridge B (bit 15)
} MotorStepperStatus_t;

/**
 * @brief Stepper motors stop modes
 *
 */
typedef enum { SOFT_STOP = 0, HARD_STOP = 1, SOFT_HIZ = 2, HARD_HIZ = 3 } MotorStopMode_t;

class MotorStepper : public Motor
{
public:
    static int init(PS01_Hal_Config_t *config, PS01_Param_t *param);

    /**
     * @brief Attach a limit switch to the specified motor
     * The first limit switch attached will be used for homing.
     * Limit switches will be used for stopping motor when they will be reached
     *
     * @param motor
     * @param din
     * @param logic
     */
    static void attachLimitSwitch(MotorNum_t motor, DIn_Num_t din, Logic_t logic = ACTIVE_HIGH);

    /**
     * @brief Detach a limit switch to the specified motor
     *
     * @param motor
     * @param din
     */
    static void detachLimitSwitch(MotorNum_t motor, DIn_Num_t din);

    /**
     * @brief Set the Step Resolution. Default resolution is 1/16 step
     *
     * @param motor
     * @param res
     */
    static void setStepResolution(MotorNum_t motor, MotorStepResolution_t res);

    /**
     * @brief Set acceleration
     *
     * @param motor
     * @param acc step/s2
     */
    static void setAcceleration(MotorNum_t motor, float acc);

    /**
     * @brief Set deceleration
     *
     * @param motor
     * @param dec step/s2
     */
    static void setDeceleration(MotorNum_t motor, float dec);

    /**
     * @brief Set max speed
     *
     * @param motor
     * @param speed step/s
     */
    static void setMaxSpeed(MotorNum_t motor, float speed);

    /**
     * @brief Set min speed
     *
     * @param motor
     * @param speed step/s
     */
    static void setMinSpeed(MotorNum_t motor, float speed);

    /**
     * @brief Set full step speed. Above this speed, the motor will run in full step
     *
     * @param motor
     * @param speed step/s
     */
    static void setFullStepSpeed(MotorNum_t motor, float speed);

    /**
     * @brief Get the current position
     *
     * @param motor Motor num
     * @return int32_t position in step
     */
    static int32_t getPosition(MotorNum_t motor);

    /**
     * @brief Get the current speed
     *
     * @param motor Motor num
     * @return float speed in step/s
     */
    static float getSpeed(MotorNum_t motor);

    /**
     * @brief Get the motor status without clearing it
     *
     * @param motor Motor num
     * @return MotorStepperStatus_t Structure containing parsed motor status information
     */
    static MotorStepperStatus_t getStatus(MotorNum_t motor);

    
    /**
     * @brief Clear the motor status and reset errors
     *
     * @param motor Motor num
     * @return MotorStepperStatus_t Structure containing parsed motor status information
     */
    static void clearStatus(MotorNum_t motor);

    /**
     * @brief Reset home position without perform homing
     *
     * @param motor
     */
    static void resetHomePosition(MotorNum_t motor);

    /**
     * @brief Set the absolute position
     *
     * @param motor
     * @param position position in step
     */
    static void setPosition(MotorNum_t motor, int32_t position);

    /**
     * @brief Stop the motor
     *
     * @param motor Motor num
     * @param mode Mode [SOFT_STOP, HARD_STOP, SOFT_HIZ, HARD_HIZ]
     */
    static void stop(MotorNum_t motor, MotorStopMode_t mode = SOFT_HIZ);

    /**
     * @brief Go to position
     *
     * @param motor
     * @param position Absolute position
     * @param microStep Default to false. If true, position will be read as microstep position
     */
    static void moveAbsolute(MotorNum_t motor, uint32_t position, bool microStep = false);

    /**
     * @brief Move
     *
     * @param motor
     * @param position Absolute position, can be positive or negative
     * @param microStep Default to false. If true, position will be read as microstep position
     */
    static void moveRelative(MotorNum_t motor, int32_t position, bool microStep = false);

    /**
     * @brief Stepper Run command
     *
     * @param[in] motor Motor num
     * @param[in] direction Movement direction (FORWARD, REVERSE)
     * @param[in] speed in step/s
     * @return None
     */
    static void run(MotorNum_t motor, MotorDirection_t direction, float speed);

    /**
     * @brief Await end of motor movement.
     * If motor is moving with a 'run' command, wait will return as soon as the required speed is
     * reach.
     *
     * @param motor
     */
    static void wait(MotorNum_t motor);

    /**
     * @brief Run task to perform homing. Please set a limit switch before performing a homing.
     * The motor will run in reverse until it reach the limit switch. Then it will move at min speed
     * until it move out of the sensor. The position will be set at the home position.
     *
     * @param motor
     * @param speed
     */
    static void homing(MotorNum_t motor, float speed);

    /**
     * @brief Get the supply voltage
     *
     * @return float Supply voltage in volts
     */
    static float getSupplyVoltage(void);

    /**
     * @brief Attach a flag interrupt for the given motor
     * 
     * @param callback Callback function
     */
    static void attachFlagInterrupt(void (*callback)(MotorNum_t, MotoStepperFlag_t));

    /**
     * @brief Detach flag interrupt callback for the given motor
     *
     */
    static void detachFlagInterrupt(void);

    /**
     * @brief Trigger the limit switch
     * 
     * @param motor 
     */
    static void triggerLimitSwitch(MotorNum_t motor);

private:

    static int _registerCLI(void);
};
