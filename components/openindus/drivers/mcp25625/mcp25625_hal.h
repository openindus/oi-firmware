/**
 * @file mcp25625_hal.h
 * @brief MCP25625 Driver Hardware Abstraction Layer
 * @author Mani Gillier <mani.gillier@openindus.com>
 * @copyright (c) [2026] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "mcp25625.h"
#include "mcp25625_reg.h"
#include <driver/spi_master.h>
#include <esp_err.h>

//
/* BASE SPI ACCESS */
//

enum mcp25625_commands : uint8_t {
    MCP25625_COMMAND_WRITE = 0b00000010,
    MCP25625_COMMAND_READ  = 0b00000011
};

esp_err_t reg_write(reg_addr_t reg, reg_value_t value);
esp_err_t reg_read(reg_addr_t reg, reg_value_t *value);
esp_err_t reg_write_bitfield(reg_addr_t addr, reg_bitfield_t bitfield);

//
/* DEBUG OR LOG */
//

enum { LOG_BUF_SIZE = 256 };

// Register EFLG
char const *error_name(uint8_t mask);
// Register CANINTF
char const *interrupt_name(uint8_t mask);

/**
 * @brief Used to print a log with the value of a boolean filled registed
 */
void log_register(reg_value_t value, char const *reg_name, char const *(*get_name)(uint8_t mask));

//
/* CAN & SPI Interface */
//

enum {
    CAN_RX_QUEUE_SIZE = 16,
    CAN_TX_QUEUE_SIZE = 16,
    SPI_CLOCK_SPEED   = 4000000U,
    SPI_QUEUE_SIZE    = 16,
    SPI_COMMAND_BITS  = 8,
    SPI_ADDRESS_BITS  = 8,
    BYTESIZE          = 8
};

/**
 * @brief MCP25625 data structure
 */

extern TaskHandle_t mcp25625_task_handle;       /**< Task handle for CAN processing */
extern spi_device_handle_t mcp25625_spi_handle; /**< SPI device handle */

struct raw_can_message {
    uint16_t sid : 11;
    uint8_t srr : 1;
    uint8_t ide : 1;
    uint32_t eid : 29;
    uint8_t rtr : 1;
    uint8_t dlc : 4;
    uint8_t data[CAN_MAX_BYTES];
};

/**
 * @brief Reads the RX0 message buffer and store the raw data in message_ptr
 * @par message_ptr A non-NULL pointer to a struct raw_can_message.
 */
esp_err_t read_raw_message(struct raw_can_message *message_ptr);

//
/* INTERRUPT MANAGEMENT */
//

esp_err_t manage_interrupt(reg_value_t mask);
void mcp25625_can_task(void *args);
IRAM_ATTR void mcp25625_isr(void *args);
esp_err_t mcp25625_init_isr(gpio_num_t intr);

esp_err_t mcp25625_hal_configure();
void mcp25625_hal_stop();
esp_err_t mcp25625_init_spi();
