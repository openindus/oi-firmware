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

typedef struct {
    uint32_t id;    // CAN id
    uint8_t size;   // Size of the message (0 to 8)
    bool IDE;       // Extended frame : true, Standard frame : false
    bool RTR;       // Remote transfer frame or data frame
    uint8_t msg[8]; // Message array
} CAN_Message_t;

class CAN
{
public:

    CAN(spi_host_device_t host, gpio_num_t cs, gpio_num_t intr) :
        _spi_host(host),
        _pin_cs(cs),
        _pin_intr(intr) {}

    void begin(unsigned long baudrate=1000000, bool extended_mode=false);

    void end(void);

    void write(CAN_Message_t msg);
    int available(void);
    CAN_Message_t read(void);
    void setStandardFilter(uint16_t mask, uint16_t filter);
    void setExtendedFilter(uint32_t mask, uint32_t filter);

private:

    spi_host_device_t _spi_host; 
    gpio_num_t _pin_cs; 
    gpio_num_t _pin_intr;

};