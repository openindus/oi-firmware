/**
 * @file mcp25625.h
 * @brief MCP25625 API
 * @author Mani Gillier <mani.gillier@openindus.com>
 * @copyright (c) [2026] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* HEADERS */

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include <stdbool.h>

/* PROTOTYPES */

/**
 * @brief Init interface
 * @param host SPI host device
 * @param cs SPI Chip Select pin
 * @param intr SPI Interrupt pin
 * @return esp_err_t ESP_OK on success, error code otherwise
 */
esp_err_t mcp25625_can_init(spi_host_device_t host, gpio_num_t cs, gpio_num_t intr);

/**
 * @brief Begin
 */
void mcp25625_can_begin(unsigned long baudrate, bool extended_mode);

/**
 * @brief Deinit
 */
void mcp25625_can_deinit();

/**
 * @brief Write can message
 */
void mcp25625_can_write();

/**
 * @brief Read can message
 */
void mcp25625_can_read();

/**
 * @brief Set standard filter
 */
void mcp25625_can_set_standard_filter();

/**
 * @brief Set extended filter
 */
void mcp25625_can_set_extended_filter();

/**
 * @brief Does the queue have messages to read
 */
bool mcp25625_can_available();

/**
 * @brief Wait for message to be available
 */
void mcp25625_can_wait();

/* STRUCTS & GLOBALS */

enum { CAN_MAX_BYTES = 8 };

struct mcp25625_can_instance {
    spi_host_device_t host; /**< SPI host device */
    gpio_num_t cs;          /**< Chip select pin */
    gpio_num_t intr;        /**< Interrupt pin */
    unsigned long baudrate; /**< CAN baudrate in bits/s */
    bool extended_mode;     /**< Extended frame mode */
    bool initialized;       /**< Initialization status */
};
extern struct mcp25625_can_instance mcp25625_instance;

struct can_message {
    uint32_t id : 29;           /**< CAN message ID */
    uint8_t size : 4;           /**< CAN message size */
    bool IDE : 1;               /**< Extended frame: true, Standard frame: false */
    bool RTR : 1;               /**< Remote transfer frame or data frame */
    uint8_t msg[CAN_MAX_BYTES]; /**< CAN message data */
};

extern StaticQueue_t mcp25625_rx_queue_buffer;
extern uint8_t mcp25625_rx_buffer[];
extern QueueHandle_t mcp25625_rx_queue;

extern StaticQueue_t mcp25625_tx_queue_buffer;
extern uint8_t mcp25625_tx_buffer[];
extern QueueHandle_t mcp25625_tx_queue;

#ifdef __cplusplus
}
#endif
