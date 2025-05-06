/**
 * @file Types.h
 * @brief Types definition file for the API
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Common.h"

/**
 * @brief Command enumeration
 * 
 */
enum Cmd_e {
    CMD_NOP                 = (uint8_t) 0x00,
    CMD_RESTART             = (uint8_t) 0x01,
    CMD_PING                = (uint8_t) 0x02,
    CMD_SET_LED             = (uint8_t) 0x03,
    CMD_DISCOVER            = (uint8_t) 0x04,
    CMD_GET_BOARD_INFO      = (uint8_t) 0x05,
    CMD_CONTROLLER_REQUEST  = (uint8_t) 0x06,
    CMD_CONTROLLER_EVENT    = (uint8_t) 0x07,
    CMD_FLASH_LOADER_BEGIN  = (uint8_t) 0x08,
    CMD_FLASH_LOADER_WRITE  = (uint8_t) 0x09,
    CMD_FLASH_LOADER_CHECK  = (uint8_t) 0x0A,
    CMD_FLASH_LOADER_END    = (uint8_t) 0x0B,
    CMD_READ_REGISTER       = (uint8_t) 0x0C,
    // CMD_WRITE_REGISTER      = (uint8_t) 0x0D,
    // CMD_READ_NVS            = (uint8_t) 0x0E,
    // CMD_WRITE_NVS           = (uint8_t) 0x0F,
    CMD_HEARTBEAT           = (uint8_t) 0x10,
    CMD_RESET               = (uint8_t) 0x11, // Reset Module
    CMD_ERROR               = (uint8_t) 0x12,
};

/**
 * @brief Command controller request enumeration
 * 
 */
enum CmdControllerRequest_e {
    /* DIGITAL */
    REQUEST_DIGITAL_WRITE                   = 0x00,
    REQUEST_TOGGLE_OUTPUT                   = 0x01,
    REQUEST_OUTPUT_MODE                     = 0x02,
    REQUEST_SET_PWM_FREQUENCY               = 0x03,
    REQUEST_SET_PWM_DUTY_CYCLE              = 0x04,
    REQUEST_GET_OUTPUT_CURRENT              = 0x05,
    REQUEST_OUTPUT_IS_OVERCURRENT           = 0x06,
    REQUEST_DIGITAL_READ                    = 0x07,
    REQUEST_ATTACH_INTERRUPT                = 0x08,
    REQUEST_DETACH_INTERRUPT                = 0x09,
    REQUEST_SET_OVERCURRENT_THRESHOLD       = 0x0A,
    REQUEST_ATTACH_OVERCURRENT_CALLBACK     = 0x0B,
    REQUEST_DETACH_OVERCURRENT_CALLBACK     = 0x0C,

    /* ANALOG */
    REQUEST_ANALOG_INPUT_MODE               = 0x20,
    REQUEST_ANALOG_INPUT_GET_MODE           = 0x21,
    REQUEST_ANALOG_INPUT_VOLTAGE_RANGE      = 0x22,
    REQUEST_ANALOG_INPUT_GET_VOLTAGE_RANGE  = 0x23,
    REQUEST_ANALOG_READ                     = 0x24,
    REQUEST_ANALOG_READ_VOLT                = 0x25,
    REQUEST_ANALOG_READ_MILLIVOLT           = 0x26,
    REQUEST_ANALOG_READ_AMP                 = 0x27,
    REQUEST_ANALOG_READ_MILLIAMP            = 0x28,
    REQUEST_ANALOG_OUTPUT_MODE              = 0x29,
    REQUEST_ANALOG_WRITE                    = 0x2A,

    /* STEPPER MOTOR */
    REQUEST_MOTOR_STOP                      = 0x40,
    REQUEST_MOTOR_MOVE_ABSOLUTE             = 0x41,
    REQUEST_MOTOR_MOVE_RELATIVE             = 0x42,
    REQUEST_MOTOR_RUN                       = 0x43,
    REQUEST_MOTOR_WAIT                      = 0x44,
    REQUEST_MOTOR_HOMING                    = 0x45,
    REQUEST_MOTOR_ATTACH_LIMIT_SWITCH       = 0x46,
    REQUEST_MOTOR_DETACH_LIMIT_SWITCH       = 0x47,
    REQUEST_MOTOR_SET_STEP_RESOLUTION       = 0x48,
    REQUEST_MOTOR_GET_POSITION              = 0x49,
    REQUEST_MOTOR_GET_SPEED                 = 0x4A,
    REQUEST_MOTOR_RESET_HOME_POSITION       = 0x4B,
    REQUEST_MOTOR_SET_MAX_SPEED             = 0x4C,
    REQUEST_MOTOR_SET_MIN_SPEED             = 0x4D,
    REQUEST_MOTOR_SET_FULL_STEP_SPEED       = 0x4E,
    REQUEST_MOTOR_SET_ACCELERATION          = 0x4F,
    REQUEST_MOTOR_SET_DECELERATION          = 0x50,
    REQUEST_MOTOR_SET_POSITION              = 0x51,
    REQUEST_MOTOR_SET_ADVANCED_PARAM        = 0x52,
    REQUEST_MOTOR_GET_ADVANCED_PARAM        = 0x53,
    REQUEST_MOTOR_GET_SUPPLY_VOLTAGE        = 0x54,
    REQUEST_MOTOR_ATTACH_FLAG_INTERRUPT     = 0x55,
    REQUEST_MOTOR_DETACH_FLAG_INTERRUPT     = 0x56,

    /* BRUSHLESS MOTOR */
    REQUEST_MOTOR_SET_SPEED                 = 0x60,
    REQUEST_MOTOR_SET_BRAKE                 = 0x61,
    REQUEST_MOTOR_SET_DIRECTION             = 0x62,

    /* ENCODER */
    REQUEST_ENCODER_BEGIN                   = 0x80,
    REQUEST_ENCODER_END                     = 0x81,
    REQUEST_ENCODER_RESET                   = 0x82,
    REQUEST_ENCODER_GET_REVOLUTIONS         = 0x83,
    REQUEST_ENCODER_GET_PULSES              = 0x84,
    REQUEST_ENCODER_GET_ANGLE               = 0x85,
    REQUEST_ENCODER_GET_SPEED               = 0x86,

    /* DC MOTOR */
    REQUEST_MOTOR_DC_RUN                    = 0xA0,
    REQUEST_MOTOR_DC_STOP                   = 0xA1,

    /* SENSOR */
    REQUEST_ADD_SENSOR                      = 0xB0,
    REQUEST_SENSOR_SET_PARAMETER            = 0xB1,
    REQUEST_SENSOR_READ                     = 0xB2,
    REQUEST_SENSOR_READ_MILLIVOLT           = 0xB3,
    REQUEST_SENSOR_READ_RESISTANCE          = 0xB4,
    REQUEST_SENSOR_READ_TEMPERATURE         = 0xB5,
    REQUEST_SENSOR_READ_RAW                 = 0xB6,
};

/**
 * @brief Command controller event enumeration
 * 
 */
enum CmdControllerEvent_e {
    /* DIGITAL */
    EVENT_DIGITAL_INTERRUPT                 = 0x00,
    EVENT_OVERCURRENT                       = 0x01,

    /* MOTOR */
    EVENT_MOTOR_READY                       = 0x01,
    EVENT_MOTOR_FLAG_INTERRUPT              = 0x02,

    /* SENSOR */
    EVENT_SENSOR_READ                       = 0xB0,
    EVENT_SENSOR_READ_MILLIVOLT             = 0xB1,
    EVENT_SENSOR_READ_RESISTANCE            = 0xB2,
    EVENT_SENSOR_READ_TEMPERATURE           = 0xB3,
    EVENT_SENSOR_READ_RAW                   = 0xB4,
};


/**
 * @brief Error code enumeration
 * 
 */
enum ErrorCode_e {
    ERROR_NONE          = (int) 0,
    ERROR_BOOT          = (int) 1,
    ERROR_MODULE_INIT   = (int) 2,
    ERROR_AUTO_ID       = (int) 3,
};