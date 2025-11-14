/**
 * @file Types.h
 * @brief OpenIndus Types
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Common.h"

/**
 * @brief Bus command enumeration
 * 
 */
enum BusCmd_e {
    CMD_NOP                     = (uint8_t) 0x00,
    CMD_RESTART                 = (uint8_t) 0x01,
    CMD_PING                    = (uint8_t) 0x02,
    CMD_LED_CTRL                = (uint8_t) 0x03,
    CMD_DISCOVER_SLAVES         = (uint8_t) 0x04,
    CMD_GET_BOARD_INFO          = (uint8_t) 0x05,
    CMD_RUN_CALLBACK            = (uint8_t) 0x06,
    CMD_SEND_EVENT              = (uint8_t) 0x07,
    CMD_FLASH_LOADER_BEGIN      = (uint8_t) 0x08,
    CMD_FLASH_LOADER_WRITE      = (uint8_t) 0x09,
    CMD_FLASH_LOADER_CHECK      = (uint8_t) 0x0A,
    CMD_FLASH_LOADER_END        = (uint8_t) 0x0B,
    CMD_READ_REGISTER           = (uint8_t) 0x0C,
    CMD_WRITE_REGISTER          = (uint8_t) 0x0D, // Unused
    CMD_READ_NVS                = (uint8_t) 0x0E, // Unused
    CMD_WRITE_NVS               = (uint8_t) 0x0F, // Unused
    CMD_HEARTBEAT               = (uint8_t) 0x10,
    CMD_RESET                   = (uint8_t) 0x11, // Reset Module
    CMD_SEND_ERROR              = (uint8_t) 0x12,
    CMD_REGISTER_EVENT_CALLBACK = (uint8_t) 0x13,
    CMD_GET_SLAVE_ID            = (uint8_t) 0x14,
};

/**
 * @brief Callback enumeration
 * 
 */
enum Callback_e {
    /* DIGITAL */
    CALLBACK_DIGITAL_WRITE                  = 0x00,
    CALLBACK_TOGGLE_OUTPUT                  = 0x01,
    CALLBACK_OUTPUT_MODE                    = 0x02,
    CALLBACK_SET_PWM_FREQUENCY              = 0x03,
    CALLBACK_SET_PWM_DUTY_CYCLE             = 0x04,
    CALLBACK_GET_OUTPUT_CURRENT             = 0x05,
    CALLBACK_OUTPUT_IS_OVERCURRENT          = 0x06,
    CALLBACK_DIGITAL_READ                   = 0x07,
    CALLBACK_ATTACH_INTERRUPT               = 0x08,
    CALLBACK_DETACH_INTERRUPT               = 0x09,
    CALLBACK_SET_OVERCURRENT_THRESHOLD      = 0x0A,
    CALLBACK_ATTACH_OVERCURRENT_CALLBACK    = 0x0B,
    CALLBACK_DETACH_OVERCURRENT_CALLBACK    = 0x0C,

    /* ANALOG */
    CALLBACK_ANALOG_INPUT_MODE              = 0x20,
    CALLBACK_ANALOG_INPUT_GET_MODE          = 0x21,
    CALLBACK_ANALOG_INPUT_VOLTAGE_RANGE     = 0x22,
    CALLBACK_ANALOG_INPUT_GET_VOLTAGE_RANGE = 0x23,
    CALLBACK_ANALOG_READ                    = 0x24,
    CALLBACK_ANALOG_READ_VOLT               = 0x25,
    CALLBACK_ANALOG_READ_MILLIVOLT          = 0x26,
    CALLBACK_ANALOG_READ_AMP                = 0x27,
    CALLBACK_ANALOG_READ_MILLIAMP           = 0x28,
    CALLBACK_ANALOG_OUTPUT_MODE             = 0x29,
    CALLBACK_ANALOG_WRITE                   = 0x2A,

    /* STEPPER MOTOR */
    CALLBACK_MOTOR_STOP                     = 0x40,
    CALLBACK_MOTOR_MOVE_ABSOLUTE            = 0x41,
    CALLBACK_MOTOR_MOVE_RELATIVE            = 0x42,
    CALLBACK_MOTOR_RUN                      = 0x43,
    CALLBACK_MOTOR_WAIT                     = 0x44,
    CALLBACK_MOTOR_HOMING                   = 0x45,
    CALLBACK_MOTOR_ATTACH_LIMIT_SWITCH      = 0x46,
    CALLBACK_MOTOR_DETACH_LIMIT_SWITCH      = 0x47,
    CALLBACK_MOTOR_SET_STEP_RESOLUTION      = 0x48,
    CALLBACK_MOTOR_GET_POSITION             = 0x49,
    CALLBACK_MOTOR_GET_SPEED                = 0x4A,
    CALLBACK_MOTOR_RESET_HOME_POSITION      = 0x4B,
    CALLBACK_MOTOR_SET_MAX_SPEED            = 0x4C,
    CALLBACK_MOTOR_SET_MIN_SPEED            = 0x4D,
    CALLBACK_MOTOR_SET_FULL_STEP_SPEED      = 0x4E,
    CALLBACK_MOTOR_SET_ACCELERATION         = 0x4F,
    CALLBACK_MOTOR_SET_DECELERATION         = 0x50,
    CALLBACK_MOTOR_SET_POSITION             = 0x51,
    CALLBACK_MOTOR_SET_ADVANCED_PARAM       = 0x52,
    CALLBACK_MOTOR_GET_ADVANCED_PARAM       = 0x53,
    CALLBACK_MOTOR_GET_SUPPLY_VOLTAGE       = 0x54,
    CALLBACK_MOTOR_ATTACH_FLAG_INTERRUPT    = 0x55,
    CALLBACK_MOTOR_DETACH_FLAG_INTERRUPT    = 0x56,
    CALLBACK_MOTOR_GET_STATUS               = 0x57,
    CALLBACK_MOTOR_CLEAR_STATUS             = 0x58,
    CALLBACK_MOTOR_RESET_ALL_ADVANCED_PARAM = 0x59,

    /* BRUSHLESS MOTOR */
    CALLBACK_MOTOR_SET_SPEED                = 0x60,
    CALLBACK_MOTOR_SET_BRAKE                = 0x61,
    CALLBACK_MOTOR_SET_DIRECTION            = 0x62,

    /* ENCODER */
    CALLBACK_ENCODER_BEGIN                  = 0x80,
    CALLBACK_ENCODER_END                    = 0x81,
    CALLBACK_ENCODER_RESET                  = 0x82,
    CALLBACK_ENCODER_GET_REVOLUTIONS        = 0x83,
    CALLBACK_ENCODER_GET_PULSES             = 0x84,
    CALLBACK_ENCODER_GET_ANGLE              = 0x85,
    CALLBACK_ENCODER_GET_SPEED              = 0x86,

    /* DC MOTOR */
    CALLBACK_MOTOR_DC_RUN                   = 0xA0,
    CALLBACK_MOTOR_DC_STOP                  = 0xA1,
    CALLBACK_MOTOR_DC_GET_CURRENT           = 0xA2,

    /* SENSOR */
    CALLBACK_ADD_SENSOR                     = 0xB0,
    CALLBACK_SENSOR_SET_PARAMETER           = 0xB1,
    CALLBACK_SENSOR_READ                    = 0xB2,
    CALLBACK_SENSOR_READ_MILLIVOLT          = 0xB3,
    CALLBACK_SENSOR_READ_RESISTANCE         = 0xB4,
    CALLBACK_SENSOR_READ_TEMPERATURE        = 0xB5,
    CALLBACK_SENSOR_READ_RAW                = 0xB6,
};

/**
 * @brief Event enumeration
 * 
 */
enum Event_e {
    /* DIGITAL */
    EVENT_DIGITAL_INTERRUPT                 = 0x00,
    EVENT_OVERCURRENT                       = 0x01,

    /* MOTOR */
    EVENT_MOTOR_READY                       = 0x01,
    EVENT_MOTOR_FLAG_INTERRUPT              = 0x02,

    /* SENSOR */
    EVENT_SENSOR_VALUE                      = 0xB0,
    EVENT_SENSOR_VALUE_MILLIVOLT            = 0xB1,
    EVENT_SENSOR_VALUE_RESISTANCE           = 0xB2,
    EVENT_SENSOR_VALUE_TEMPERATURE          = 0xB3,
    EVENT_SENSOR_VALUE_RAW                  = 0xB4,
};

/**
 * @brief Error code enumeration
 * 
 */
enum ErrorCode_e {
    ERROR_NONE          = (int) 0,
    ERROR_HARDWARE_INIT = (int) -1,
    ERROR_AUTO_ID       = (int) -2,
    ERROR_BOOT          = (int) -3,
};

/**
 * @brief Event callback enumeration
 * 
 */
enum EventCallback_e {
    EVENT_CALLBACK_NONE                 = 0x00,
    EVENT_CALLBACK_DIGITAL_WRITE        = 0x01,
    EVENT_CALLBACK_TOGGLE_OUTPUT        = 0x02,
    EVENT_CALLBACK_MOTOR_STOP           = 0x03,
    EVENT_CALLBACK_TRIGGER_LIMIT_SWITCH = 0x04,
};

/**
 * @brief State enumeration
 * 
 */
enum State_e {
    STATE_UNDEFINED = (int) -1,
    STATE_IDLE      = (int) 0,
    STATE_RUNNING   = (int) 1,
    STATE_ERROR     = (int) 2
};

/**
 * @brief Event message structure
 * 
 */
struct EventMsg_s {
    uint8_t id;
    std::vector<uint8_t> args;
};

/**
 * @brief Event callback configuration structure
 * 
 */
struct EventCallbackConfig_s {
    uint16_t moduleId;
    uint8_t eventId;
    uint8_t eventArg;
    uint8_t callbackId;
    std::vector<uint8_t> callbackArgs;
};