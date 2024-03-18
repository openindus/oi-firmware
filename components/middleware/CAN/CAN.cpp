/**
 * @file CAN.cpp
 * @brief CAN bus
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "CAN.h"

namespace OI 
{
    /**
     * @brief Initialize the CAN communication
     * @note You must use a standard baudrate (50KBPS, 100KBPS, 125KBPS, 250KBPS, 500KBPS, 1000kBPS)
     * 
     * @param baudrate CAN baudrate in bits/s
     * @param host the spi type you would like to use
     * @param cs the pin you would like to use
     * @param interrupt the pin you would like to use
     */
    void CAN::begin(unsigned long baudrate, bool extended_mode, spi_host_device_t host, gpio_num_t cs, gpio_num_t interrupt)
    {
        MCP25625_DeviceConfig_t deviceConfig;
        deviceConfig.spi_host = host;
        deviceConfig.spi_pin_cs = cs;
        deviceConfig.spi_freq = MCP25625_SPI_FREQ;
        deviceConfig.spi_pin_int = interrupt;
        mcp25625_create(&deviceConfig);
        mcp25625_init(OPMODE_NORMAL, (can_baudrate_t)baudrate, (can_id_mode_t)(extended_mode+1));

    }

    /**
     * @brief Delete CAN instance
     * 
     */
    void CAN::end(void)
    {
        mcp25625_delete();
    }

    /**
     * @brief Writes binary data to the CAN bus.
     * 
     * @param data an array to send as a series of bytes.
     * @param len the number of bytes to be sent from the array.
     */
    void CAN::write(CAN_Message_t msg)
    {
        mcp25625_msg_transfer(msg.msg, msg.size, msg.id, msg.IDE, msg.RTR);
    }

    /**
     * @brief Get the number of bytes available for reading
     * 
     * @return the number of bytes available to read
     */
    int CAN::available(void)
    {
        return mcp25625_queue_available();
    }

    /**
     * @brief Reads incoming CAN data.
     * 
     * @return the first message of incoming CAN data available (or -1 if no data is available)
     */
    CAN_Message_t CAN::read(void)
    {
        CAN_Message_t canMsg;
        mcp25625_queue_read((MCP25625_canMessage_t*)(void*)(&canMsg));

        return canMsg;
    }

    /**
     * @brief Set CAN bus standard IDs filter.
     * 
     * @param mask CAN mask for ID filter.
     * @param filter CAN IDs filter.
     */
    void CAN::setStandardFilter(uint16_t mask, uint16_t filter)
    {
        mcp25625_mask_config(RXB0, mask, 0);
        mcp25625_mask_config(RXB1, mask, 0);
        mcp25625_filter_config(RXF_0, filter, 0, false);
        mcp25625_filter_config(RXF_1, filter, 0, false);
    }

    /**
     * @brief Set CAN bus extended IDs filter.
     * 
     * @param mask CAN mask for ID filter.
     * @param filter CAN IDs filter.
     */
    void CAN::setExtendedFilter(uint32_t mask, uint32_t filter)
    {
        mcp25625_mask_config(RXB0, 0, mask);
        mcp25625_mask_config(RXB1, 0, mask);
        mcp25625_filter_config(RXF_0, 0, filter, true);
        mcp25625_filter_config(RXF_1, 0, filter, true);
    }
}