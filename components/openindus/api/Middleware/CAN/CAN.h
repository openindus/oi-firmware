/**
 * @file CAN.h
 * @brief CAN bus
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include <stdint.h>
#include "pcal6524/pcal6524.h"
#include "mcp25625/DriversComponents/MCP25625.h"

/**
 * @brief CAN message structure
 */
typedef struct {
    uint32_t id;    // CAN id
    uint8_t size;   // Size of the message (0 to 8)
    bool IDE;       // Extended frame : true, Standard frame : false
    bool RTR;       // Remote transfer frame or data frame
    uint8_t msg[8]; // Message array
} CAN_Message_t;

/**
 * @brief CAN bus class
 */
class CAN
{
public:
    /**
     * @brief Construct a new CAN object
     * @param host SPI host device
     * @param cs Chip select pin
     * @param intr Interrupt pin
     */
    CAN(spi_host_device_t host, gpio_num_t cs, gpio_num_t intr);

    /**
     * @brief Initialize the CAN communication
     * @param baudrate CAN baudrate in bits/s (default: 1000000)
     * @param extended_mode Enable extended frame mode
     */
    void begin(unsigned long baudrate=1000000, bool extended_mode=false);

    /**
     * @brief Delete CAN instance
     */
    void end(void);

    /**
     * @brief Write a CAN message to the bus
     * @param msg CAN message structure
     */
    void write(CAN_Message_t msg);

    /**
     * @brief Get the number of messages available for reading
     * @return Number of available messages
     */
    int available(void);

    /**
     * @brief Read a CAN message from the bus
     * @return CAN message structure
     */
    CAN_Message_t read(void);

    /**
     * @brief Set CAN bus standard IDs filter
     * @param mask CAN mask for ID filter
     * @param filter CAN IDs filter
     */
    void setStandardFilter(uint16_t mask, uint16_t filter);

    /**
     * @brief Set CAN bus extended IDs filter
     * @param mask CAN mask for ID filter
     * @param filter CAN IDs filter
     */
    void setExtendedFilter(uint32_t mask, uint32_t filter);

private:
    spi_host_device_t _spi_host; 
    gpio_num_t _pin_cs; 
    gpio_num_t _pin_intr;
};