/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file mcp25625.h
 * @brief Functions for MCP25625 SPI to CAN 
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "mcp25625/mcp25625.h"
#include "DriversComponents/MCP25625.h"

static const char CAN_USR_TAG[] = __FILE__;

#define CAN_USR_CHECK(a, str, goto_tag, ...)                                                    \
    do                                                                                      \
    {                                                                                       \
        if (a)                                                                              \
        {                                                                                   \
            ESP_LOGE(CAN_USR_TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__);       \
            goto goto_tag;                                                                  \
        }                                                                                   \
    } while (0)

//Private variables
static MCP25625_DeviceConfig_t _deviceConfig;
static spi_device_handle_t _spiHandler = NULL;
static bool _deviceConfigured = false;
static bool _spiInitialized = false;
TaskHandle_t canIsrTaskHandle;
SemaphoreHandle_t xSemaphoreQueue;
SemaphoreHandle_t xSemaphoreSpi;

gpio_num_t _gpio_isr;

MCP25625_canRxQueue_t rx_queue;

///////////////////////////////INIT & DEINIT FUNCTIONS/////////////////////////////////////////////////////////////////////////

esp_err_t mcp25625_create(MCP25625_DeviceConfig_t *config)
{
    gpio_config_t intr_config;
    _gpio_isr = config->spi_pin_int;
    intr_config.intr_type = GPIO_INTR_NEGEDGE;
    intr_config.mode = GPIO_MODE_INPUT;
    intr_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    intr_config.pull_up_en = GPIO_PULLUP_DISABLE;

    intr_config.pin_bit_mask = (1ULL<<_gpio_isr);

    ESP_ERROR_CHECK(gpio_config(&intr_config));
    gpio_isr_handler_add(_gpio_isr, _can_isr_handler, NULL);
    gpio_intr_enable(_gpio_isr);

    rx_queue.head = 0;
    rx_queue.tail = 0;
    rx_queue.count = 0;

    _deviceConfig = *config;
    
    spi_device_interface_config_t devConfig = {
            .command_bits = 8,
            .address_bits = 8,
            .dummy_bits = 0,
            .mode = 0,
            .duty_cycle_pos = 0,
            .cs_ena_pretrans = 0,
            .cs_ena_posttrans = 0,
            .clock_speed_hz = _deviceConfig.spi_freq,
            .input_delay_ns = 0,
            .spics_io_num = _deviceConfig.spi_pin_cs,
            .flags = 0, //SPI_DEVICE_HALFDUPLEX
            .queue_size = 1,
            .pre_cb = NULL,
            .post_cb = NULL
        };

    CAN_USR_CHECK(spi_bus_add_device(_deviceConfig.spi_host, &devConfig, &_spiHandler), "error while adding device to spi bus", err);
    _spiInitialized = true;
    ESP_LOGV(CAN_USR_TAG, "SPI configured");

    xTaskCreate(can_task_interrupt_handler, "can_task_interrupt_handler", 4096, NULL, 10, &canIsrTaskHandle);
    xSemaphoreQueue = xSemaphoreCreateMutex();
    xSemaphoreSpi = xSemaphoreCreateBinary();
    ESP_LOGV(CAN_USR_TAG, "Internal Tasks created");

    return ESP_OK;

    err:
        return ESP_FAIL;

}

esp_err_t mcp25625_delete()
{

    CAN_USR_CHECK(spi_bus_remove_device(_spiHandler), "error while removing device to spi bus", err);
    _spiInitialized = false;

    gpio_isr_handler_remove(_gpio_isr);
    gpio_intr_disable(_gpio_isr);

    vTaskDelete(canIsrTaskHandle);

    _deviceConfigured = false;
    ESP_LOGV(CAN_USR_TAG, "Device deinit successful");

    return ESP_OK;
    err:
        return ESP_FAIL;

}

///////////////////////////////SPI FUNCTIONS/////////////////////////////////////////////////////////////////////////

void SPI_Wr_Ptr( unsigned char cmd, unsigned char reg , unsigned char *data_out, int len )
{

    ESP_LOGV(CAN_USR_TAG, "Write SPI byte to the device");

    if (_spiInitialized == true)
    {

        spi_transaction_ext_t trans_cmd_addr;

        trans_cmd_addr.command_bits = 8;
        trans_cmd_addr.address_bits = 0;
        trans_cmd_addr.dummy_bits = 0;

        trans_cmd_addr.base.flags = SPI_TRANS_VARIABLE_CMD | SPI_TRANS_VARIABLE_ADDR;
        trans_cmd_addr.base.cmd = cmd;
        trans_cmd_addr.base.addr = 0;
        trans_cmd_addr.base.length = 8*len;
        trans_cmd_addr.base.rxlength = 8;
        trans_cmd_addr.base.user = NULL;
        trans_cmd_addr.base.tx_buffer = &data_out[0];
        trans_cmd_addr.base.rx_buffer = NULL;


        if(cmd == MCP25625_SPI_CMD_WRITE_REG || cmd == MCP25625_SPI_CMD_MODIFY_REG)
        {        
          trans_cmd_addr.address_bits = 8;
          trans_cmd_addr.base.addr = reg;
        }
    
        xSemaphoreTake(xSemaphoreSpi, portTICK_PERIOD_MS);
        ESP_ERROR_CHECK(spi_device_polling_transmit(_spiHandler, &trans_cmd_addr.base));
        xSemaphoreGive(xSemaphoreSpi);
    }

    else
    {
        ESP_LOGE(CAN_USR_TAG, "Configure SPI before transaction");
    }

}

void SPI_Rd_Ptr( unsigned char cmd, unsigned char reg, unsigned char * buffer, int len )
{

    ESP_LOGV(CAN_USR_TAG, "Read device SPI byte");
    uint8_t data_out[len];

    if (_spiInitialized == true)
    {

        spi_transaction_ext_t trans_cmd_addr;

        trans_cmd_addr.command_bits = 8;
        trans_cmd_addr.address_bits = 0;
        trans_cmd_addr.dummy_bits = 0;

        trans_cmd_addr.base.flags = SPI_TRANS_VARIABLE_CMD | SPI_TRANS_VARIABLE_ADDR;
        trans_cmd_addr.base.cmd = cmd;
        trans_cmd_addr.base.addr = 0;
        trans_cmd_addr.base.length = 8*len;
        trans_cmd_addr.base.rxlength = 8*len;
        trans_cmd_addr.base.user = NULL;
        trans_cmd_addr.base.tx_buffer = &data_out[0];
        trans_cmd_addr.base.rx_buffer = &buffer[0];

    
        if(cmd == MCP25625_SPI_CMD_READ_REG)
        {
            trans_cmd_addr.address_bits = 8;
            trans_cmd_addr.base.addr = reg;
        }

        xSemaphoreTake(xSemaphoreSpi, portTICK_PERIOD_MS);
        ESP_ERROR_CHECK(spi_device_polling_transmit(_spiHandler, &trans_cmd_addr.base));
        xSemaphoreGive(xSemaphoreSpi);
    }
    else
    {
        ESP_LOGE(CAN_USR_TAG, "Configure SPI before transaction");
    }

}


///////////////////////////////MCP25625 RX QUEUE FUNCTION/////////////////////////////////////////////////////////////////////////

int can_queue_push(MCP25625_canMessage_t element)
{
    xSemaphoreTake(xSemaphoreQueue, portTICK_PERIOD_MS);
    if((rx_queue.count == MCP25625_CAN_RX_QUEUE_SIZE))
    {
        ESP_LOGV(CAN_USR_TAG, "RX internal queue is full");
        return -1;
    }

    rx_queue.e[rx_queue.tail] = element;
    rx_queue.tail = (rx_queue.tail + 1) % MCP25625_CAN_RX_QUEUE_SIZE;
    rx_queue.count++;
    xSemaphoreGive(xSemaphoreQueue);
    return 0;
}

int can_queue_pop(MCP25625_canMessage_t * element)
{
    xSemaphoreTake(xSemaphoreQueue, portTICK_PERIOD_MS);
    if(!(rx_queue.count == MCP25625_CAN_RX_QUEUE_SIZE) && (rx_queue.head == rx_queue.tail)) //queue is empty ?
    {
        ESP_LOGV(CAN_USR_TAG, "RX internal queue is empty");
        return -1;
    }
    *element = rx_queue.e[rx_queue.head];
    rx_queue.head = (rx_queue.head + 1) % MCP25625_CAN_RX_QUEUE_SIZE;
    rx_queue.count--;
    xSemaphoreGive(xSemaphoreQueue);
    return 0;
}

int can_queue_size()
{
    int temp;
    xSemaphoreTake(xSemaphoreQueue, portTICK_PERIOD_MS);
    temp = rx_queue.count;
    xSemaphoreGive(xSemaphoreQueue);
    return temp;
}

int mcp25625_queue_available()
{
    int size = can_queue_size();
    return size;
}

int mcp25625_queue_read(MCP25625_canMessage_t *msg)
{
    return can_queue_pop(msg);
}

///////////////////////////////MCP25625 ISR FUNCTIONS/////////////////////////////////////////////////////////////////////////
void IRAM_ATTR _can_isr_handler(void* arg)
{
    xTaskNotifyFromISR(canIsrTaskHandle, 0, eNoAction, NULL);
}

void can_task_interrupt_handler(void* arg)
{
    mcp25625_int_ctl int_flag;
    mcp25625_eflg_ctl err_flag;
    err_flag.reg = 0x2D;
    int_flag.reg = INT_FLG;
    MCP25625_canMessage_t temp;

    while (1)
    {
        // Wait for an interrupt
        xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
        mcp25625_hw_ctl_get(&int_flag);
        mcp25625_hw_ctl_get(&err_flag);
        if(int_flag.rx0 | int_flag.rx1)
        {
            if((can_queue_size() + 1 < MCP25625_CAN_RX_QUEUE_SIZE))
            {
                if(int_flag.rx0)
                {
                    mcp25625_msg_read(RXB0, temp.msg, &(temp.count), &(temp.id), &(temp.IDE), &(temp.RTR));
                }
                else
                {
                    mcp25625_msg_read(RXB1, temp.msg, &(temp.count), &(temp.id), &(temp.IDE), &(temp.RTR));
                }
                can_queue_push(temp);
            }

            else
            {
                ESP_LOGW("CAN_EXT", "Internal RX buffer has not enought space to read values");
            }
        }   
    }
}

/*************** END OF FUNCTIONS *********************************************/