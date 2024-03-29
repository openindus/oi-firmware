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

typedef enum {
    MODULE_CONTROL              = 0x00,
    MODULE_RESTART              = 0x01,
    MODULE_PING                 = 0x02,
    MODULE_AUTO_ID              = 0x03,
    MODULE_FLASH_LOADER_BEGIN          = 0x04,
    MODULE_FLASH_LOADER_WRITE          = 0x05,
    MODULE_FLASH_LOADER_CHECK          = 0x06,
    MODULE_FLASH_LOADER_END            = 0x07, 
    MODULE_READ_REGISTER        = 0x08,
    MODULE_WRITE_REGISTER       = 0x09,
    MODULE_READ_NVS             = 0x0A,
    MODULE_WRITE_NVS            = 0x0B,
    MODULE_BOARD_TYPE           = 0x0C,
    MODULE_SERIAL_NUMBER        = 0x0D,
    MODULE_HW_VERSION           = 0x0E,
    MODULE_SW_VERSION           = 0x0F,
    MODULE_GET_STATUS           = 0x10,
    MODULE_SET_STATUS           = 0x11,
    MODULE_GET_MODE             = 0x12,
    MODULE_SET_MODE             = 0x13,
    MODULE_REQUEST              = 0x14,
    MODULE_RESPONSE             = 0x15,
    MODULE_EVENT                = 0x16,
    MODULE_BUS_ID               = 0x17,
    MODULE_LED_STATE            = 0x18,
} ModuleCommand_t;

typedef enum {
    /* DIGITAL [0x30-0x3F] */
    CMD_DIGITAL_WRITE                   = 0x30,
    CMD_DIGITAL_READ                    = 0x31,
    CMD_ATTACH_INTERRUPT                = 0x32,
    CMD_DETACH_INTERRUPT                = 0x33,

    /* ANALOG [0x40-0x4F] */
    CMD_ANALOG_READ                     = 0x40,
    CMD_ANALOG_READ_MILLIVOLTS          = 0x41,
    CMD_ANALOG_READ_MODE                = 0x42,
    CMD_ANALOG_READ_RESOLUTION          = 0x43,
    CMD_ANALOG_READ_REFERENCE           = 0x44,
    CMD_ANALOG_WRITE                    = 0x45,
    CMD_ANALOG_WRITE_VOLTAGE            = 0x46,
    CMD_ANALOG_WRITE_VOLTAGE_MILLIVOLTS = 0x47,
    CMD_ANALOG_WRITE_VOLTAGE_MODE       = 0x48,
    CMD_ANALOG_WRITE_CURRENT            = 0x49, 
    CMD_ANALOG_WRITE_CURRENT_MILLIAMPS  = 0x4A,
    CMD_ANALOG_WRITE_CURRENT_MODE       = 0x4B,
    CMD_ANALOG_ADD_VOLTMETER            = 0x4C,
    CMD_ANALOG_GET_CHANNEL_DATA         = 0x4D,
    CMD_ANALOG_ADD_TC                   = 0x4E,
    CMD_ANALOG_ADD_LEVEL_DETECTOR       = 0x4F,


    /* MOTOR [0x50-0x6F] */
    CMD_MOTOR_STOP                      = 0x50,
    CMD_MOTOR_MOVE_ABSOLUTE             = 0x51,
    CMD_MOTOR_MOVE_RELATIVE             = 0x52,
    CMD_MOTOR_RUN                       = 0x53,
    CMD_WAIT_WHILE_MOTOR_IS_RUNNING     = 0x54,
    CMD_MOTOR_HOMING                    = 0x55,

    CMD_MOTOR_SET_LIMIT_SWITCH          = 0x60,
    CMD_MOTOR_SET_STEP_RESOLUTION       = 0x61,
    CMD_MOTOR_SET_SPEED                 = 0x62,
    CMD_MOTOR_GET_POSITION              = 0x63,
    CMD_MOTOR_GET_SPEED                 = 0x64,
    CMD_MOTOR_RESET_HOME_POSITION       = 0x65,
    CMD_MOTOR_SET_BRAKE                 = 0x66,
    CMD_MOTOR_SET_DIRECTION             = 0x67,

    CMD_ENCODER_GET_DIRECTION           = 0x71,
    CMD_ENCODER_GET_POSITION            = 0x72,
    CMD_ENCODER_GET_SPEED               = 0x73,

    /*ANALOG [0x70-0x7F]*/
    CMD_ANALOG_ADD_RTD                  = 0x70,
} RequestCmd_t;

typedef enum {
    DIGITAL_INTERRUPT = 0x00,
} EventType_t;

typedef struct {
    union {
        uint8_t byte[8];
        struct {
            struct { 
                uint32_t cmd : 8;
                uint32_t param : 24;
            };
            uint32_t data;
        };
    };
} RequestMsg_t;

typedef std::function<uint32_t(RequestMsg_t msg)> RequestCallback_t;
typedef std::function<void(uint8_t intr)> EventCallback_t;
