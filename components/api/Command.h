/**
 * @file Command.h
 * @brief Commands
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"

#define CMD_NOP                     (uint8_t) 0x00
#define CMD_RESTART                 (uint8_t) 0x01
#define CMD_PING                    (uint8_t) 0x02
#define CMD_LED_STATUS              (uint8_t) 0x03
#define CMD_DISCOVER                (uint8_t) 0x04
#define CMD_GET_BOARD_INFO          (uint8_t) 0x05
#define CMD_CONTROL                 (uint8_t) 0x06
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

enum Command_Control_e {
    /* DIGITAL */
    CONTROL_DIGITAL_WRITE                   = 0x00,
    CONTROL_DIGITAL_READ                    = 0x01,
    CONTROL_ATTACH_INTERRUPT                = 0x02,
    CONTROL_DETACH_INTERRUPT                = 0x03,
    CONTROL_TOGGLE_OUTPUT                   = 0x04,
    CONTROL_BLINK_OUTPUT                    = 0x05,

    /* ANALOG */
    CONTROL_ANALOG_INPUT_MODE               = 0x20,
    CONTROL_ANALOG_INPUT_VOLTAGE_RANGE      = 0x21,
    CONTROL_ANALOG_READ                     = 0x22,
    CONTROL_ANALOG_READ_VOLT                = 0x23,
    CONTROL_ANALOG_READ_MILLIVOLT           = 0x24,
    CONTROL_ANALOG_READ_AMP                 = 0x25,
    CONTROL_ANALOG_READ_MILLIAMP            = 0x26,

    CONTROL_ANALOG_OUTPUT_MODE              = 0x27,
    CONTROL_ANALOG_WRITE                    = 0x28,

    /* MOTOR */
    CONTROL_MOTOR_STOP                      = 0x40,
    CONTROL_MOTOR_MOVE_ABSOLUTE             = 0x41,
    CONTROL_MOTOR_MOVE_RELATIVE             = 0x42,
    CONTROL_MOTOR_RUN                       = 0x43,
    CONTROL_MOTOR_IS_RUNNING                = 0x44,
    CONTROL_MOTOR_HOMING                    = 0x45,
    CONTROL_MOTOR_SET_LIMIT_SWITCH          = 0x46,
    CONTROL_MOTOR_SET_STEP_RESOLUTION       = 0x47,
    CONTROL_MOTOR_GET_POSITION              = 0x48,
    CONTROL_MOTOR_GET_SPEED                 = 0x49,
    CONTROL_MOTOR_RESET_HOME_POSITION       = 0x4A,
    CONTROL_MOTOR_SET_MAX_SPEED             = 0x4B,
    CONTROL_MOTOR_SET_MIN_SPEED             = 0x4C,
    CONTROL_MOTOR_SET_FULL_STEP_SPEED       = 0x4D,
    CONTROL_MOTOR_SET_ACCELERATION          = 0x4E,
    CONTROL_MOTOR_SET_DECELERATION          = 0x4F,
    CONTROL_MOTOR_SET_SPEED                 = 0x50,
    CONTROL_MOTOR_SET_BRAKE                 = 0x51,
    CONTROL_MOTOR_SET_DIRECTION             = 0x52,

    /* ENCODER */
    CONTROL_ENCODER_GET_DIRECTION           = 0x81,
    CONTROL_ENCODER_GET_POSITION            = 0x82,
    CONTROL_ENCODER_GET_SPEED               = 0x83,
};

enum Command_Event_e {
    EVENT_DIGITAL_INTERRUPT                 = 0x00,
};
