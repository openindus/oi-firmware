/**
 * @file mcp25625_reg.h
 * @brief MCP25625 Register Map
 * @author Mani Gillier <mani.gillier@openindus.com>
 * @copyright (c) [2026] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "mcp25625.h"
#include <stdint.h>

typedef uint16_t reg_bitfield_t;
esp_err_t reg_write_bitfield(reg_addr_t addr, reg_bitfield_t bitfield);

#define CONSTRUCT_BITFIELD(mask, value) (uint16_t)(((mask) << 8U) | (value))
#define BITFIELD(reg, subset, value)                                                     \
    CONSTRUCT_BITFIELD(reg##_##subset##_MASK, reg##_##subset##_##value)
#define IS_BIT_SET(reg, subset, value) (bool)((value) & (reg##_##subset##_MASK))

#define EXTRACT_BF_MASK(bitfield) ((uint8_t)(((bitfield) & 0xff00U) >> 8U))
#define EXTRACT_BF_VALUES(bitfield) ((uint8_t)((bitfield) & 0x00ffU))

reg_addr_t const REG_CANCTRL = 0x0F; // Control Register
reg_value_t const CANCTRL_REQOP_MASK = 0xE0;
reg_value_t const CANCTRL_REQOP_NORMAL = 0x00;
reg_value_t const CANCTRL_REQOP_SLEEP = 0x20;
reg_value_t const CANCTRL_REQOP_LOOPBACK = 0x40;
reg_value_t const CANCTRL_REQOP_LISTEN_ONLY = 0x60;
reg_value_t const CANCTRL_REQOP_CONFIGURATION = 0x80;
reg_value_t const CANCTRL_ABAT_MASK = 0x10;
reg_value_t const CANCTRL_ABAT_ON = 0x10;
reg_value_t const CANCTRL_ABAT_OFF = 0x00;
reg_value_t const CANCTRL_OSM_MASK = 0x08;
reg_value_t const CANCTRL_OSM_ON = 0x08;
reg_value_t const CANCTRL_OSM_OFF = 0x00;
reg_value_t const CANCTRL_CLKEN_MASK = 0x04;
reg_value_t const CANCTRL_CLKEN_ON = 0x04;
reg_value_t const CANCTRL_CLKEN_OFF = 0x00;
reg_value_t const CANCTRL_CLKPRE_MASK = 0x03;
reg_value_t const CANCTRL_CLKPRE_1 = 0x00;
reg_value_t const CANCTRL_CLKPRE_2 = 0x01;
reg_value_t const CANCTRL_CLKPRE_4 = 0x02;
reg_value_t const CANCTRL_CLKPRE_8 = 0x03;

reg_addr_t const REG_CANINTF = 0x2C; // Interrupt flags register
reg_value_t const CANINTF_MERRF_MASK = 0x80;
reg_value_t const CANINTF_WAKIF_MASK = 0x40;
reg_value_t const CANINTF_ERRIF_MASK = 0x20;
reg_value_t const CANINTF_TX2IF_MASK = 0x10;
reg_value_t const CANINTF_TX1IF_MASK = 0x08;
reg_value_t const CANINTF_TX0IF_MASK = 0x04;
reg_value_t const CANINTF_RX1IF_MASK = 0x02;
reg_value_t const CANINTF_RX0IF_MASK = 0x01;

reg_addr_t const REG_CANINTE = 0x2B; // Interrupt enable register
reg_value_t const CANINTE_MERRE_MASK = 0x80;
reg_value_t const CANINTE_WAKIE_MASK = 0x40;
reg_value_t const CANINTE_ERRIE_MASK = 0x20;
reg_value_t const CANINTE_TX2IE_MASK = 0x10;
reg_value_t const CANINTE_TX1IE_MASK = 0x08;
reg_value_t const CANINTE_TX0IE_MASK = 0x04;
reg_value_t const CANINTE_RX1IE_MASK = 0x02;
reg_value_t const CANINTE_RX0IE_MASK = 0x01;

reg_addr_t const REG_EFLG = 0x2D; // Error flag register
reg_value_t const EFLG_RX1OVR_MASK = 0x80;
reg_value_t const EFLG_RX0OVR_MASK = 0x40;
reg_value_t const EFLG_TXBO_MASK = 0x20;
reg_value_t const EFLG_TXEP_MASK = 0x10;
reg_value_t const EFLG_RXEP_MASK = 0x08;
reg_value_t const EFLG_TXWAR_MASK = 0x04;
reg_value_t const EFLG_RXWAR_MASK = 0x02;
reg_value_t const EFLG_EWARN_MASK = 0x01;
