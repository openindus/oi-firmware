/**
 * @file mcp25625.c
 * @brief MCP25625 Driver
 * @author Mani Gillier <mani.gillier@openindus.com>
 * @copyright (c) [2026] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "mcp25625.h"
#include "OSAL.h"
#include "mcp25625_hal.h"
#include <esp_err.h>

/* GLOBALS */

static char const *const TAG = "MCP25625";

struct mcp25625_can_instance mcp25625_instance = {
    .host = -1, .cs = -1, .intr = -1, .baudrate = 0, .extended_mode = false, .initialized = false};

StaticQueue_t mcp25625_rx_queue_buffer                                     = {0};
uint8_t mcp25625_rx_buffer[sizeof(struct can_message) * CAN_RX_QUEUE_SIZE] = {0};
QueueHandle_t mcp25625_rx_queue                                            = NULL;

StaticQueue_t mcp25625_tx_queue_buffer                                     = {0};
uint8_t mcp25625_tx_buffer[sizeof(struct can_message) * CAN_TX_QUEUE_SIZE] = {0};
QueueHandle_t mcp25625_tx_queue                                            = NULL;

/* FUNCTIONS */

esp_err_t mcp25625_can_init(spi_host_device_t host, gpio_num_t cs, gpio_num_t intr)
{
    mcp25625_instance.host = host;
    mcp25625_instance.cs   = cs;
    mcp25625_instance.intr = intr;
    mcp25625_rx_queue      = xQueueCreateStatic(CAN_RX_QUEUE_SIZE, sizeof(struct can_message),
                                                mcp25625_rx_buffer, &mcp25625_rx_queue_buffer);
    mcp25625_tx_queue      = xQueueCreateStatic(CAN_TX_QUEUE_SIZE, sizeof(struct can_message),
                                                mcp25625_tx_buffer, &mcp25625_tx_queue_buffer);
    if (!mcp25625_rx_queue || !mcp25625_tx_queue) {
        LOGE(TAG, "Failed to create queues");
        return ESP_FAIL;
    }
    // Init spi device
    return mcp25625_init_spi();
}

void mcp25625_can_begin(enum mcp25625_can_baudrate baudrate, bool extended_mode)
{
    if (mcp25625_instance.initialized) {
        LOGW(TAG, "MCP25625 is already started, please use mcp25625_can_stop before trying to "
                  "begin again");
        return;
    }
    mcp25625_instance.baudrate      = baudrate;
    mcp25625_instance.extended_mode = extended_mode;

    esp_err_t err                 = mcp25625_hal_configure();
    mcp25625_instance.initialized = err == ESP_OK;
}

void mcp25625_can_stop()
{
    mcp25625_hal_stop();
    mcp25625_instance.initialized = false;
}

void mcp25625_can_read(struct can_message *msg_ptr)
{
    xQueueReceive(mcp25625_rx_queue, msg_ptr, portMAX_DELAY);
}

esp_err_t mcp25625_can_write(struct can_message *msg_ptr)
{
    BaseType_t ret = xQueueSend(mcp25625_tx_queue, msg_ptr, 0);
    return ret == pdPASS ? ESP_OK : ESP_FAIL;
}

bool mcp25625_can_available() { return uxQueueMessagesWaiting(mcp25625_rx_queue) > 0; }

void mcp25625_can_reconfigure(enum mcp25625_can_baudrate baudrate, bool extended_mode)
{
    mcp25625_instance.baudrate      = baudrate;
    mcp25625_instance.extended_mode = extended_mode;
    mcp25625_hal_reconfigure();
}

void mcp25625_can_set_standard_filter(uint16_t mask, uint16_t filter)
{
    reg_write_bitfield(REG_CANCTRL, BITFIELD(CANCTRL, REQOP, CONFIGURATION));

    reg_write(REG_RXM0SIDH, (mask >> 3) & 0xFF); // 10:3 placed at 7:0
    reg_write(REG_RXM0SIDL, (mask << 5) & 0xE0); // 2:0 placed at 7:5
    reg_write(REG_RXM0EID8, 0x00);
    reg_write(REG_RXM0EID0, 0x00);

    reg_write(REG_RXF0SIDH, (filter >> 3) & 0xFF);
    reg_write(REG_RXF0SIDL, (filter << 5) & 0xE0);
    reg_write(REG_RXF0EID8, 0x00);
    reg_write(REG_RXF0EID0, 0x00);
    reg_write(REG_RXF1SIDH, (filter >> 3) & 0xFF);
    reg_write(REG_RXF1SIDL, (filter << 5) & 0xE0);
    reg_write(REG_RXF1EID8, 0x00);
    reg_write(REG_RXF1EID0, 0x00);

    reg_write_bitfield(REG_CANCTRL, BITFIELD(CANCTRL, REQOP, NORMAL));
}

void mcp25625_can_set_extended_filter(uint32_t mask, uint32_t filter)
{
    reg_write_bitfield(REG_CANCTRL, BITFIELD(CANCTRL, REQOP, CONFIGURATION));

    // 28:21 placed at 7:0
    reg_write(REG_RXM0SIDH, (mask >> 21) & 0xFF);
    // 20:18 placed at 7:5, 17:16 placed at 1:0
    reg_write(REG_RXM0SIDL, ((mask >> 13) & 0xE0) | ((mask >> 16) & 0x03));
    // 15:8 placed at 7:0
    reg_write(REG_RXM0EID8, (mask >> 8) & 0xFF);
    // 7:0 placed at 7:0
    reg_write(REG_RXM0EID0, mask & 0xFF);

    reg_write(REG_RXF1SIDH, (filter >> 21) & 0xFF);
    reg_write(REG_RXF1SIDL, ((filter >> 13) & 0xE0) | ((filter >> 16) & 0x03) | RXFSIDL_EXIDE);
    reg_write(REG_RXF1EID8, (filter >> 8) & 0xFF);
    reg_write(REG_RXF1EID0, filter & 0xFF);
    reg_write(REG_RXF0SIDH, (filter >> 21) & 0xFF);
    reg_write(REG_RXF0SIDL, ((filter >> 13) & 0xE0) | ((filter >> 16) & 0x03) | RXFSIDL_EXIDE);
    reg_write(REG_RXF0EID8, (filter >> 8) & 0xFF);
    reg_write(REG_RXF0EID0, filter & 0xFF);

    reg_write_bitfield(REG_CANCTRL, BITFIELD(CANCTRL, REQOP, NORMAL));
}
