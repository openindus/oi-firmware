/**
 * @file CAN.cpp
 * @brief CAN bus
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "CAN.h"

CAN::CAN(spi_host_device_t host, gpio_num_t cs, gpio_num_t intr)
    : _spi_host(host), _pin_cs(cs), _pin_intr(intr)
{
}

void CAN::begin(unsigned long baudrate, bool extended_mode)
{
    MCP25625_DeviceConfig_t deviceConfig;
    deviceConfig.spi_host = _spi_host;
    deviceConfig.spi_pin_cs = _pin_cs;
    deviceConfig.spi_freq = MCP25625_SPI_FREQ;
    deviceConfig.spi_pin_int = _pin_intr;
    mcp25625_create(&deviceConfig);
    mcp25625_init(OPMODE_NORMAL, (can_baudrate_t)baudrate, (can_id_mode_t)(extended_mode+1));

}

void CAN::end(void)
{
    mcp25625_delete();
}

void CAN::write(CAN_Message_t msg)
{
    mcp25625_msg_transfer(msg.msg, msg.size, msg.id, msg.IDE, msg.RTR);
}

int CAN::available(void)
{
    return mcp25625_queue_available();
}

CAN_Message_t CAN::read(void)
{
    CAN_Message_t canMsg;
    mcp25625_queue_read((MCP25625_canMessage_t*)(void*)(&canMsg));

    return canMsg;
}

void CAN::setStandardFilter(uint16_t mask, uint16_t filter)
{
    mcp25625_mask_config(RXB0, mask, 0);
    mcp25625_mask_config(RXB1, mask, 0);
    mcp25625_filter_config(RXF_0, filter, 0, false);
    mcp25625_filter_config(RXF_1, filter, 0, false);
}

void CAN::setExtendedFilter(uint32_t mask, uint32_t filter)
{
    mcp25625_mask_config(RXB0, 0, mask);
    mcp25625_mask_config(RXB1, 0, mask);
    mcp25625_filter_config(RXF_0, 0, filter, true);
    mcp25625_filter_config(RXF_1, 0, filter, true);
}