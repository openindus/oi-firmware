/**
 * @file Cmd.h
 * @brief Commands definition
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Common.h"

enum Cmd_Request_e {
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

    /* ANALOGLS */
    REQUEST_ADD_SENSOR                      = 0xB0,
    REQUEST_SENSOR_SET_PARAMETER            = 0xB1,
    REQUEST_SENSOR_READ                     = 0xB2,
    REQUEST_SENSOR_READ_MILLIVOLT           = 0xB3,
    REQUEST_SENSOR_READ_RESISTANCE          = 0xB4,
    REQUEST_SENSOR_READ_TEMPERATURE         = 0xB5,
    REQUEST_SENSOR_READ_RAW                 = 0xB6,
};

enum Cmd_Event_e {
    /* DIGITAL */
    EVENT_DIGITAL_INTERRUPT                 = 0x00,

    /* MOTOR */
    EVENT_MOTOR_READY                       = 0x01,

    /* AnalogLS */
    EVENT_SENSOR_READ                       = 0xB0,
    EVENT_SENSOR_READ_MILLIVOLT             = 0xB1,
    EVENT_SENSOR_READ_RESISTANCE            = 0xB2,
    EVENT_SENSOR_READ_TEMPERATURE           = 0xB3,
    EVENT_SENSOR_READ_RAW                   = 0xB4,
};
