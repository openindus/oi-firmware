/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file Command.h
 * @brief Functions for module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"

enum Module_Command_e {
    MODULE_RESTART                  = 0x01,
    MODULE_PING                     = 0x02,
    MODULE_AUTO_ID                  = 0x03,
    MODULE_FLASH_LOADER_BEGIN       = 0x04,
    MODULE_FLASH_LOADER_WRITE       = 0x05,
    MODULE_FLASH_LOADER_CHECK       = 0x06,
    MODULE_FLASH_LOADER_END         = 0x07, 
    MODULE_READ_REGISTER            = 0x08,
    MODULE_WRITE_REGISTER           = 0x09,
    MODULE_READ_NVS                 = 0x0A,
    MODULE_WRITE_NVS                = 0x0B,
    MODULE_BOARD_TYPE               = 0x0C,
    MODULE_SERIAL_NUMBER            = 0x0D,
    MODULE_HW_VERSION               = 0x0E,
    MODULE_SW_VERSION               = 0x0F,
    MODULE_GET_STATUS               = 0x10,
    MODULE_SET_STATUS               = 0x11,
    MODULE_GET_MODE                 = 0x12,
    MODULE_SET_MODE                 = 0x13,
    MODULE_REQUEST                  = 0x14,
    MODULE_RESPONSE                 = 0x15,
    MODULE_EVENT                    = 0x16,
    MODULE_BUS_ID                   = 0x17,
    MODULE_LED_STATE                = 0x18,
};

typedef enum {
    /* DIGITAL [0x30-0x3F] */
    DIGITAL_WRITE                   = 0x30,
    DIGITAL_READ                    = 0x31,
    ATTACH_INTERRUPT                = 0x32,
    DETACH_INTERRUPT                = 0x33,
    DIGITAL_TOGGLE                  = 0x34,

    /* ANALOG [0x40-0x4F] */
    ANALOG_READ                     = 0x40,
    ANALOG_READ_MILLIVOLTS          = 0x41,
    ANALOG_READ_MODE                = 0x42,
    ANALOG_READ_RESOLUTION          = 0x43,
    ANALOG_READ_REFERENCE           = 0x44,
    ANALOG_OUTPUT_MODE              = 0x45,
    ANALOG_WRITE                    = 0x46,
    /** @todo: to modify */
    ANALOG_ADD_VOLTMETER            = 0x4C,
    ANALOG_GET_CHANNEL_DATA         = 0x4D,
    ANALOG_ADD_TC                   = 0x4E,
    ANALOG_ADD_LEVEL_DETECTOR       = 0x4F,
    ANALOG_ADD_RTD                  = 0x50,

    /* MOTOR [0x60-0x7F] */
    MOTOR_STOP                      = 0x60,
    MOTOR_MOVE_ABSOLUTE             = 0x61,
    MOTOR_MOVE_RELATIVE             = 0x62,
    MOTOR_RUN                       = 0x63,
    MOTOR_IS_RUNNING                = 0x64,
    MOTOR_HOMING                    = 0x65,
    MOTOR_SET_LIMIT_SWITCH          = 0x66,
    MOTOR_SET_STEP_RESOLUTION       = 0x67,
    MOTOR_SET_SPEED                 = 0x68,
    MOTOR_GET_POSITION              = 0x69,
    MOTOR_GET_SPEED                 = 0x6A,
    MOTOR_RESET_HOME_POSITION       = 0x6B,
    MOTOR_SET_BRAKE                 = 0x6C,
    MOTOR_SET_DIRECTION             = 0x6E,
    MOTOR_SET_MAX_SPEED             = 0x6F,
    MOTOR_SET_MIN_SPEED             = 0x70,
    MOTOR_SET_FULL_STEP_SPEED       = 0x71,
    MOTOR_SET_ACCELERATION          = 0x72,
    MOTOR_SET_DECELERATION          = 0x73,

    /* ENCODER [0x80-0x8F] */
    ENCODER_GET_DIRECTION           = 0x81,
    ENCODER_GET_POSITION            = 0x82,
    ENCODER_GET_SPEED               = 0x83,
    
} Module_Request_t;

typedef enum {
    DIGITAL_INTERRUPT               = 0x00,
} Module_Event_t;

typedef struct {
    union {
        uint8_t byte[8];
        struct {
            struct { 
                uint32_t request : 8;
                uint32_t param : 24;
            };
            uint32_t data;
        };
    };
} Module_RequestMsg_t;

typedef std::function<uint32_t(Module_RequestMsg_t msg)> Module_RequestCallback_t;
typedef std::function<void(uint8_t intr)> Module_EventCallback_t;