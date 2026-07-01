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

/* Baudrate config */

enum baudrate_config_sjw : uint8_t {
    BAUDCONF_SJW_X1 = 0b00,
    BAUDCONF_SJW_X2 = 0b01,
    BAUDCONF_SJW_X3 = 0b10,
    BAUDCONF_SJW_X4 = 0b11
};

struct baudrate_config {
    enum baudrate_config_sjw sjw : 2;
    uint8_t brp : 6;
    uint8_t phseg1 : 3;
    uint8_t prseg : 3;
    uint8_t phseg2 : 3;
};

static struct baudrate_config const BAUDRATE_CONFIG_1M = {
    .sjw = BAUDCONF_SJW_X4, .brp = 0, .phseg1 = 3, .prseg = 1, .phseg2 = 2};
static struct baudrate_config const BAUDRATE_CONFIG_500K = {
    .sjw = BAUDCONF_SJW_X4, .brp = 0, .phseg1 = 4, .prseg = 7, .phseg2 = 5};

esp_err_t apply_baudrate_config(struct baudrate_config const *config);
struct baudrate_config const *get_baudrate_config(enum mcp25625_can_baudrate baudrate);

extern TaskHandle_t mcp25625_rx_task_handle;    /**< Task handle for RX CAN processing */
extern TaskHandle_t mcp25625_tx_task_handle;    /**< Task handle for TX CAN processing */
extern spi_device_handle_t mcp25625_spi_handle; /**< SPI device handle */
extern SemaphoreHandle_t spi_mutex;             /**< Mutex for SPI access to ensure thread safety */

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

void convert_raw_message(struct raw_can_message const *source, struct can_message *destination);

//
/* INTERRUPT MANAGEMENT */
//

void manage_interrupts(reg_value_t value);
esp_err_t manage_interrupt(reg_value_t mask);
void mcp25625_rx_task(void *args);
IRAM_ATTR void mcp25625_isr(void *args);
esp_err_t mcp25625_init_isr(gpio_num_t intr);

esp_err_t mcp25625_hal_configure();
esp_err_t mcp25625_hal_reconfigure();
void mcp25625_hal_stop();
esp_err_t mcp25625_init_spi();

//
/* Transmit Queue */
//

void mcp25625_tx_task(void *args);
void write_msg_to_tx_buffer(struct can_message *msg);
extern SemaphoreHandle_t tx_sem;
