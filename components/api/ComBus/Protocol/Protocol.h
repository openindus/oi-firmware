/**
 * @file Protocol.h
 * @brief Protocol interface
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"

/* Protocol cmd */
#define CMD_NOP                     (uint8_t) 0x00
#define CMD_RESTART                 (uint8_t) 0x01
#define CMD_PING                    (uint8_t) 0x02
#define CMD_LED_STATUS              (uint8_t) 0x03
#define CMD_DISCOVER                (uint8_t) 0x04
#define CMD_GET_BOARD_INFO          (uint8_t) 0x05
#define CMD_REQUEST                 (uint8_t) 0x06
#define CMD_EVENT                   (uint8_t) 0x07
#define CMD_FLASH_LOADER_BEGIN      (uint8_t) 0x08
#define CMD_FLASH_LOADER_WRITE      (uint8_t) 0x09
#define CMD_FLASH_LOADER_CHECK      (uint8_t) 0x0A
#define CMD_FLASH_LOADER_END        (uint8_t) 0x0B 
#define CMD_READ_REGISTER           (uint8_t) 0x0C
// #define CMD_WRITE_REGISTER          (uint8_t) 0x0D
// #define CMD_READ_NVS                (uint8_t) 0x0E
// #define CMD_WRITE_NVS               (uint8_t) 0x0F
// #define CMD_HEARTBEAT               (uint8_t) 0x10
// #define CMD_AUTO_TEST               (uint8_t) 0x11
#define CMD_OI_GPIO                 (uint8_t) 0x12

enum Protocol_Request_e {
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

    /* BRUSHLESS MOTOR */
    REQUEST_MOTOR_SET_SPEED                 = 0x60,
    REQUEST_MOTOR_SET_BRAKE                 = 0x61,
    REQUEST_MOTOR_SET_DIRECTION             = 0x62,

    /* ENCODER */
    REQUEST_ENCODER_GET_DIRECTION           = 0x81,
    REQUEST_ENCODER_GET_POSITION            = 0x82,
    REQUEST_ENCODER_GET_SPEED               = 0x83,
};

enum Protocol_Event_e {
    EVENT_DIGITAL_INTERRUPT                 = 0x00,
    EVENT_MOTOR_READY                       = 0x01,
};

enum Protocol_OIGpio
{
    SET_LOW = 0,
    SET_HIGH = 1,
    TOGGLE = 2,
    READ = 3
};