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

/* ETOR */
#define OISTEPPER_GPIO_PIN_ETOR1        GPIO_NUM_41
#define OISTEPPER_GPIO_PIN_ETOR2        GPIO_NUM_40
#define OISTEPPER_GPIO_PIN_ETOR3        GPIO_NUM_39
#define OISTEPPER_GPIO_PIN_ETOR4        GPIO_NUM_38

/* powerSTEP01 */

/* SPI Device Host */
#define OISTEPPER_SPI_HOST              FSPI_HOST
#define OISTEPPER_SPI_FREQ              SPI_MASTER_FREQ_8M

/* SPI pins */
#define OISTEPPER_SPI_PIN_MOSI          GPIO_NUM_37
#define OISTEPPER_SPI_PIN_MISO          GPIO_NUM_35
#define OISTEPPER_SPI_PIN_CLK           GPIO_NUM_36
#define OISTEPPER_SPI_PIN_CS            GPIO_NUM_34

#ifdef CONFIG_OI_STEPPER
#define OISTEPPER_NUMBER_OF_DEVICES     (2)
#else
#define OISTEPPER_NUMBER_OF_DEVICES     (1)
#endif

/* Device 1 GPIOs pins */
#define OISTEPPER_GPIO_PIN_D1_SW         GPIO_NUM_42
#define OISTEPPER_GPIO_PIN_D1_STBY_RST   GPIO_NUM_11

/* Device 2 GPIOs pins */
#ifndef CONFIG_OI_STEPPERVE
#define OISTEPPER_GPIO_PIN_D2_SW         GPIO_NUM_15
#define OISTEPPER_GPIO_PIN_D2_STBY_RST   GPIO_NUM_16
#endif

/* Common GPIO pins */
#define OISTEPPER_GPIO_PIN_BUSY_SYNC     GPIO_NUM_12
#define OISTEPPER_GPIO_PIN_FLAG          GPIO_NUM_14

/* step clock */
#define OISTEPPER_PWM_PIN_STCK          GPIO_NUM_13
#define OISTEPPER_PWM_TIMER             LEDC_TIMER_1
#define OISTEPPER_PWM_MODE              LEDC_LOW_SPEED_MODE
#define OISTEPPER_PWM_CHANNEL           LEDC_CHANNEL_0

#define ESP_INTR_FLAG_DEFAULT           (0)

#ifdef CONFIG_OI_STEPPER

class OIStepper : public OIModuleMaster
{
public:

    OIStepper() : OIModuleMaster() {}

    /**
     * @brief Initialization
     * 
     */
    void init();

    inline int getEtorLevel(Etor_t etor) {
        return gpio_get_level(_etor[etor]);
    }

    inline void attachEtorInterrupt(Etor_t etor, void (*callback)(void)) {
        gpio_isr_handler_add(_etor[etor], (gpio_isr_t)callback, NULL);
    }

    inline void detachEtorInterrupt(Etor_t etor) {
        gpio_isr_handler_remove(_etor[etor]);
    }

    void attachLimitSwitch(Etor_t etor, uint8_t deviceId, bool notify = false);

    void detachLimitSwitch(Etor_t etor);

    void busyInterruptEvent(void);
    void errorHandlerEvent(void);
    void flagInterruptEvent(void);

    inline void attachBusyInterrupt(void (*callback)(void)) {
        #ifdef CONFIG_L6470
        L6470_AttachBusyInterrupt(callback);
        #else
        Powerstep01_AttachBusyInterrupt(callback);
        #endif
    }

    inline void attachErrorHandler(void (*callback)(uint16_t)) {
        #ifdef CONFIG_L6470
        L6470_AttachErrorHandler(callback);
        #else
        Powerstep01_AttachErrorHandler(callback);
        #endif
    }

    inline void attachFlagInterrupt(void (*callback)(void)) {
        #ifdef CONFIG_L6470
        L6470_AttachFlagInterrupt(callback);
        #else
        Powerstep01_AttachFlagInterrupt(callback);
        #endif
    }

    inline uint8_t checkBusyHw(void) const {
        #ifdef CONFIG_L6470
        return L6470_CheckBusyHw();
        #else
        return Powerstep01_CheckBusyHw();
        #endif
    }

    inline uint8_t checkStatusHw(void) const {
        #ifdef CONFIG_L6470
        return L6470_CheckStatusHw();
        #else
        return Powerstep01_CheckStatusHw();
        #endif
    }

    inline uint16_t cmdGetStatus(uint8_t deviceId) const {
        #ifdef CONFIG_L6470
        return L6470_CmdGetStatus(deviceId);
        #else
        return Powerstep01_CmdGetStatus(deviceId);
        #endif
    }

    inline void cmdGoHome(uint8_t deviceId) const {
        #ifdef CONFIG_L6470
        L6470_CmdGoHome(deviceId);
        #else
        Powerstep01_CmdGoHome(deviceId);
        #endif
    }

    inline void cmdGoMark(uint8_t deviceId) const {
        #ifdef CONFIG_L6470
        L6470_CmdGoMark(deviceId);
        #else
        Powerstep01_CmdGoMark(deviceId);
        #endif
    }

    inline void cmdGoTo(uint8_t deviceId, int32_t abs_pos) const {
        #ifdef CONFIG_L6470
        L6470_CmdGoTo(deviceId, abs_pos);
        #else
        Powerstep01_CmdGoTo(deviceId, abs_pos);
        #endif
    }

    inline void cmdGoToDir(uint8_t deviceId, motorDir_t direction, int32_t abs_pos) const {
        #ifdef CONFIG_L6470
        L6470_CmdGoToDir(deviceId, direction, abs_pos);
        #else
        Powerstep01_CmdGoToDir(deviceId, direction, abs_pos);
        #endif
    }

    inline void cmdGoUntil(uint8_t deviceId, motorAction_t action, motorDir_t direction, uint32_t speed) const {
        #ifdef CONFIG_L6470
        L6470_CmdGoUntil(deviceId, action, direction, speed);
        #else
        Powerstep01_CmdGoUntil(deviceId, action, direction, speed);
        #endif
    }

    inline void cmdHardHiZ(uint8_t deviceId) const {
        #ifdef CONFIG_L6470
        L6470_CmdHardHiZ(deviceId);
        #else
        Powerstep01_CmdHardHiZ(deviceId);
        #endif
    }

    inline void cmdHardStop(uint8_t deviceId) const {
        #ifdef CONFIG_L6470
        L6470_CmdHardStop(deviceId);
        #else
        Powerstep01_CmdHardStop(deviceId);
        #endif
    }

    inline void cmdMove(uint8_t deviceId, motorDir_t direction, uint32_t n_step) const {
        #ifdef CONFIG_L6470
        L6470_CmdMove(deviceId, direction, n_step);
        #else
        Powerstep01_CmdMove(deviceId, direction, n_step);
        #endif
    }

    inline void cmdReleaseSw(uint8_t deviceId, motorAction_t action, motorDir_t direction) const {
        #ifdef CONFIG_L6470
        L6470_CmdReleaseSw(deviceId, action, direction);
        #else
        Powerstep01_CmdReleaseSw(deviceId, action, direction);
        #endif
    }

    inline void cmdResetDevice(uint8_t deviceId) const {
        #ifdef CONFIG_L6470
        L6470_CmdResetDevice(deviceId);
        #else
        Powerstep01_CmdResetDevice(deviceId);
        #endif
    }

    inline void cmdResetPos(uint8_t deviceId) const {
        #ifdef CONFIG_L6470
        L6470_CmdResetPos(deviceId);
        #else
        Powerstep01_CmdResetPos(deviceId);
        #endif
    }

    inline void cmdRun(uint8_t deviceId, motorDir_t direction, uint32_t speed) const {
        #ifdef CONFIG_L6470
        L6470_CmdRun(deviceId, direction, speed);
        #else
        Powerstep01_CmdRun(deviceId, direction, speed);
        #endif
    }

    inline void cmdSoftHiZ(uint8_t deviceId) const {
        #ifdef CONFIG_L6470
        L6470_CmdSoftHiZ(deviceId);
        #else
        Powerstep01_CmdSoftHiZ(deviceId);
        #endif
    }

    inline void cmdSoftStop(uint8_t deviceId) const {
        #ifdef CONFIG_L6470
        L6470_CmdSoftStop(deviceId);
        #else
        Powerstep01_CmdSoftStop(deviceId);
        #endif
    }

    inline void cmdStepClock(uint8_t deviceId, motorDir_t direction) const {
        #ifdef CONFIG_L6470
        L6470_CmdStepClock(deviceId, direction);
        #else
        Powerstep01_CmdStepClock(deviceId, direction);
        #endif
    }

    inline void fetchAndClearAllStatus(void) const {
        #ifdef CONFIG_L6470
        L6470_FetchAndClearAllStatus();
        #else
        Powerstep01_FetchAndClearAllStatus();
        #endif
    }

    inline uint16_t getFetchedStatus(uint8_t deviceId) const {
        #ifdef CONFIG_L6470
        return L6470_GetFetchedStatus(deviceId);
        #else
        return Powerstep01_GetFetchedStatus(deviceId);
        #endif
    }

    inline int32_t getMark(uint8_t deviceId) const {
        #ifdef CONFIG_L6470
        return L6470_GetMark(deviceId);
        #else
        return Powerstep01_GetMark(deviceId);
        #endif
    }

    inline int32_t getPosition(uint8_t deviceId) const {
        #ifdef CONFIG_L6470
        return L6470_GetPosition(deviceId);
        #else
        return Powerstep01_GetPosition(deviceId);
        #endif
    }

    inline bool isDeviceBusy(uint8_t deviceId) const {
        #ifdef CONFIG_L6470
        return L6470_IsDeviceBusy(deviceId);
        #else
        return Powerstep01_IsDeviceBusy(deviceId);
        #endif
    }

    inline void queueCommands(uint8_t deviceId, uint8_t command, int32_t value) const {
        #ifdef CONFIG_L6470
        L6470_QueueCommands(deviceId, command, value);
        #else
        Powerstep01_QueueCommands(deviceId, command, value);
        #endif
    }

    inline uint16_t readStatusRegister(uint8_t deviceId) const {
        #ifdef CONFIG_L6470
        return L6470_ReadStatusRegister(deviceId);
        #else
        return Powerstep01_ReadStatusRegister(deviceId);
        #endif
    }

    inline void releaseReset(uint8_t deviceId) const {
        #ifdef CONFIG_L6470
        L6470_ReleaseReset(deviceId);
        #else
        Powerstep01_ReleaseReset(deviceId);
        #endif
    }

    inline void reset(uint8_t deviceId) const {
        #ifdef CONFIG_L6470
        L6470_Reset(deviceId);
        #else
        Powerstep01_Reset(deviceId);
        #endif
    }

    inline bool selectStepMode(uint8_t deviceId, motorStepMode_t stepMode) const {
        #ifdef CONFIG_L6470
        return L6470_SelectStepMode(deviceId, stepMode);
        #else
        return Powerstep01_SelectStepMode(deviceId, stepMode);
        #endif
    }

    inline void sendQueuedCommands(void) const {
        #ifdef CONFIG_L6470
        L6470_SendQueuedCommands();
        #else
        Powerstep01_SendQueuedCommands();
        #endif
    }

    inline void setHome(uint8_t deviceId, int32_t homePos) const {
        #ifdef CONFIG_L6470
        L6470_SetHome(deviceId, homePos);
        #else
        Powerstep01_SetHome(deviceId, homePos);
        #endif
    }

    inline void setMark(uint8_t deviceId, int32_t markPos) const {
        #ifdef CONFIG_L6470
        L6470_SetMark(deviceId, markPos);
        #else
        Powerstep01_SetMark(deviceId, markPos);
        #endif
    }

    inline void startStepClock(uint16_t newFreq) const {
        #ifdef CONFIG_L6470
        L6470_StartStepClock(newFreq);
        #else
        Powerstep01_StartStepClock(newFreq);
        #endif
    }

    inline void stopStepClock(void) const {
        #ifdef CONFIG_L6470
        L6470_StopStepClock();
        #else
        Powerstep01_StopStepClock();
        #endif
    }

    inline void waitForAllDevicesNotBusy(void) const {
        #ifdef CONFIG_L6470
        L6470_WaitForAllDevicesNotBusy();
        #else
        Powerstep01_WaitForAllDevicesNotBusy();
        #endif
    }

    inline void waitWhileActive(uint8_t deviceId) const {
        #ifdef CONFIG_L6470
        L6470_WaitWhileActive(deviceId);
        #else
        Powerstep01_WaitWhileActive(deviceId);
        #endif
    }

    inline void cmdSetParam(uint8_t deviceId, uint32_t param, uint32_t value) const {
        #ifdef CONFIG_L6470
        L6470_SetMotorConfigToNVS(deviceId, param, (uint16_t)value);
        L6470_CmdSetParam(deviceId, param, value);
        #else
        Powerstep01_SetMotorConfigToNVS(deviceId, param, (uint16_t)value);
        Powerstep01_CmdSetParam(deviceId, param, value);
        #endif
    }
    
    inline uint32_t cmdGetParam(uint8_t deviceId, uint32_t param) const {
        #ifdef CONFIG_L6470
        return L6470_CmdGetParam(deviceId, param);
        #else
        return Powerstep01_CmdGetParam(deviceId, param);
        #endif
    }
    
    inline bool setAnalogValue(uint8_t deviceId, uint32_t param, float value) const {
        #ifdef CONFIG_L6470
        return L6470_SetAnalogValue(deviceId, param, value);
        #else
        return Powerstep01_SetAnalogValue(deviceId, param, value);
        #endif
    }
    
    inline float getAnalogValue(uint8_t deviceId, uint32_t param) const {
        #ifdef CONFIG_L6470
        return L6470_GetAnalogValue(deviceId, param);
        #else
        return Powerstep01_GetAnalogValue(deviceId, param);
        #endif
    }

private:

    /* Event */
    static TaskHandle_t _taskHandle;
    static EventGroupHandle_t _eventGroupHandle;

    static void _handleEvent(void *pvParameters);

    static int _limitSwitchToMotor[4];
    static bool _limitSwitchToNotify[4];

    static const gpio_num_t _etor[4];


    #ifdef CONFIG_L6470
    static L6470_DeviceConfig_t device_conf;
    #else
    static Powerstep01_DeviceConfig_t device_conf;
    #endif
};

#else

class OIStepper : public OIModuleSlave
{
public:

    OIStepper(uint8_t id) : OIModuleSlave(id) {}

    inline int getEtorLevel(Etor_t etor) {
        return getMessage(OIMessage(CMD_GET_ETOR_LEVEL, _senderId, static_cast<uint16_t>(etor)));
    }

    /**
    * @brief Set an Etor connected to a limit switch to stop a motor, the user can register a callback when an interrupt occurs
    * @param[in] etor ETOR to attach interrupt
    * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1)
    * @param[in] callback (optionnal)
    */
    inline void attachLimitSwitch(Etor_t etor, uint8_t deviceId, void (*callback)(void) = NULL) {
        if ((callback != NULL))
        {
            setMessage(OIMessage(CMD_ATTACH_LIMIT_SWITCH, _senderId, static_cast<uint16_t>((etor << 8) | deviceId), static_cast<uint32_t>(true)));
            Fct.add(OIMessage(CMD_ETOR_INTERRUPT, _senderId), [callback](OIMessage msg) -> uint32_t {
                callback();
                return 0;
            });
        }
        else
        {
            setMessage(OIMessage(CMD_ATTACH_LIMIT_SWITCH, _senderId, static_cast<uint16_t>((etor << 8) | deviceId), static_cast<uint32_t>(false)));
        }
    }

    /**
    * @brief Detach an Etor connected to a limit switch to stop a motor
    * @param[in] etor ETOR to attach interrupt
    */
    inline void detachLimitSwitch(Etor_t etor) {
        setMessage(OIMessage(CMD_DETACH_LIMIT_SWITCH, _senderId, static_cast<uint16_t>(etor)));
    }

    /**
    * @brief  Attaches a user callback to the busy Interrupt
    * The call back will be then called each time the busy 
    * pin is set or reset 
    * @param[in] callback Name of the callback to attach 
    * to the Busy Interrupt
    * @return None
    **/
    inline void attachBusyInterrupt(void (*callback)(void)) {
        setMessage(OIMessage(CMD_ATTACH_BUSY_INTERRUPT, _senderId));
        Fct.add(OIMessage(CMD_BUSY_INTERRUPT, _senderId), [callback](OIMessage msg) -> uint32_t {
            callback();
            return 0;
        });
    }

    /**
    * @brief  Attaches a user callback to the error Handler.
    * The call back will be then called each time the library 
    * detects an error
    * @param[in] callback Name of the callback to attach 
    * to the error Hanlder
    * @return None
    **/
    inline void attachErrorHandler(void (*callback)(uint16_t)) {
        setMessage(OIMessage(CMD_ATTACH_ERROR_HANDLER, _senderId));
        Fct.add(OIMessage(CMD_ERROR_HANDLER, _senderId), [callback](OIMessage msg) -> uint32_t {
            callback((uint16_t)msg.getData());
            return 0;
        });
    }

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
    inline void attachFlagInterrupt(void (*callback)(void)) {
        setMessage(OIMessage(CMD_ATTACH_FLAG_INTERRUPT, _senderId));
        Fct.add(OIMessage(CMD_FLAG_INTERRUPT, _senderId), [callback](OIMessage msg) -> uint32_t {
            callback();
            return 0;
        });
    }

    /**
    * @brief Checks if at least one OIStepper is busy by checking 
    * busy pin position. 
    * The busy pin is shared between all devices.
    * @return One if at least one OIStepper is busy, otherwise zero
    **/
    inline uint8_t checkBusyHw(void) const {
        return (uint8_t)getMessage(OIMessage(CMD_CHECK_BUSY_HW, _senderId));
    }

    /**
    * @brief Checks if at least one OIStepper has an alarm flag set
    * by reading flag pin position.
    * The flag pin is shared between all devices.
    * @return One if at least one OIStepper has an alarm flag set ,
    * otherwise zero
    **/
    inline uint8_t checkStatusHw(void) const {
        return (uint8_t)getMessage(OIMessage(CMD_CHECK_STATUS_HW, _senderId));
    }

    /**
    * @brief Issues OIStepper Get Status command
    * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
    * @return Status Register content
    */
    inline uint16_t cmdGetStatus(uint8_t deviceId) const {
        return (uint16_t)getMessage(OIMessage(CMD_GET_STATUS, _senderId, deviceId));
    }

    /**
    * @brief Issues OIStepper Go Home command (Shortest path to zero position)
    * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
    * @return None
    */
    inline void cmdGoHome(uint8_t deviceId) const {
        setMessage(OIMessage(CMD_GO_HOME, _senderId, deviceId));
    }

    /**
    * @brief Issues OIStepper Go Mark command
    * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
    * @return None
    */
    inline void cmdGoMark(uint8_t deviceId) const {
        setMessage(OIMessage(CMD_GO_MARK, _senderId, deviceId));
    }

    /**
    * @brief Issues OIStepper Go To command
    * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
    * @param[in] abs_pos absolute position in steps in agreement
    * with the step mode where requested to move
    * @return None
    */
    inline void cmdGoTo(uint8_t deviceId, int32_t abs_pos) const {
        setMessage(OIMessage(CMD_GO_TO, _senderId, deviceId, abs_pos));
    }

    /**
    * @brief Issues OIStepper Go To Dir command
    * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
    * @param[in] direction movement direction
    * @param[in] abs_pos absolute position in steps in agreement
    * with the step mode where requested to move
    * @return None
    */
    inline void cmdGoToDir(uint8_t deviceId, motorDir_t direction, int32_t abs_pos) const {
        setMessage(OIMessage(CMD_GO_TO_DIR, _senderId, (uint16_t)((direction << 8) | deviceId), abs_pos));
    }

    /**
    * @brief Issues OIStepper Go Until command
    * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
    * @param[in] action ACTION_RESET or ACTION_COPY
    * @param[in] direction movement direction
    * @param[in] speed in 2^-28 step/tick
    * @return None
    */
    inline void cmdGoUntil(uint8_t deviceId, motorAction_t action, motorDir_t direction, uint32_t speed) const {
        setMessage(OIMessage(CMD_GO_UNTIL, _senderId, (uint16_t)((action << 12) | (direction << 8) | deviceId), speed));
    }

    /**
    * @brief Issues OIStepper Hard HiZ command
    * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
    * @return None
    * @note The HardHiZ command immediately disables the power bridges
    * (high impedance state) and raises the HiZ flag. 
    * When the motor is stopped, a HardHiZ command forces the bridges 
    * to enter high impedance state.
    * This command can be given anytime and is immediately executed.
    */
    inline void cmdHardHiZ(uint8_t deviceId) const {
        setMessage(OIMessage(CMD_HARD_HIZ, _senderId, deviceId));
    }

    /**
    * @brief Issues OIStepper Hard Stop command
    * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
    * @return None
    * @note The HardStop command causes an immediate motor stop with
    * infinite deceleration.
    * When the motor is in high impedance state, a HardStop command
    * forces the bridges to exit high impedance state; no motion is performed.
    * This command can be given anytime and is immediately executed.
    * This command keeps the BUSY flag low until the motor is stopped.
    */
    inline void cmdHardStop(uint8_t deviceId) const {
        setMessage(OIMessage(CMD_HARD_STOP, _senderId, deviceId));
    }

    
    /**
    * @brief Issues OIStepper Move command
    * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
    * @param[in] direction Movement direction
    * @param[in] n_step number of steps
    * @return None
    */
    inline void cmdMove(uint8_t deviceId, motorDir_t direction, uint32_t n_step) const {
        setMessage(OIMessage(CMD_MOVE, _senderId, (uint16_t)((direction << 8) | deviceId), n_step));
    }

    /**
    * @brief Issues OIStepper Release SW command
    * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
    * @param[in] action type of action to undertake when the SW
    * input is forced high
    * @param[in] direction movement direction
    * @return None
    */
    inline void cmdReleaseSw(uint8_t deviceId, motorAction_t action, motorDir_t direction) const {
        setMessage(OIMessage(CMD_RELEASE_SW, _senderId, (uint16_t)((action << 12) | (direction << 8) | deviceId)));
    }

    /**
    * @brief Issues OIStepper Reset Device command
    * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
    * @return None
    */
    inline void cmdResetDevice(uint8_t deviceId) const {
        setMessage(OIMessage(CMD_RESET_DEVICE, _senderId, deviceId));
    }

    /**
    * @brief Issues OIStepper Reset Pos command
    * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
    * @return None
    */
    inline void cmdResetPos(uint8_t deviceId) const {
        setMessage(OIMessage(CMD_RESET_POS, _senderId, deviceId));
    }

    /**
    * @brief Issues OIStepper Run command
    * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
    * @param[in] direction Movement direction (FORWARD, BACKWARD)
    * @param[in] speed in 2^-28 step/tick
    * @return None
    */
    inline void cmdRun(uint8_t deviceId, motorDir_t direction, uint32_t speed) const {
        setMessage(OIMessage(CMD_RUN, _senderId, (uint16_t)((direction << 8) | deviceId), speed));
    }

    /**
    * @brief Issues OIStepper Soft HiZ command
    * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
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
    inline void cmdSoftHiZ(uint8_t deviceId) const {
        setMessage(OIMessage(CMD_SOFT_HIZ, _senderId, deviceId));
    }

    /**
    * @brief Issues OIStepper Soft Stop command
    * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
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
    inline void cmdSoftStop(uint8_t deviceId) const {
        setMessage(OIMessage(CMD_SOFT_STOP, _senderId, deviceId));
    }

    /**
    * @brief Issues OIStepper Step Clock command
    * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
    * @param[in] direction Movement direction (FORWARD, BACKWARD)
    * @return None
    */
    inline void cmdStepClock(uint8_t deviceId, motorDir_t direction) const {
        setMessage(OIMessage(CMD_STEP_CLOCK, _senderId, (uint16_t)((direction << 8) | deviceId)));
    }

    /**
    * @brief Fetch and clear status flags of all devices 
    * by issuing a GET_STATUS command simultaneously  
    * to all devices.
    * Then, the fetched status of each device can be retrieved
    * by using the GetFetchedStatus function
    * provided there is no other calls to functions which 
    * use the SPI in between.
    * @return None
    */
    inline void fetchAndClearAllStatus(void) const {
        setMessage(OIMessage(CMD_FETCH_AND_CLEAR_ALL_STATUS, _senderId));
    }

    /**
    * @brief Get the value of the STATUS register which was 
    * fetched by using FetchAndClearAllStatus.
    * The fetched values are available  as long as there
    * no other calls to functions which use the SPI.
    * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
    * @return Last fetched value of the STATUS register
    */
    inline uint16_t getFetchedStatus(uint8_t deviceId) const {
        return (uint16_t)getMessage(OIMessage(CMD_GET_FETCHED_STATUS, _senderId, deviceId));
    }

    /**
    * @brief  Returns the mark position  of the specified device
    * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
    * @return Mark register value converted in a 32b signed integer 
    **/
    inline int32_t getMark(uint8_t deviceId) const {
        return getMessage(OIMessage(CMD_GET_MARK, _senderId, deviceId));
    }

    /**
    * @brief  Returns the ABS_POSITION of the specified device
    * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
    * @return ABS_POSITION register value converted in a 32b signed integer
    **/
    inline int32_t getPosition(uint8_t deviceId) const {
        return getMessage(OIMessage(CMD_GET_POSITION, _senderId, deviceId));
    }

    /**
    * @brief Checks if the specified device is busy
    * by reading the Busy flag bit ot its status Register
    * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
    * @return true if device is busy, false zero
    */
    bool isDeviceBusy(uint8_t deviceId) const {
        return (bool)getMessage(OIMessage(CMD_IS_DEVICE_BUSY, _senderId, deviceId));
    }

    /**
    * @brief Put commands in queue before synchronous sending
    * done by calling SendQueuedCommands.
    * Any call to functions that use the SPI between the calls of 
    * QueueCommands and SendQueuedCommands 
    * will corrupt the queue.
    * A command for each device of the daisy chain must be 
    * specified before calling SendQueuedCommands.
    * @param[in] deviceId deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
    * @param[in] command Command to queue (all OIStepper commmands 
    * except Powerstep01_SET_PARAM, Powerstep01_GET_PARAM, 
    * Powerstep01_GET_STATUS)
    * @param[in] value argument of the command to queue
    * @return None
    */
    void queueCommands(uint8_t deviceId, uint8_t command, int32_t value) const {
        setMessage(OIMessage(CMD_QUEUE_COMMANDS, _senderId, (uint16_t)((command << 8) | deviceId), value));
    }

    /**
    * @brief  Reads the Status Register value
    * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
    * @return Status register value
    * @note The status register flags are not cleared 
    * at the difference with CmdGetStatus()
    **/
    uint16_t readStatusRegister(uint8_t deviceId) const {
        return (uint16_t)getMessage(OIMessage(CMD_READ_STATUS_REGISTER, _senderId, deviceId));
    }

    /**
    * @brief  Releases the OIStepper reset (pin set to High) of all devices
    * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
    * @return None
    **/
    void releaseReset(uint8_t deviceId) const {
        setMessage(OIMessage(CMD_RELEASE_RESET, _senderId, deviceId));
    }

    /**
    * @brief  Resets the OIStepper (reset pin set to low) of all devices
    * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
    * @return None
    **/
    void reset(uint8_t deviceId) const {
        setMessage(OIMessage(CMD_RESET, _senderId, deviceId));
    }

    /**
    * @brief  Set the stepping mode 
    * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
    * @param[in] stepMode from full step to 1/128 microstep as specified in enum motorStepMode_t
    * @return TRUE if successfull, FALSE if failure
    **/
    bool selectStepMode(uint8_t deviceId, motorStepMode_t stepMode) const {
        return (bool)getMessage(OIMessage(CMD_SELECT_STEP_MODE, _senderId, (uint16_t)((stepMode << 8) | deviceId)));
    }

    /**
    * @brief Sends commands stored previously in the queue by 
    * QueueCommands
    * @return None
    */
    void sendQueuedCommands(void) const {
        sendMessage(OIMessage(CMD_SEND_QUEUED_COMMANDS, _senderId));
    }

    /**
    * @brief  Sets Home Position 
    * (ABS pos set to current pos minus new home position)
    * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
    * @param[in] homePos new home position
    * @return None
    **/
    void setHome(uint8_t deviceId, int32_t homePos) const {
        setMessage(OIMessage(CMD_SET_HOME, _senderId, deviceId, homePos));
    }

    /**
    * @brief  Sets Mark position 
    * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
    * @param[in] markPos new mark position
    * @return None
    **/
    void setMark(uint8_t deviceId, int32_t markPos) const {
        setMessage(OIMessage(CMD_SET_MARK, _senderId, deviceId, markPos));
    }
    
    /**
    * @brief  Start the step clock by using the given frequency
    * @param[in] newFreq in Hz of the step clock
    * @return None
    * @note The frequency is directly the current speed of the device
    **/
    void startStepClock(uint16_t newFreq) const {
        setMessage(OIMessage(CMD_START_STEP_CLOCK, _senderId, newFreq));
    }

    /**
    * @brief  Stops the PWM uses for the step clock
    * @return None
    **/
    void stopStepClock(void) const {
        setMessage(OIMessage(CMD_STOP_STEP_CLOCK, _senderId));
    }

    /**
    * @brief  Locks until all devices become not busy
    * @return None
    **/
    void waitForAllDevicesNotBusy(void) const {
        /**
         * @todo add blocking callback
         * 
         */
    }

    /**
    * @brief  Locks until the device becomes not busy
    * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
    * @return None
    **/
    void waitWhileActive(uint8_t deviceId) const {
        /**
         * @todo add blocking callback
         * 
         */
    }

    /**
    * @brief Issues the SetParam command to the OIStepper of the specified device
    * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
    * @param[in] param Register adress (Powerstep01_ABS_POS, Powerstep01_MARK,...)
    * @param[in] value Value to set in the register
    * @return None
    */
    void cmdSetParam(uint8_t deviceId, uint32_t param, uint32_t value) const {
        setMessage(OIMessage(CMD_SET_PARAM, _senderId, (uint16_t)(((uint8_t)param << 8) | deviceId), value));
    }

    /**
    * @brief Issues OIStepper Get Param command
    * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
    * @param[in] param OIStepper register address
    * @return Register value - 1 to 3 bytes (depends on register)
    */
    uint32_t cmdGetParam(uint8_t deviceId, uint32_t param) const {
        return getMessage(OIMessage(CMD_GET_PARAM, _senderId, (uint16_t)(((uint8_t)param << 8) | deviceId)));
    }

    /**
    * @brief Issues the SetParam command to the OIStepper of the specified device
    * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
    * @param[in] param Register adress (Powerstep01_ABS_POS, Powerstep01_MARK,...)
    * @param[in] value Floating point value to convert and set into the register
    * @return TRUE if param and value are valid, FALSE otherwise
    */
    void setAnalogValue(uint8_t deviceId, uint32_t param, float value) const {
        uint32_t castedValue = reinterpret_cast<uint32_t &>(value);
        setMessage(OIMessage(CMD_SET_ANALOG_VALUE, _senderId, (uint16_t)(((uint8_t)param << 8) | deviceId), castedValue));
    }

    /**
    * @brief Issues OIStepper Get Parameter command and convert the result to
    * floating point if meaningfull
    * @param[in] deviceId (from 0 to MAX_NUMBER_OF_DEVICES-1 )
    * @param[in] param Powerstep01 register address
    * @return Register value - 1 to 3 bytes (depends on register)
    */
    float getAnalogValue(uint8_t deviceId, uint32_t param) const {
        uint32_t value = getMessage(OIMessage(CMD_GET_ANALOG_VALUE, _senderId, (uint16_t)(((uint8_t)param << 8) | deviceId)));
        return reinterpret_cast<float &>(value);
    }
};

#endif /* CONFIG_OI_STEPPER */