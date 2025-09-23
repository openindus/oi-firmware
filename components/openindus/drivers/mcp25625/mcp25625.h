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

#ifndef _MCP25625_
#define _MCP25625_

#pragma once

#include <string.h>

#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#ifdef __cplusplus
extern "C" {
#endif

#define __MIKROC_PRO_FOR_ESP32__       1
#define MCP25625_SPI_FREQ              4000000UL
#define MCP25625_CAN_RX_QUEUE_SIZE     64

#define MCP25625_SPI_CMD_WRITE_REG              0x02
#define MCP25625_SPI_CMD_READ_REG               0x03
#define MCP25625_SPI_CMD_MODIFY_REG             0x05

/**CAN Driver device configuration.
 *
 *  
 */
typedef struct
{
    spi_host_device_t spi_host;
    int spi_freq;
    int spi_pin_cs;
    gpio_num_t spi_pin_int;

} MCP25625_DeviceConfig_t;

/**CAN message structure.
 *
 *  
 */
typedef struct
{
    uint32_t id;
    uint8_t count;
    bool IDE;
    bool RTR;
    uint8_t msg[ 8 ];
} MCP25625_canMessage_t;

/**CAN queue message.
 *
 *  
 */
typedef struct 
{
    uint16_t head, tail, count;
    MCP25625_canMessage_t e[MCP25625_CAN_RX_QUEUE_SIZE];
} MCP25625_canRxQueue_t;

/**Driver install function.

 *
 *   @return ESP_OK if good result
 */
esp_err_t mcp25625_create(MCP25625_DeviceConfig_t *config);


/**Driver delete function.

 *
 *   @return ESP_OK if good result
 */
esp_err_t mcp25625_delete();

/**SPI Write function.

 *   @params cmd : spi cmd, reg : spi register, data_out : tx data, len : data len
 *   @return none
 */
void SPI_Wr_Ptr( unsigned char cmd, unsigned char reg , unsigned char *data_out, int len );

/**SPI Read function.

 *   @params cmd : spi cmd, reg : spi register, buffer : rx data buffer, len : data len
 *   @return none
 */
void SPI_Rd_Ptr( unsigned char cmd, unsigned char reg, unsigned char * buffer,  int len );

/**Read one CAN message of queue.

 *   
 *   @return 0 if queue is empty
 */
int mcp25625_queue_read(MCP25625_canMessage_t *msg);

/**Return number of elements in queue.

 *   
 *   @return 0 if queue is full
 */
int mcp25625_queue_available();
/**Push one element in queue.

 *   
 *   @return -1 if queue is full
 */
int can_queue_push(MCP25625_canMessage_t element);

/**Pop one element of queue.

 *
 *   @return -1 if queue is empty
 */
int can_queue_pop(MCP25625_canMessage_t *element);

/**Return number of elements in queue.

 *
 *   @return 0 if queue is empty
 */
int can_queue_size();

/**ISR GPIO Handler.

 *
 *   @return none
 */
void _can_isr_handler(void* arg);

/**ISR GPIO Task.

 *
 *   @return none
 */
void can_task_interrupt_handler(void* arg);

#ifdef __cplusplus
}
#endif

#endif