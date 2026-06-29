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

/* TYPES */

typedef uint8_t reg_addr_t;
typedef uint8_t reg_value_t;

typedef uint16_t reg_bitfield_t;

/* MACROS AND CONSTANTS */

#define CONSTRUCT_BITFIELD(mask, value) (((uint16_t)(mask) << 8U) | (uint16_t)(value))
#define BITFIELD(reg, subset, value)                                                               \
    CONSTRUCT_BITFIELD(reg##_##subset##_MASK, reg##_##subset##_##value)
#define BITFIELD_P(reg, subset, parameter)                                                         \
    CONSTRUCT_BITFIELD(reg##_##subset##_MASK, reg##_##subset(parameter))
#define IS_BIT_SET(reg, subset, value) (bool)((value) & (reg##_##subset##_MASK))

#define EXTRACT_BF_MASK(bitfield) ((uint8_t)(((bitfield) & 0xff00U) >> 8U))
#define EXTRACT_BF_VALUES(bitfield) ((uint8_t)((bitfield) & 0x00ffU))

static reg_addr_t const REG_CANCTRL                  = 0x0F; // Control Register
static reg_value_t const CANCTRL_REQOP_MASK          = 0xE0;
static reg_value_t const CANCTRL_REQOP_NORMAL        = 0x00;
static reg_value_t const CANCTRL_REQOP_SLEEP         = 0x20;
static reg_value_t const CANCTRL_REQOP_LOOPBACK      = 0x40;
static reg_value_t const CANCTRL_REQOP_LISTEN_ONLY   = 0x60;
static reg_value_t const CANCTRL_REQOP_CONFIGURATION = 0x80;
static reg_value_t const CANCTRL_ABAT_MASK           = 0x10;
static reg_value_t const CANCTRL_ABAT_ON             = 0x10;
static reg_value_t const CANCTRL_ABAT_OFF            = 0x00;
static reg_value_t const CANCTRL_OSM_MASK            = 0x08;
static reg_value_t const CANCTRL_OSM_ON              = 0x08;
static reg_value_t const CANCTRL_OSM_OFF             = 0x00;
static reg_value_t const CANCTRL_CLKEN_MASK          = 0x04;
static reg_value_t const CANCTRL_CLKEN_ON            = 0x04;
static reg_value_t const CANCTRL_CLKEN_OFF           = 0x00;
static reg_value_t const CANCTRL_CLKPRE_MASK         = 0x03;
static reg_value_t const CANCTRL_CLKPRE_1            = 0x00;
static reg_value_t const CANCTRL_CLKPRE_2            = 0x01;
static reg_value_t const CANCTRL_CLKPRE_4            = 0x02;
static reg_value_t const CANCTRL_CLKPRE_8            = 0x03;

static reg_addr_t const REG_CANINTF         = 0x2C; // Interrupt flags register
static reg_value_t const CANINTF_MERRF_MASK = 0x80;
static reg_value_t const CANINTF_WAKIF_MASK = 0x40;
static reg_value_t const CANINTF_ERRIF_MASK = 0x20;
static reg_value_t const CANINTF_TX2IF_MASK = 0x10;
static reg_value_t const CANINTF_TX1IF_MASK = 0x08;
static reg_value_t const CANINTF_TX0IF_MASK = 0x04;
static reg_value_t const CANINTF_RX1IF_MASK = 0x02;
static reg_value_t const CANINTF_RX0IF_MASK = 0x01;

static reg_addr_t const REG_CANINTE         = 0x2B; // Interrupt enable register
static reg_value_t const CANINTE_MERRE_MASK = 0x80;
static reg_value_t const CANINTE_WAKIE_MASK = 0x40;
static reg_value_t const CANINTE_ERRIE_MASK = 0x20;
static reg_value_t const CANINTE_TX2IE_MASK = 0x10;
static reg_value_t const CANINTE_TX1IE_MASK = 0x08;
static reg_value_t const CANINTE_TX0IE_MASK = 0x04;
static reg_value_t const CANINTE_RX1IE_MASK = 0x02;
static reg_value_t const CANINTE_RX0IE_MASK = 0x01;
static reg_value_t const CANINTE_ALL_ON     = 0xFF;

static reg_addr_t const REG_EFLG          = 0x2D; // Error flag register
static reg_value_t const EFLG_RX1OVR_MASK = 0x80;
static reg_value_t const EFLG_RX0OVR_MASK = 0x40;
static reg_value_t const EFLG_TXBO_MASK   = 0x20;
static reg_value_t const EFLG_TXEP_MASK   = 0x10;
static reg_value_t const EFLG_RXEP_MASK   = 0x08;
static reg_value_t const EFLG_TXWAR_MASK  = 0x04;
static reg_value_t const EFLG_RXWAR_MASK  = 0x02;
static reg_value_t const EFLG_EWARN_MASK  = 0x01;

static reg_addr_t const REG_CNF1       = 0x2A; // Configuration register 1
static reg_value_t const CNF1_SJW_MASK = 0xC0;
#define CNF1_SJW(value) (((value) << 6U) & CNF1_SJW_MASK)
static reg_value_t const CNF1_BRP_MASK = 0x3F;
#define CNF1_BRP(value) ((value) & CNF1_BRP_MASK)

static reg_addr_t const REG_CNF2           = 0x29; // Configuration register 2
static reg_value_t const CNF2_BTLMODE_MASK = 0x80;
static reg_value_t const CNF2_BTLMODE_ON   = 0x80;
static reg_value_t const CNF2_BTLMODE_OFF  = 0x00;
static reg_value_t const CNF2_SAM_MASK     = 0x40;
static reg_value_t const CNF2_SAM_THRICE   = 0x40;
static reg_value_t const CNF2_SAM_ONCE     = 0x00;
static reg_value_t const CNF2_PHSEG1_MASK  = 0x38;
#define CNF2_PHSEG1(value) (((value) << 3U) & CNF2_PHSEG1_MASK)
static reg_value_t const CNF2_PRSEG_MASK = 0x07;
#define CNF2_PRSEG(value) ((value) & CNF2_PRSEG_MASK)

static reg_addr_t const REG_CNF3              = 0x28; // Configuration register 3
static reg_value_t const CNF3_SOF_MASK        = 0x80;
static reg_value_t const CNF3_SOF_ON          = 0x80;
static reg_value_t const CNF3_SOF_OFF         = 0x00;
static reg_value_t const CNF3_WAKFIL_MASK     = 0x40;
static reg_value_t const CNF3_WAKFIL_ENABLED  = 0x40;
static reg_value_t const CNF3_WAKFIL_DISABLED = 0x00;
static reg_value_t const CNF3_PHSEG2_MASK     = 0x07;
#define CNF3_PHSEG2(value) ((value) & CNF3_PHSEG2_MASK)

static reg_addr_t const REG_TXB0CTRL          = 0x30; // Transmit buffer 0 control
static reg_value_t const TXB0CTRL_TXREQ_MASK  = 0x08; // Request to send bit
static reg_value_t const TXB0CTRL_TXREQ_SEND  = 0x08;
static reg_value_t const TXB0CTRL_TXREQ_ABORT = 0x00;

static reg_addr_t const REG_TXB0SIDH = 0x31; // Transmit buffer 0 standard identifier high
static reg_addr_t const REG_TXB0SIDL = 0x32; // Transmit buffer 0 standard identifier low
static reg_addr_t const REG_TXB0EID8 = 0x33; // Transmit buffer 0 extended identifier high
static reg_addr_t const REG_TXB0EID0 = 0x34; // Transmit buffer 0 extended identifier low
static reg_addr_t const REG_TXB0DLC  = 0x35; // Transmit buffer 0 data length code
static reg_addr_t const REG_TXB0DATA = 0x36; // Transmit buffer 0 data byte 0
