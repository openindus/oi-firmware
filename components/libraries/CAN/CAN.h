/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file CAN.h
 * @brief 
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#if defined(CONFIG_CORE)

#include <stdint.h>
#include "pcal6524/pcal6524.h"
#include "mcp25625/DriversComponents/MCP25625.h"

// Driver is specific to OICore, we use CorePinout.h for hadrware gpio number
#include "CorePinout.h"

#define OICAN_DEFAULT_SPI_HOST                   CORE_SPI_USER_HOST
#define OICAN_DEFAULT_SPI_PIN_CS                 CORE_PIN_CAN_SPI_CS

#if defined CONFIG_OI_CORE
#define OICAN_DEFAULT_SPI_PIN_INTERRUPT          CORE_PIN_CAN_INTERRUPT
#else
#define OICAN_DEFAULT_SPI_PIN_INTERRUPT          GPIO_NUM_26
#endif

#define CAN_MAX_LENGTH               8

/**
 * @brief CAN message structure
 * 
 */
typedef struct {
    uint32_t id; /*!< CAN id. */
    uint8_t count; /*!< Size of the message (0 to 8). */
    bool IDE; /*!< Extended frame : true, Standard frame : false. */
    bool RTR; /*!< Remote transfer frame or data frame. */
    uint8_t msg[8]; /*!< Message array. */
}OICanMessage_t;

/**
 * @brief OICAN class
 * 
 */
class OICAN

{
public:

/**
 * @brief Initialize the CAN communication
 * @note You must use a standard baudrate (50KBPS, 100KBPS, 125KBPS, 250KBPS, 500KBPS, 1000kBPS)
 * 
 * @param baudrate CAN baudrate in bits/s
 * @param host the spi type you would like to use
 * @param cs the pin you would like to use
 * @param interrupt the pin you would like to use
 */
void begin(unsigned long baudrate = 1000000, bool extended_mode = false, spi_host_device_t host = OICAN_DEFAULT_SPI_HOST, gpio_num_t cs = OICAN_DEFAULT_SPI_PIN_CS, gpio_num_t interrupt = OICAN_DEFAULT_SPI_PIN_INTERRUPT);

/**
 * @brief Delete CAN instance
 * 
 */
void end();

/**
 * @brief Get the number of bytes available for reading
 * 
 * @return the number of bytes available to read
 */
int available(void);

/**
 * @brief Reads incoming CAN data.
 * 
 * @return the first message of incoming CAN data available (or -1 if no data is available)
 */
OICanMessage_t read(void);

/**
 * @brief Writes binary data to the CAN bus.
 * 
 * @param data an array to send as a series of bytes.
 * @param len the number of bytes to be sent from the array.
 */
void write(OICanMessage_t msg);

/**
 * @brief Set CAN bus standard IDs filter.
 * 
 * @param mask CAN mask for ID filter.
 * @param filter CAN IDs filter.
 */
void setStandardFilter(uint16_t mask, uint16_t filter);

/**
 * @brief Set CAN bus extended IDs filter.
 * 
 * @param mask CAN mask for ID filter.
 * @param filter CAN IDs filter.
 */
void setExtendedFilter(uint32_t mask, uint32_t filter);

};

extern OICAN CAN;

#endif
