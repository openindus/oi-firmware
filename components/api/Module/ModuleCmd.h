/**
 * @file ModuleCmd.h
 * @brief Module commands
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"

enum ModuleCmd_e {
    CMD_NOP                         = 0x00,
    CMD_RESTART                     = 0x01,
    CMD_PING                        = 0x02,
    CMD_LED_CTRL                    = 0x03,
    CMD_DISCOVER                    = 0x04,
    CMD_GET_BOARD_INFO              = 0x05,
    CMD_REQUEST                     = 0x06,
    CMD_EVENT                       = 0x07,
    CMD_FLASH_LOADER_BEGIN          = 0x08,
    CMD_FLASH_LOADER_WRITE          = 0x09,
    CMD_FLASH_LOADER_CHECK          = 0x0A,
    CMD_FLASH_LOADER_END            = 0x0B, 
    CMD_READ_REGISTER               = 0x0C,
    // CMD_WRITE_REGISTER              = 0x0D,
    // CMD_READ_NVS                    = 0x0E,
    // CMD_WRITE_NVS                   = 0x0F,
    // CMD_HEARTBEAT                   = 0x10,
    // CMD_AUTO_TEST                   = 0x11,
};

typedef enum {
    /* DIGITAL */
    REQUEST_DIGITAL_WRITE               = 0x00,
    REQUEST_DIGITAL_READ                = 0x01,
    REQUEST_ATTACH_INTERRUPT            = 0x02,
    REQUEST_DETACH_INTERRUPT            = 0x03,
    REQUEST_TOGGLE_OUTPUT               = 0x04,
    REQUEST_BLINK_OUTPUT                = 0x05,

    /* ANALOG */
    REQUEST_ANALOG_READ                 = 0x20,
    REQUEST_ANALOG_READ_MILLIVOLTS      = 0x21,
    REQUEST_ANALOG_INPUT_MODE           = 0x22,
    REQUEST_ANALOG_INPUT_RESOLUTION     = 0x23,
    REQUEST_ANALOG_INPUT_REFERENCE      = 0x24,
    REQUEST_ANALOG_WRITE                = 0x25,
    REQUEST_ANALOG_OUTPUT_MODE          = 0x26,

    /* MOTOR */
    REQUEST_MOTOR_STOP                  = 0x40,
    REQUEST_MOTOR_MOVE_ABSOLUTE         = 0x41,
    REQUEST_MOTOR_MOVE_RELATIVE         = 0x42,
    REQUEST_MOTOR_RUN                   = 0x43,
    REQUEST_MOTOR_IS_RUNNING            = 0x44,
    REQUEST_MOTOR_HOMING                = 0x45,
    REQUEST_MOTOR_SET_LIMIT_SWITCH      = 0x46,
    REQUEST_MOTOR_SET_STEP_RESOLUTION   = 0x47,
    REQUEST_MOTOR_GET_POSITION          = 0x48,
    REQUEST_MOTOR_GET_SPEED             = 0x49,
    REQUEST_MOTOR_RESET_HOME_POSITION   = 0x4A,
    REQUEST_MOTOR_SET_MAX_SPEED         = 0x4B,
    REQUEST_MOTOR_SET_MIN_SPEED         = 0x4C,
    REQUEST_MOTOR_SET_FULL_STEP_SPEED   = 0x4D,
    REQUEST_MOTOR_SET_ACCELERATION      = 0x4E,
    REQUEST_MOTOR_SET_DECELERATION      = 0x4F,
    REQUEST_MOTOR_SET_SPEED             = 0x50,
    REQUEST_MOTOR_SET_BRAKE             = 0x51,
    REQUEST_MOTOR_SET_DIRECTION         = 0x52,

    /* ENCODER */
    REQUEST_ENCODER_GET_DIRECTION       = 0x81,
    REQUEST_ENCODER_GET_POSITION        = 0x82,
    REQUEST_ENCODER_GET_SPEED           = 0x83,
} ModuleCmd_RequestId_t;

typedef enum {
    EVENT_DIGITAL_INTERRUPT             = 0x00,
} ModuleCmd_EventId_t;

typedef struct {
    union {
        uint8_t byte[8];
        struct {
            struct { 
                uint32_t id : 8;
                uint32_t param : 24;
            };
            uint32_t data;
        };
    };
} ModuleCmd_RequestMsg_t;

typedef std::function<uint32_t(ModuleCmd_RequestMsg_t msg)> ModuleCmd_RequestCallback_t;

typedef std::function<void(uint8_t intr)> ModuleCmd_EventCallback_t;