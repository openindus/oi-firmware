/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file OIProtocol.h
 * @brief 
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#define UNDEFINED_ID 0xFE
#define BROADCAST_ID 0xFF

#define TYPE_SET    (uint8_t) (0b0 << 7)
#define TYPE_GET    (uint8_t) (0b10 << 6)
#define TYPE_SEND   (uint8_t) (0b11 << 6)

#define MASK_SET    (uint8_t) (0x80)
#define MASK_GET    (uint8_t) (0xC0)
#define MASK_SEND   (uint8_t) (0xC0)

#define CMD_TYPE_SET(CMD)   (uint8_t) ((CMD & ~MASK_SET) | TYPE_SET)
#define CMD_TYPE_GET(CMD)   (uint8_t) ((CMD & ~MASK_GET) | TYPE_GET)
#define CMD_TYPE_SEND(CMD)  (uint8_t) ((CMD & ~MASK_SEND) | TYPE_SEND)

/*-------------------- MODULE & SYSTEM -----------------------*/
enum {
    /* set */
    CMD_RESTART             = CMD_TYPE_SET(0x7F),
    CMD_LED                 = CMD_TYPE_SET(0x7E),
    CMD_DOWNLOAD_MODE       = CMD_TYPE_SET(0x7D),
    CMD_INTERNAL_TEMP       = CMD_TYPE_SET(0x7C),
    CMD_SET_AUTO_ID         = CMD_TYPE_SET(0x7B),
    /* get */
    CMD_PING                = CMD_TYPE_GET(0x3F),
    CMD_GET_TYPE            = CMD_TYPE_GET(0x3E),
    CMD_GET_ALL_OI_ID       = CMD_TYPE_GET(0x3D),
    /* send */
    CMD_SYSTEM              = CMD_TYPE_SEND(0x3F),
    CMD_GET_TYPE_ALL        = CMD_TYPE_SEND(0x3E),
    CMD_ETOR_INTERRUPT      = CMD_TYPE_SEND(0x3D),
    CMD_SEND_OI_ID          = CMD_TYPE_SEND(0x3C),
};

/*---------------------- DISCRETE ----------------------------*/
enum {
    /* set */
    CMD_DIGITAL_WRITE =             CMD_TYPE_SET(0x00),
    CMD_ANALOG_WRITE =              CMD_TYPE_SET(0x01),
    CMD_ATTACH_INTERRUPT =          CMD_TYPE_SET(0x02),
    CMD_DETACH_INTERRUPT =          CMD_TYPE_SET(0x03),
    /* get */
    CMD_DIGITAL_READ =              CMD_TYPE_GET(0x00),
    CMD_ANALOG_READ =               CMD_TYPE_GET(0x01),
    CMD_ANALOG_READ_MILLIVOLTS =    CMD_TYPE_GET(0x02),
    CMD_STOR_SENSOR_READ =          CMD_TYPE_GET(0x03),
};

/*---------------------- MIXED ----------------------------*/
enum {
    /* set */
    CMD_VOLTAGE_WRITE_DAC =         CMD_TYPE_SET(0x04),
    CMD_SET_ANALOG_ADC_MODE =       CMD_TYPE_SET(0x05),
    CMD_SET_ANALOG_ADC_RESOLUTION = CMD_TYPE_SET(0x06),
    CMD_SET_ANALOG_DAC_MODE =       CMD_TYPE_SET(0x07),
    CMD_SET_CURRENT_DAC_MODE =      CMD_TYPE_SET(0x08),
    CMD_CURRENT_WRITE_DAC =         CMD_TYPE_SET(0x09),
    /* get */
    CMD_ANALOG_READ_ADC =           CMD_TYPE_GET(0x04),
    CMD_GET_ANALOG_ADC_REFERENCE =  CMD_TYPE_GET(0x05),
    CMD_GET_ANALOG_ADC_MODE =       CMD_TYPE_GET(0x06),
    CMD_GET_ANALOG_ADC_RESOLUTION = CMD_TYPE_GET(0x07),
    CMD_GET_ANALOG_DAC_MODE =       CMD_TYPE_GET(0x08),
    CMD_GET_CURRENT_DAC_MODE =      CMD_TYPE_GET(0x09),
};

/*------------------------ STEPPER ---------------------------*/
enum {
    /* set */
    CMD_ATTACH_BUSY_INTERRUPT =         CMD_TYPE_SET(0x00),
    CMD_ATTACH_ERROR_HANDLER =          CMD_TYPE_SET(0x01),
    CMD_ATTACH_FLAG_INTERRUPT =         CMD_TYPE_SET(0x02),
    CMD_GO_HOME =                       CMD_TYPE_SET(0x03),
    CMD_GO_MARK =                       CMD_TYPE_SET(0x04),
    CMD_GO_TO =                         CMD_TYPE_SET(0x05),
    CMD_GO_TO_DIR =                     CMD_TYPE_SET(0x06),
    CMD_GO_UNTIL =                      CMD_TYPE_SET(0x07),
    CMD_HARD_HIZ =                      CMD_TYPE_SET(0x08),
    CMD_HARD_STOP =                     CMD_TYPE_SET(0x09),
    CMD_MOVE =                          CMD_TYPE_SET(0x0A),
    CMD_SET_ANALOG_VALUE =              CMD_TYPE_SET(0x0B),
    CMD_RELEASE_SW =                    CMD_TYPE_SET(0x0C),
    CMD_RESET_DEVICE =                  CMD_TYPE_SET(0x0D),
    CMD_RESET_POS =                     CMD_TYPE_SET(0x0E),
    CMD_RUN =                           CMD_TYPE_SET(0x0F),
    CMD_SOFT_HIZ =                      CMD_TYPE_SET(0x10),
    CMD_SOFT_STOP =                     CMD_TYPE_SET(0x11),
    CMD_STEP_CLOCK =                    CMD_TYPE_SET(0x12),
    CMD_FETCH_AND_CLEAR_ALL_STATUS =    CMD_TYPE_SET(0x13),
    CMD_QUEUE_COMMANDS =                CMD_TYPE_SET(0x14),
    CMD_RELEASE_RESET =                 CMD_TYPE_SET(0x15),
    CMD_RESET =                         CMD_TYPE_SET(0x16),
    CMD_SET_HOME =                      CMD_TYPE_SET(0x17),
    CMD_SET_MARK =                      CMD_TYPE_SET(0x18),
    CMD_START_STEP_CLOCK =              CMD_TYPE_SET(0x19),
    CMD_STOP_STEP_CLOCK =               CMD_TYPE_SET(0x1A),
    CMD_SET_PARAM =                     CMD_TYPE_SET(0x1B),
    CMD_ATTACH_LIMIT_SWITCH =           CMD_TYPE_SET(0x1C),
    CMD_DETACH_LIMIT_SWITCH =           CMD_TYPE_SET(0x1D),
    
    /* get */
    CMD_CHECK_BUSY_HW =                 CMD_TYPE_GET(0x00),    
    CMD_CHECK_STATUS_HW =               CMD_TYPE_GET(0x01),
    CMD_GET_STATUS =                    CMD_TYPE_GET(0x02),
    CMD_GET_FETCHED_STATUS =            CMD_TYPE_GET(0x03),
    CMD_GET_MARK =                      CMD_TYPE_GET(0x04),
    CMD_GET_POSITION =                  CMD_TYPE_GET(0x05),
    CMD_IS_DEVICE_BUSY =                CMD_TYPE_GET(0x06),
    CMD_READ_STATUS_REGISTER =          CMD_TYPE_GET(0x07),
    CMD_SELECT_STEP_MODE =              CMD_TYPE_GET(0x08),
    CMD_GET_PARAM =                     CMD_TYPE_GET(0x09),
    CMD_GET_ANALOG_VALUE =              CMD_TYPE_GET(0x0A),
    CMD_GET_ETOR_LEVEL =                CMD_TYPE_GET(0x0B),
    
    /* send */
    CMD_BUSY_INTERRUPT =                CMD_TYPE_SEND(0x00),
    CMD_ERROR_HANDLER =                 CMD_TYPE_SEND(0x01),
    CMD_FLAG_INTERRUPT =                CMD_TYPE_SEND(0x02),
    CMD_SEND_QUEUED_COMMANDS =          CMD_TYPE_SEND(0x03),
};

/*------------------------ RELAY ---------------------------*/
enum
{
    CMD_SET_RELAY =                    CMD_TYPE_SET(0x00),
};