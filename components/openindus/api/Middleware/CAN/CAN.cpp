/**
 * @file CAN.cpp
 * @brief CAN bus
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "CAN.h"
#include "mcp25625.h"

CAN::CAN(spi_host_device_t host, gpio_num_t cs, gpio_num_t intr)
    : _spi_host(host), _pin_cs(cs), _pin_intr(intr)
{
}

void CAN::begin(unsigned long baudrate, bool extended_mode)
{
    mcp25625_can_init(_spi_host, _pin_cs, _pin_intr);
    mcp25625_can_begin(baudrate, extended_mode);
}

void CAN::end(void)
{
    mcp25625_can_stop();
}

esp_err_t CAN::write(CAN_Message_t msg)
{
    struct can_message can_msg;
    can_msg.id = msg.id;
    can_msg.size = msg.size;
    can_msg.IDE = msg.IDE;
    can_msg.RTR = msg.RTR;
    for (int index = 0; index < sizeof(msg.msg); index++) {
	can_msg.msg[index] = msg.msg[index];
    }
    return mcp25625_can_write(&can_msg);
}

int CAN::available(void)
{
    return mcp25625_can_available();
}

CAN_Message_t CAN::read(void)
{
    struct can_message msg;
    mcp25625_can_read(&msg);
    CAN_Message_t can_msg;
    can_msg.id = msg.id;
    can_msg.size = msg.size;
    can_msg.IDE = msg.IDE;
    can_msg.RTR = msg.RTR;
    for (int index = 0; index < sizeof(can_msg.msg); index++) {
	can_msg.msg[index] = msg.msg[index];
    }
    return can_msg;
}

void CAN::setStandardFilter(uint16_t mask, uint16_t filter)
{
    /*
      mcp25625_mask_config(RXB0, mask, 0);
      mcp25625_mask_config(RXB1, mask, 0);
      mcp25625_filter_config(RXF_0, filter, 0, false);
      mcp25625_filter_config(RXF_1, filter, 0, false);
    */
}

void CAN::setExtendedFilter(uint32_t mask, uint32_t filter)
{
    /*
      mcp25625_mask_config(RXB0, 0, mask);
      mcp25625_mask_config(RXB1, 0, mask);
      mcp25625_filter_config(RXF_0, 0, filter, true);
      mcp25625_filter_config(RXF_1, 0, filter, true);
    */
}
