/**
 * @file Bus.h
 * @brief Bus
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "BusIO.h"
#include "BusCan.h"
#include "BusRs.h"

enum Bus_Cmd_e {
    CMD_NOP                         = 0x00,
    CMD_RESTART                     = 0x01,
    CMD_PING                        = 0x02,
    CMD_LED_CTRL                    = 0x03,
    CMD_AUTO_ID                     = 0x04,
    CMD_GET_ID                      = 0x05, // TODO: change in get-info
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
