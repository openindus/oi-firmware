/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file OIStepper.h
 * @brief Functions for stepper module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"

#include "powerSTEP01/powerSTEP01.h"
#include "L6470/L6470.h"

#include "OIBus.h"
#include "OIModule.h"
#include "OIType.h"


/************ DEFINES FOR BOI13 STEPPER MODULE *******************/
#if defined(CONFIG_OI_STEPPER)

#define OISTEPPER_NUMBER_OF_DEVICES     (2)

/* SPI pins */
#define OISTEPPER_SPI_PIN_MOSI          GPIO_NUM_37
#define OISTEPPER_SPI_PIN_MISO          GPIO_NUM_35
#define OISTEPPER_SPI_PIN_CLK           GPIO_NUM_36
#define OISTEPPER_SPI_PIN_CS            GPIO_NUM_34
/* SPI Device Host */
#define OISTEPPER_SPI_HOST              FSPI_HOST
#define OISTEPPER_SPI_FREQ              SPI_MASTER_FREQ_8M

#define ESP_INTR_FLAG_DEFAULT           (0)

/* ETOR */
#define OISTEPPER_NB_ETORS              (4)
#define OISTEPPER_GPIO_PIN_ETOR1        GPIO_NUM_41
#define OISTEPPER_GPIO_PIN_ETOR2        GPIO_NUM_40
#define OISTEPPER_GPIO_PIN_ETOR3        GPIO_NUM_39
#define OISTEPPER_GPIO_PIN_ETOR4        GPIO_NUM_38

/* Device 1 GPIOs pins */
#define OISTEPPER_GPIO_PIN_D1_SW         GPIO_NUM_42
#define OISTEPPER_GPIO_PIN_D1_STBY_RST   GPIO_NUM_11

/* Device 2 GPIOs pins */
#define OISTEPPER_GPIO_PIN_D2_SW         GPIO_NUM_15
#define OISTEPPER_GPIO_PIN_D2_STBY_RST   GPIO_NUM_16

/* Common GPIO pins */
#define OISTEPPER_GPIO_PIN_BUSY_SYNC     GPIO_NUM_12
#define OISTEPPER_GPIO_PIN_FLAG          GPIO_NUM_14

/* step clock */
#define OISTEPPER_PWM_PIN_STCK          GPIO_NUM_13
#define OISTEPPER_PWM_TIMER             LEDC_TIMER_1
#define OISTEPPER_PWM_MODE              LEDC_LOW_SPEED_MODE
#define OISTEPPER_PWM_CHANNEL           LEDC_CHANNEL_0

/************ DEFINES FOR STEPPER VERTICAL MODULE *******************/
#elif defined(CONFIG_OI_STEPPER_VERTICAL)

#define OISTEPPER_NUMBER_OF_DEVICES     (1)

/* SPI pins */
#define OISTEPPER_SPI_PIN_MOSI          GPIO_NUM_35
#define OISTEPPER_SPI_PIN_MISO          GPIO_NUM_37
#define OISTEPPER_SPI_PIN_CLK           GPIO_NUM_36
#define OISTEPPER_SPI_PIN_CS            GPIO_NUM_34
/* SPI Device Host */
#define OISTEPPER_SPI_HOST              FSPI_HOST
#define OISTEPPER_SPI_FREQ              SPI_MASTER_FREQ_8M

#define ESP_INTR_FLAG_DEFAULT           (0)

/* ETOR */
#define OISTEPPER_NB_ETORS              (6)
#define OISTEPPER_GPIO_PIN_ETOR1        GPIO_NUM_13
#define OISTEPPER_GPIO_PIN_ETOR2        GPIO_NUM_6
#define OISTEPPER_GPIO_PIN_ETOR3        GPIO_NUM_8
#define OISTEPPER_GPIO_PIN_ETOR4        GPIO_NUM_12
#define OISTEPPER_GPIO_PIN_ETOR5        GPIO_NUM_11
#define OISTEPPER_GPIO_PIN_ETOR6        GPIO_NUM_9

/* STOR */
#define OISTEPPER_GPIO_PIN_STOR1        GPIO_NUM_15
#define OISTEPPER_GPIO_PIN_STOR2        GPIO_NUM_16
#define OISTEPPER_GPIO_PIN_STOR1_STAT   GPIO_NUM_14
#define OISTEPPER_GPIO_PIN_STOR2_STAT   GPIO_NUM_45

/* Device 1 GPIOs pins */
#define OISTEPPER_GPIO_PIN_D1_SW         GPIO_NUM_19
#define OISTEPPER_GPIO_PIN_D1_STBY_RST   GPIO_NUM_20

/* Common GPIO pins */
#define OISTEPPER_GPIO_PIN_BUSY_SYNC     GPIO_NUM_38
#define OISTEPPER_GPIO_PIN_FLAG          GPIO_NUM_26

/* step clock */
#define OISTEPPER_PWM_PIN_STCK          GPIO_NUM_33
#define OISTEPPER_PWM_TIMER             LEDC_TIMER_1
#define OISTEPPER_PWM_MODE              LEDC_LOW_SPEED_MODE
#define OISTEPPER_PWM_CHANNEL           LEDC_CHANNEL_0

#endif


typedef enum {
    MOTOR1 = 0,
    MOTOR2 = 1,
    NUMBER_OF_MOTORS,
} Motor_t;


class OIStepperInterface
{
    /**
     * @brief Get Etor Level
     * 
     * @param etor 
     * @return int level
     */
    virtual int getEtorLevel(Etor_t etor) = 0;

    /**
    * @brief Set switch logic. Default is normally open (NO).
    * @param[in] no_logic True for normally open logic, false to set normally closed logic.
    */
    virtual void setSwitchLogic(bool no_logic) = 0;

    /**
    * @brief Set an Etor connected to a limit switch to stop a motor, the user can register a callback when an interrupt occurs
    * @param[in] etor ETOR to attach interrupt
    * @param[in] motor Motor num
    * @param[in] callback (optionnal)
    */
    virtual void attachLimitSwitch(Etor_t etor, Motor_t motor, void (*callback)(void) = NULL) = 0;

    /**
    * @brief Detach an Etor connected to a limit switch to stop a motor
    * @param[in] etor ETOR to attach interrupt
    */
    virtual void detachLimitSwitch(Etor_t etor) = 0;

    /**
    * @brief  Attaches a user callback to the busy Interrupt
    * The call back will be then called each time the busy 
    * pin is set or reset 
    * @param[in] callback Name of the callback to attach 
    * to the Busy Interrupt
    * @return None
    **/
    virtual void attachBusyInterrupt(void (*callback)(void)) = 0;

    /**
    * @brief  Attaches a user callback to the error Handler.
    * The call back will be then called each time the library 
    * detects an error
    * @param[in] callback Name of the callback to attach 
    * to the error Hanlder
    * @return None
    **/
    virtual void attachErrorHandler(void (*callback)(uint16_t)) = 0;

    /**
    * @brief  Attaches a user callback to the flag Interrupt
    * The call back will be then called each time the status 
    * flag pin will be pulled down due to the occurrence of 
    * a programmed alarms ( OCD, thermal pre-warning or 
    * shutdown, UVLO, wrong command, non-performable command)
    * @param[in] callback Name of the callback to attach 
    * to the Flag Interrupt
    * @return None
    **/
    virtual void attachFlagInterrupt(void (*callback)(void)) = 0;

    /**
    * @brief Issues OIStepper Get Status command
    * @param[in] motor Motor num
    * @return Status Register content
    */
    virtual uint16_t getStatus(Motor_t motor) const = 0;

    /**
    * @brief Issues OIStepper Go Home command (Shortest path to zero position)
    * @param[in] motor Motor num
    * @return None
    */
    virtual void goHome(Motor_t motor) const = 0;

    /**
    * @brief Issues OIStepper Go Mark command
    * @param[in] motor Motor num
    * @return None
    */
    virtual void goMark(Motor_t motor) const = 0;

    /**
    * @brief Issues OIStepper Go To command
    * @param[in] motor Motor num
    * @param[in] abs_pos absolute position in steps in agreement
    * with the step mode where requested to move
    * @return None
    */
    virtual void goTo(Motor_t motor, int32_t abs_pos) const = 0;

    /**
    * @brief Issues OIStepper Go To Dir command
    * @param[in] motor Motor num
    * @param[in] direction movement direction
    * @param[in] abs_pos absolute position in steps in agreement
    * with the step mode where requested to move
    * @return None
    */
    virtual void goToDir(Motor_t motor, motorDir_t direction, int32_t abs_pos) const = 0;

    /**
    * @brief Issues OIStepper Go Until command
    * @param[in] motor Motor num
    * @param[in] action ACTION_RESET or ACTION_COPY
    * @param[in] direction movement direction
    * @param[in] speed in 2^-28 step/tick
    * @return None
    */
    virtual void goUntil(Motor_t motor, motorAction_t action, motorDir_t direction, uint32_t speed) const = 0;

    /**
    * @brief Issues OIStepper Hard HiZ command
    * @param[in] motor Motor num
    * @return None
    * @note The HardHiZ command immediately disables the power bridges
    * (high impedance state) and raises the HiZ flag. 
    * When the motor is stopped, a HardHiZ command forces the bridges 
    * to enter high impedance state.
    * This command can be given anytime and is immediately executed.
    */
    virtual void hardHiZ(Motor_t motor) const = 0;

    /**
    * @brief Issues OIStepper Hard Stop command
    * @param[in] motor Motor num
    * @return None
    * @note The HardStop command causes an immediate motor stop with
    * infinite deceleration.
    * When the motor is in high impedance state, a HardStop command
    * forces the bridges to exit high impedance state; no motion is performed.
    * This command can be given anytime and is immediately executed.
    * This command keeps the BUSY flag low until the motor is stopped.
    */
    virtual void hardStop(Motor_t motor) const = 0;
    
    /**
    * @brief Issues OIStepper Move command
    * @param[in] motor Motor num
    * @param[in] direction Movement direction
    * @param[in] n_step number of steps
    * @return None
    */
    virtual void move(Motor_t motor, motorDir_t direction, uint32_t n_step) const = 0;

    /**
    * @brief Issues OIStepper Release SW command
    * @param[in] motor Motor num
    * @param[in] action type of action to undertake when the SW
    * input is forced high
    * @param[in] direction movement direction
    * @return None
    */
    virtual void releaseSw(Motor_t motor, motorAction_t action, motorDir_t direction) const = 0;

    /**
    * @brief Issues OIStepper Reset Device command
    * @param[in] motor Motor num
    * @return None
    */
    virtual void resetDevice(Motor_t motor) const = 0;

    /**
    * @brief Issues OIStepper Reset Pos command
    * @param[in] motor Motor num
    * @return None
    */
    virtual void resetPos(Motor_t motor) const = 0;

    /**
    * @brief Issues OIStepper Run command
    * @param[in] motor Motor num
    * @param[in] direction Movement direction (FORWARD, BACKWARD)
    * @param[in] speed in 2^-28 step/tick
    * @return None
    */
    virtual void run(Motor_t motor, motorDir_t direction, uint32_t speed) const = 0;

    /**
    * @brief Issues OIStepper Soft HiZ command
    * @param[in] motor Motor num
    * @return None
    * @note The SoftHiZ command disables the power bridges
    * (high impedance state) after a deceleration to zero.
    * The deceleration value used is the one stored in the DEC register.
    * When bridges are disabled, the HiZ flag is raised.
    * When the motor is stopped, a SoftHiZ command forces the bridges
    * to enter high impedance state.
    * This command can be given anytime and is immediately executed.
    * This command keeps the BUSY flag low until the motor is stopped.
    */
    virtual void softHiZ(Motor_t motor) const = 0;

    /**
    * @brief Issues OIStepper Soft Stop command
    * @param[in] motor Motor num
    * @return None
    * @note The SoftStop command causes an immediate deceleration
    * to zero speed and a consequent motor stop.
    * The deceleration value used is the one stored in the DEC register.
    * When the motor is in high impedance state, a SoftStop
    * command forces the bridges to exit from high impedance state.
    * No motion is performed.
    * This command can be given anytime and is immediately executed.
    * This command keeps the BUSY flag low until the motor is stopped.
    */
    virtual void softStop(Motor_t motor) const = 0;

    /**
    * @brief Issues OIStepper Step Clock command
    * @param[in] motor Motor num
    * @param[in] direction Movement direction (FORWARD, BACKWARD)
    * @return None
    */
    virtual void stepClock(Motor_t motor, motorDir_t direction) const = 0;

    /**
    * @brief  Returns the mark position  of the specified device
    * @param[in] motor Motor num
    * @return Mark register value converted in a 32b signed integer 
    **/
    virtual int32_t getMark(Motor_t motor) const = 0;

    /**
    * @brief  Returns the ABS_POSITION of the specified device
    * @param[in] motor Motor num
    * @return ABS_POSITION register value converted in a 32b signed integer
    **/
    virtual int32_t getPosition(Motor_t motor) const = 0;

    /**
    * @brief Checks if the specified motor is busy
    * by reading the Busy flag bit ot its status Register
    * @param[in] motor Motor num
    * @param software if true, check register else check pin
    * @return true if motor is busy, false zero
    */
    virtual bool busyCheck(Motor_t motor, bool software) const = 0;

    /**
    * @brief  Set the stepping mode 
    * @param[in] motor Motor num
    * @param[in] stepMode from full step to 1/128 microstep as specified in enum motorStepMode_t
    * @return TRUE if successfull, FALSE if failure
    **/
    virtual bool selectStepMode(Motor_t motor, motorStepMode_t stepMode) const = 0;

    /**
    * @brief  Sets Home Position 
    * (ABS pos set to current pos minus new home position)
    * @param[in] motor Motor num
    * @param[in] homePos new home position
    * @return None
    **/
    virtual void setHome(Motor_t motor, int32_t homePos) const = 0;

    /**
    * @brief  Sets Mark position 
    * @param[in] motor Motor num
    * @param[in] markPos new mark position
    * @return None
    **/
    virtual void setMark(Motor_t motor, int32_t markPos) const = 0;

    /**
    * @brief Issues the SetParam command to the OIStepper of the specified device
    * @param[in] motor Motor num
    * @param[in] param Register adress (Powerstep01_ABS_POS, Powerstep01_MARK,...)
    * @param[in] value Value to set in the register
    * @return None
    */
    virtual void setParam(Motor_t motor, uint32_t param, uint32_t value) const = 0;

    /**
    * @brief Issues OIStepper Get Param command
    * @param[in] motor Motor num
    * @param[in] param OIStepper register address
    * @return Register value - 1 to 3 bytes (depends on register)
    */
    virtual uint32_t getParam(Motor_t motor, uint32_t param) const = 0;
};


#if (defined CONFIG_OI_STEPPER) || (defined CONFIG_OI_STEPPER_VERTICAL)

class OIStepper : public OIModuleMaster, public OIStepperInterface
{
public:

    OIStepper() : OIModuleMaster() {}

    /***************************/
    /* Hardware initialization */
    /***************************/

    void init(void);

    /********/
    /* ETOR */
    /********/

    void setSwitchLogic(bool no_logic);
    int getEtorLevel(Etor_t etor);

    void attachEtorInterrupt(Etor_t etor, void (*callback)(void));
    void detachEtorInterrupt(Etor_t etor);

    void attachLimitSwitch(Etor_t etor, Motor_t motor, bool notify = false);
    void attachLimitSwitch(Etor_t etor, Motor_t motor, void (*callback)(void) = NULL) {};
    void detachLimitSwitch(Etor_t etor);


    /********/
    /* STOR */
    /********/

    //TODO

    /*********/
    /* MOTOR */
    /*********/

    void attachBusyInterrupt(void (*callback)(void));
    void attachErrorHandler(void (*callback)(uint16_t));
    void attachFlagInterrupt(void (*callback)(void));

    bool busyCheck(Motor_t motor, bool software=true) const;

    /* Configuration */
    int32_t getPosition(Motor_t motor) const;
    int32_t getMark(Motor_t motor) const;
    float getSpeed(Motor_t motor) const;
    float getAcceleration(Motor_t motor) const;
    float getDeceleration(Motor_t motor) const;
    float getMaxSpeed(Motor_t motor) const;
    float getMinSpeed(Motor_t motor) const;
    float getFsSpeed(Motor_t motor) const;
    float getKValHold(Motor_t motor) const;
    float getKValRun(Motor_t motor) const;
    float getKValAcceleration(Motor_t motor) const;
    float getKValDeceleration(Motor_t motor) const;

    void setHome(Motor_t motor, int32_t position) const;
    void setMark(Motor_t motor, int32_t position) const;
    void setAcceleration(Motor_t motor, float acceleration) const;
    void setDeceleration(Motor_t motor, float deceleration) const;
    void setMaxSpeed(Motor_t motor, float speed) const;
    void setMinSpeed(Motor_t motor, float speed) const;
    void setFsSpeed(Motor_t motor, float speed) const;
    void setKValHold(Motor_t motor, float kval) const;
    void setKValRun(Motor_t motor, float kval) const;
    void setKValAcceleration(Motor_t motor, float kval) const;
    void setKValDeceleration(Motor_t motor, float kval) const;

    bool selectStepMode(Motor_t motor, motorStepMode_t stepMode) const;

    /* Commands */
    void setParam(Motor_t motor, uint32_t param, uint32_t value) const;
    uint32_t getParam(Motor_t motor, uint32_t param) const;
    void run(Motor_t motor, motorDir_t direction, uint32_t speed) const;
    void stepClock(Motor_t motor, motorDir_t direction) const;
    void move(Motor_t motor, motorDir_t direction, uint32_t n_step) const;
    void goTo(Motor_t motor, int32_t abs_pos) const;
    void goToDir(Motor_t motor, motorDir_t direction, int32_t abs_pos) const;
    void goUntil(Motor_t motor, motorAction_t action, motorDir_t direction, uint32_t speed) const;
    void releaseSw(Motor_t motor, motorAction_t action, motorDir_t direction) const;
    void goHome(Motor_t motor) const;
    void goMark(Motor_t motor) const;
    void resetPos(Motor_t motor) const;
    void resetDevice(Motor_t motor) const;
    void softStop(Motor_t motor) const;
    void hardStop(Motor_t motor) const;
    void softHiZ(Motor_t motor) const;    
    void hardHiZ(Motor_t motor) const;
    uint16_t getStatus(Motor_t motor) const;

private:

    /* Event */
    static TaskHandle_t _taskHandle;
    static EventGroupHandle_t _eventGroupHandle;

    static void _handleEvent(void *pvParameters);

    static int _limitSwitchToMotor[4];
    static bool _limitSwitchToNotify[4];
    static bool _no_logic; 

    static const gpio_num_t _etor[OISTEPPER_NB_ETORS];

    #ifdef CONFIG_L6470
    static L6470_DeviceConfig_t device_conf;
    #else
    static Powerstep01_DeviceConfig_t device_conf;
    #endif
};

#else

class OIStepper : public OIModuleSlave, public OIStepperInterface
{
public:

    OIStepper(uint8_t id) : OIModuleSlave(id) {}

    inline int getEtorLevel(Etor_t etor) {
        return getMessage(OIMessage(CMD_GET_ETOR_LEVEL, _senderId, static_cast<uint16_t>(etor)));
    }

    inline void setSwitchLogic(bool no_logic)
    {
        setMessage(OIMessage(CMD_SET_LOGIC_SWITCH, _senderId, static_cast<uint16_t>(no_logic)));
    }

    inline void attachLimitSwitch(Etor_t etor, Motor_t motor, void (*callback)(void) = NULL) {
        if ((callback != NULL))
        {
            setMessage(OIMessage(CMD_ATTACH_LIMIT_SWITCH, _senderId, static_cast<uint16_t>((etor << 8) | motor), static_cast<uint32_t>(true)));
            CMD.add(OIMessage(CMD_ETOR_INTERRUPT, _senderId), [callback](OIMessage msg) -> uint32_t {
                callback();
                return 0;
            });
        }
        else
        {
            setMessage(OIMessage(CMD_ATTACH_LIMIT_SWITCH, _senderId, static_cast<uint16_t>((etor << 8) | motor), static_cast<uint32_t>(false)));
        }
    }

    inline void detachLimitSwitch(Etor_t etor) {
        setMessage(OIMessage(CMD_DETACH_LIMIT_SWITCH, _senderId, static_cast<uint16_t>(etor)));
    }

    inline void attachBusyInterrupt(void (*callback)(void)) {
        setMessage(OIMessage(CMD_ATTACH_BUSY_INTERRUPT, _senderId));
        CMD.add(OIMessage(CMD_BUSY_INTERRUPT, _senderId), [callback](OIMessage msg) -> uint32_t {
            callback();
            return 0;
        });
    }

    inline void attachErrorHandler(void (*callback)(uint16_t)) {
        setMessage(OIMessage(CMD_ATTACH_ERROR_HANDLER, _senderId));
        CMD.add(OIMessage(CMD_ERROR_HANDLER, _senderId), [callback](OIMessage msg) -> uint32_t {
            callback((uint16_t)msg.getData());
            return 0;
        });
    }

    inline void attachFlagInterrupt(void (*callback)(void)) {
        setMessage(OIMessage(CMD_ATTACH_FLAG_INTERRUPT, _senderId));
        CMD.add(OIMessage(CMD_FLAG_INTERRUPT, _senderId), [callback](OIMessage msg) -> uint32_t {
            callback();
            return 0;
        });
    }

    inline uint16_t getStatus(Motor_t motor) const {
        return (uint16_t)getMessage(OIMessage(CMD_GET_STATUS, _senderId, motor));
    }

    inline void goHome(Motor_t motor) const {
        setMessage(OIMessage(CMD_GO_HOME, _senderId, motor));
    }

    inline void goMark(Motor_t motor) const {
        setMessage(OIMessage(CMD_GO_MARK, _senderId, motor));
    }

    inline void goTo(Motor_t motor, int32_t abs_pos) const {
        setMessage(OIMessage(CMD_GO_TO, _senderId, motor, abs_pos));
    }

    inline void goToDir(Motor_t motor, motorDir_t direction, int32_t abs_pos) const {
        setMessage(OIMessage(CMD_GO_TO_DIR, _senderId, (uint16_t)((direction << 8) | motor), abs_pos));
    }

    inline void goUntil(Motor_t motor, motorAction_t action, motorDir_t direction, uint32_t speed) const {
        setMessage(OIMessage(CMD_GO_UNTIL, _senderId, (uint16_t)((action << 12) | (direction << 8) | motor), speed));
    }

    inline void hardHiZ(Motor_t motor) const {
        setMessage(OIMessage(CMD_HARD_HIZ, _senderId, motor));
    }

    inline void hardStop(Motor_t motor) const {
        setMessage(OIMessage(CMD_HARD_STOP, _senderId, motor));
    }

    inline void move(Motor_t motor, motorDir_t direction, uint32_t n_step) const {
        setMessage(OIMessage(CMD_MOVE, _senderId, (uint16_t)((direction << 8) | motor), n_step));
    }

    inline void releaseSw(Motor_t motor, motorAction_t action, motorDir_t direction) const {
        setMessage(OIMessage(CMD_RELEASE_SW, _senderId, (uint16_t)((action << 12) | (direction << 8) | motor)));
    }

    inline void resetDevice(Motor_t motor) const {
        setMessage(OIMessage(CMD_RESET_DEVICE, _senderId, motor));
    }

    inline void resetPos(Motor_t motor) const {
        setMessage(OIMessage(CMD_RESET_POS, _senderId, motor));
    }

    inline void run(Motor_t motor, motorDir_t direction, uint32_t speed) const {
        setMessage(OIMessage(CMD_RUN, _senderId, (uint16_t)((direction << 8) | motor), speed));
    }

    inline void softHiZ(Motor_t motor) const {
        setMessage(OIMessage(CMD_SOFT_HIZ, _senderId, motor));
    }

    inline void softStop(Motor_t motor) const {
        setMessage(OIMessage(CMD_SOFT_STOP, _senderId, motor));
    }

    inline void stepClock(Motor_t motor, motorDir_t direction) const {
        setMessage(OIMessage(CMD_STEP_CLOCK, _senderId, (uint16_t)((direction << 8) | motor)));
    }

    inline int32_t getMark(Motor_t motor) const {
        return getMessage(OIMessage(CMD_GET_MARK, _senderId, motor));
    }

    inline int32_t getPosition(Motor_t motor) const {
        return getMessage(OIMessage(CMD_GET_POSITION, _senderId, motor));
    }

    bool busyCheck(Motor_t motor, bool software=0) const {
        return (bool)getMessage(OIMessage(CMD_BUSY_CHECK, _senderId, motor));
    }

    bool selectStepMode(Motor_t motor, motorStepMode_t stepMode) const {
        return (bool)getMessage(OIMessage(CMD_SELECT_STEP_MODE, _senderId, (uint16_t)((stepMode << 8) | motor)));
    }

    void sendQueuedCommands(void) const {
        sendMessage(OIMessage(CMD_SEND_QUEUED_COMMANDS, _senderId));
    }

    void setHome(Motor_t motor, int32_t homePos) const {
        setMessage(OIMessage(CMD_SET_HOME, _senderId, motor, homePos));
    }

    void setMark(Motor_t motor, int32_t markPos) const {
        setMessage(OIMessage(CMD_SET_MARK, _senderId, motor, markPos));
    }

    void setParam(Motor_t motor, uint32_t param, uint32_t value) const {
        setMessage(OIMessage(CMD_SET_PARAM, _senderId, (uint16_t)(((uint8_t)param << 8) | motor), value));
    }

    uint32_t getParam(Motor_t motor, uint32_t param) const {
        return getMessage(OIMessage(CMD_GET_PARAM, _senderId, (uint16_t)(((uint8_t)param << 8) | motor)));
    }

};

#endif /* (defined CONFIG_OI_STEPPER) || (defined CONFIG_OI_STEPPER_VERTICAL) */