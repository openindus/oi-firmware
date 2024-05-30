/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file Module.h
 * @brief Functions for module
 *
 * For more information on OpenIndus:
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

typedef enum {
    STATE_IDLE      = (int) 0,
    STATE_RUNNING   = (int) 1,
    STATE_ERROR     = (int) 2
} Module_State_t;

enum ModuleType_List_e {
    TYPE_OI_CORE            = (uint16_t) 3,
    TYPE_OI_CORELITE        = (uint16_t) 4,
    TYPE_OI_DISCRETE        = (uint16_t) 6,
    TYPE_OI_DISCRETE_VE     = (uint16_t) 7,
    TYPE_OI_MIXED           = (uint16_t) 8,
    TYPE_OI_RELAY_LP        = (uint16_t) 9,
    TYPE_OI_RELAY_HP        = (uint16_t) 10,
    TYPE_OI_STEPPER         = (uint16_t) 11,
    TYPE_OI_STEPPER_VE      = (uint16_t) 12,
    TYPE_OI_ANALOG_LS       = (uint16_t) 13,
    TYPE_OI_BRUSHLESS       = (uint16_t) 18,
};

class ModuleUtils
{
public:

    static inline char* typeToName(uint16_t type, char* name) {
        switch (type) {
            case TYPE_OI_CORE:          strcpy(name, "OICore"); break;
            case TYPE_OI_CORELITE:      strcpy(name, "OICoreLite"); break;
            case TYPE_OI_DISCRETE:      strcpy(name, "OIDiscrete"); break;
            case TYPE_OI_DISCRETE_VE:   strcpy(name, "OIDiscreteVE"); break;
            case TYPE_OI_MIXED:         strcpy(name, "OIMixed");    break;
            case TYPE_OI_RELAY_LP:      strcpy(name, "OIRelayLP"); break;
            case TYPE_OI_RELAY_HP:      strcpy(name, "OIRelayHP"); break;
            case TYPE_OI_STEPPER:       strcpy(name, "OIStepper"); break;
            case TYPE_OI_STEPPER_VE:    strcpy(name, "OIStepperVE"); break;
            case TYPE_OI_ANALOG_LS:     strcpy(name, "OIAnalogLS"); break;
            case TYPE_OI_BRUSHLESS:     strcpy(name, "OIBrushless"); break;
            default:                    strcpy(name, "Unknow"); break;
        }
        return name;
    }
};

enum ModuleCmd_Control_e {
    /* DIGITAL */
    CONTROL_DIGITAL_WRITE                   = 0x00,
    CONTROL_DIGITAL_TOGGLE                  = 0x01,
    CONTROL_DIGITAL_MODE_PWM                = 0x02,
    CONTROL_DIGITAL_SET_PWM                 = 0x03,
    CONTROL_DIGITAL_GET_CURRENT             = 0x04,
    CONTROL_DIGITAL_GET_OVERCURRENT         = 0x05,
    CONTROL_DIGITAL_READ                    = 0x06,
    CONTROL_ATTACH_INTERRUPT                = 0x07,
    CONTROL_DETACH_INTERRUPT                = 0x08,

    /* ANALOG */
    CONTROL_ANALOG_INPUT_MODE               = 0x20,
    CONTROL_ANALOG_INPUT_GET_MODE           = 0x21,
    CONTROL_ANALOG_INPUT_VOLTAGE_RANGE      = 0x22,
    CONTROL_ANALOG_INPUT_GET_VOLTAGE_RANGE  = 0x23,
    CONTROL_ANALOG_READ                     = 0x24,
    CONTROL_ANALOG_READ_VOLT                = 0x25,
    CONTROL_ANALOG_READ_MILLIVOLT           = 0x26,
    CONTROL_ANALOG_READ_AMP                 = 0x27,
    CONTROL_ANALOG_READ_MILLIAMP            = 0x28,
    CONTROL_ANALOG_OUTPUT_MODE              = 0x29,
    CONTROL_ANALOG_WRITE                    = 0x2A,

    /* STEPPER MOTOR */
    CONTROL_MOTOR_STOP                      = 0x40,
    CONTROL_MOTOR_MOVE_ABSOLUTE             = 0x41,
    CONTROL_MOTOR_MOVE_RELATIVE             = 0x42,
    CONTROL_MOTOR_RUN                       = 0x43,
    CONTROL_MOTOR_WAIT                      = 0x44,
    CONTROL_MOTOR_HOMING                    = 0x45,
    CONTROL_MOTOR_ATTACH_LIMIT_SWITCH       = 0x46,
    CONTROL_MOTOR_DETACH_LIMIT_SWITCH       = 0x47,
    CONTROL_MOTOR_SET_STEP_RESOLUTION       = 0x48,
    CONTROL_MOTOR_GET_POSITION              = 0x49,
    CONTROL_MOTOR_GET_SPEED                 = 0x4A,
    CONTROL_MOTOR_RESET_HOME_POSITION       = 0x4B,
    CONTROL_MOTOR_SET_MAX_SPEED             = 0x4C,
    CONTROL_MOTOR_SET_MIN_SPEED             = 0x4D,
    CONTROL_MOTOR_SET_FULL_STEP_SPEED       = 0x4E,
    CONTROL_MOTOR_SET_ACCELERATION          = 0x4F,
    CONTROL_MOTOR_SET_DECELERATION          = 0x50,

    /* BRUSHLESS MOTOR */
    CONTROL_MOTOR_SET_SPEED                 = 0x60,
    CONTROL_MOTOR_SET_BRAKE                 = 0x61,
    CONTROL_MOTOR_SET_DIRECTION             = 0x62,

    /* ENCODER */
    CONTROL_ENCODER_GET_DIRECTION           = 0x81,
    CONTROL_ENCODER_GET_POSITION            = 0x82,
    CONTROL_ENCODER_GET_SPEED               = 0x83,
};

enum ModuleCmd_Event_e {
    EVENT_DIGITAL_INTERRUPT                 = 0x00,
    EVENT_MOTOR_READY                       = 0x01,
};