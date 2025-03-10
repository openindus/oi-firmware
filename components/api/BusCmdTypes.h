/**
 * @file BusCmdTypes.h
 * @brief BusCmdTypes definition file for the API
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Common.h"

/**
 * @brief Bus Command Type Enumeration
 * 
 */
enum Bus_Cmd_e {
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
    CMD_RESET               = (uint8_t) 0x11 // Reset Module
};