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
#include <stdint.h>

/* STRUCTURES AND ENUMS */

/**
 * @brief MCP25625 data structure
 */
struct mcp25625_can {
    spi_host_device_t host;     /**< SPI host device */
    gpio_num_t cs;              /**< Chip select pin */
    gpio_num_t intr;            /**< Interrupt pin */
    unsigned long baudrate;     /**< CAN baudrate in bits/s */
    bool extended_mode;         /**< Extended frame mode */
    bool initialized;           /**< Initialization status */
    spi_device_handle_t handle; /**< SPI device handle */
};

extern struct mcp25625_can mcp25625_can_instance;

/* TYPES */

typedef uint8_t reg_addr_t;
typedef uint8_t reg_value_t;

/* CONSTANTS */

#define BYTESIZE 8

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

#ifdef __cplusplus
}
#endif
