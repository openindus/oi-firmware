#include "CAN.h"

void OICAN::begin(unsigned long baudrate, bool extended_mode, spi_host_device_t host, gpio_num_t cs, gpio_num_t interrupt)
{
    MCP25625_DeviceConfig_t deviceConfig;
    deviceConfig.spi_host = host;
    deviceConfig.spi_pin_cs = cs;
    deviceConfig.spi_freq = MCP25625_SPI_FREQ;
    deviceConfig.spi_pin_int = interrupt;
    mcp25625_create(&deviceConfig);
    mcp25625_init(OPMODE_NORMAL, (can_baudrate_t)baudrate, (can_id_mode_t)(extended_mode+1));

}

void OICAN::end()
{
    mcp25625_delete();
}

void OICAN::write(OICanMessage_t msg)
{
        mcp25625_msg_transfer(msg.msg, msg.count, msg.id, msg.IDE, msg.RTR);
}

int OICAN::available()
{
    return mcp25625_queue_available();
}

OICanMessage_t OICAN::read()
{
    OICanMessage_t canMsg;
    mcp25625_queue_read((MCP25625_canMessage_t*)(void*)(&canMsg));

    return canMsg;
}

void OICAN::setStandardFilter(uint16_t mask, uint16_t filter)
{
    mcp25625_mask_config(RXB0, mask, 0);
    mcp25625_mask_config(RXB1, mask, 0);
    mcp25625_filter_config(RXF_0, filter, 0, false);
    mcp25625_filter_config(RXF_1, filter, 0, false);
}

void OICAN::setExtendedFilter(uint32_t mask, uint32_t filter)
{
    mcp25625_mask_config(RXB0, 0, mask);
    mcp25625_mask_config(RXB1, 0, mask);
    mcp25625_filter_config(RXF_0, 0, filter, true);
    mcp25625_filter_config(RXF_1, 0, filter, true);
}

OICAN CAN;