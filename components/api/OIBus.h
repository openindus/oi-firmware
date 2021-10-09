/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file OIBus.h
 * @brief this class control the bus
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include <stdint.h>
#include <map>

#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "driver/twai.h"

#include "OIMessage.h"
#include "OIProtocol.h"

/* OIBUS */
#define OI_BUS_MAX_RETRY             (3U)
#define OI_BUS_CMD_SET_TIMEOUT       (50U)
#define OI_BUS_CMD_GET_TIMEOUT       (50U)


/* RS485 */
#define OI_BUS_RS_NUM_PORT                   UART_NUM_1
#define OI_BUS_RS_SPEED                      (921600UL)
#define OI_BUS_RS_DATA_BIT                   UART_DATA_8_BITS
#define OI_BUS_RS_PARITY                     UART_PARITY_DISABLE
#define OI_BUS_RS_STOP_BITS                  UART_STOP_BITS_1
#define OI_BUS_RS_FLOW_CONTROL               UART_HW_FLOWCTRL_DISABLE
#define OI_BUS_RS_RX_FLOW_CTRL_THRESH        (122U)
#define OI_BUS_RS_SOURCE_CLK                 UART_SCLK_APB
#define OI_BUS_RS_DEFAULT_BUF_SIZE           (1024U)
#define OI_BUS_RS_DEFAULT_QUEUE_SIZE         (20U)
#define OI_BUS_RS_DEFAULT_TIMEOUT            (10U)

#define OI_BUS_RS_SYNC_BYTE         (0xAA)
#define OI_BUS_RS_TRAME_SIZE        (14U)
#define OI_BUS_RS_HEADER_LENGTH     (4U)
#define OI_BUS_RS_DATA_LENGTH       (8U)

/* TWAI */
#define OI_BUS_TWAI_MODE                     TWAI_MODE_NORMAL
#define OI_BUS_TWAI_DEFAULT_TIMEOUT          (10000U)


/* Pinout */

#ifdef CONFIG_IDF_TARGET_ESP32S2
#define OI_BUS_PIN_CAN_RX               GPIO_NUM_4
#define OI_BUS_PIN_CAN_TX               GPIO_NUM_5
#define OI_BUS_PIN_RS_UART_RX           GPIO_NUM_18
#define OI_BUS_PIN_RS_UART_TX           GPIO_NUM_17
#else
#define OI_BUS_PIN_CAN_RX               GPIO_NUM_4
#define OI_BUS_PIN_CAN_TX               GPIO_NUM_5
#define OI_BUS_PIN_RS_UART_RX           GPIO_NUM_16
#define OI_BUS_PIN_RS_UART_TX           GPIO_NUM_17
#endif

class OIBus
{
public:

    /**
     * @brief Initatialization of OIBus
     * 
     */
    OIBus()
    {
        messageErrorCounter = 0;
        // Create semaphore for shared ressources
        xMessageErrorCounterMutex = xSemaphoreCreateMutex();
        xSemaphoreGive(xMessageErrorCounterMutex);

        xMessageWriteMutex = xSemaphoreCreateMutex();
        xSemaphoreGive(xMessageWriteMutex);

        xMessageReadMutex = xSemaphoreCreateMutex();
        xSemaphoreGive(xMessageReadMutex);
    }

    /**
     * @brief 
     * 
     * @param msg 
     * @param id 
     */
    inline void setMessage(OIMessage const& msg, uint8_t id)
    {
        OIMessage returnMsg;
        uint8_t returnId;

        if (id == BROADCAST_ID)
        {
            xSemaphoreTake(xMessageWriteMutex, portMAX_DELAY);
            writeMessage(msg, id);
            xSemaphoreGive(xMessageWriteMutex);
        }
        else
        {
            for (int i(0); i<OI_BUS_MAX_RETRY; ++i)
            {
                xSemaphoreTake(xMessageWriteMutex, portMAX_DELAY);
                writeMessage(msg, id);

                if (readMessage(returnMsg, returnId, pdMS_TO_TICKS(OI_BUS_CMD_SET_TIMEOUT)) != -1)
                {
                    if ((returnId == msg.getId()) && (returnMsg.getId() == id) && 
                        (returnMsg == msg))
                    {
                        xSemaphoreGive(xMessageWriteMutex);
                        return;
                    }
                }
                else
                {
                    if(xSemaphoreTake(xMessageErrorCounterMutex, 10) == pdTRUE)
                    {
                        messageErrorCounter += 1;
                        xSemaphoreGive(xMessageErrorCounterMutex);
                    }
                }
                xSemaphoreGive(xMessageWriteMutex);
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }
        }
    }

    /**
     * @brief 
     * 
     * @param msg 
     * @param id 
     * @return uint32_t 
     */
    inline uint32_t getMessage(OIMessage const& msg, uint8_t id)
    {
        OIMessage returnMsg;
        uint8_t returnId;

        if (id == BROADCAST_ID)
        {
            xSemaphoreTake(xMessageWriteMutex, portMAX_DELAY);
            writeMessage(msg, id);
            xSemaphoreGive(xMessageWriteMutex);
        }
        else
        {
            for (int i(0); i<OI_BUS_MAX_RETRY; ++i)
            {
                xSemaphoreTake(xMessageWriteMutex, portMAX_DELAY);
                writeMessage(msg, id);

                if (readMessage(returnMsg, returnId, pdMS_TO_TICKS(OI_BUS_CMD_GET_TIMEOUT)) != -1)
                {
                    if ((returnId == msg.getId()) && (returnMsg.getId() == id) && 
                        (returnMsg == msg))
                    {
                        xSemaphoreGive(xMessageWriteMutex);
                        return returnMsg.getData();
                    }
                }
                else
                {
                    if(xSemaphoreTake(xMessageErrorCounterMutex, 10) == pdTRUE)
                    {
                        messageErrorCounter += 1;
                        xSemaphoreGive(xMessageErrorCounterMutex);
                    }
                }
                xSemaphoreGive(xMessageWriteMutex);
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }   
        }
        return 0;
    }

    /**
     * @brief 
     * 
     * @param msg 
     * @param id 
     */
    inline void sendMessage(OIMessage const& msg, uint8_t id)
    {
        xSemaphoreTake(xMessageWriteMutex, portMAX_DELAY);
        writeMessage(msg, id);
        xSemaphoreGive(xMessageWriteMutex);
    }

    /**
     * @brief 
     * 
     * @param msg 
     * @param id 
     * @param timeout 
     * @return int 
     */
    inline int receiveMessage(OIMessage& msg, uint8_t& id, TickType_t timeout=portMAX_DELAY)
    {
        xSemaphoreTake(xMessageReadMutex, portMAX_DELAY);
        int ret = readMessage(msg, id, timeout);
        xSemaphoreGive(xMessageReadMutex);

        if (ret == -1)
        {
            if(xSemaphoreTake(xMessageErrorCounterMutex, 10) == pdTRUE)
            {
                messageErrorCounter++;
                xSemaphoreGive(xMessageErrorCounterMutex);
            }
        }

        return ret;
    }

    /**
     * @brief return the number of message errors
     * 
     * @return int
     */
    inline int getMessageErrorCounter(void)
    {
        xSemaphoreTake(xMessageErrorCounterMutex, portMAX_DELAY);
        int ret = messageErrorCounter;
        xSemaphoreGive(xMessageErrorCounterMutex);
        return ret;
    }

protected:

    /**
     * @brief write message
     * 
     * @param msg message to write
     * @param id destination id
     */
    virtual void writeMessage(OIMessage const& msg, uint8_t id)=0;

    /**
     * @brief read message 
     * 
     * @param msg message to read
     * @param id destination id
     * @param timeout 
     * @return int 
     */
    virtual int readMessage(OIMessage& msg, uint8_t& id, TickType_t timeout=portMAX_DELAY)=0;

    int messageErrorCounter;
    SemaphoreHandle_t xMessageErrorCounterMutex;
    SemaphoreHandle_t xMessageWriteMutex;
    SemaphoreHandle_t xMessageReadMutex;
};

class OIBusRS : public OIBus
{

public:

    /**
     * @brief initialization of RS communication
     */
    void init(void);

    /**
     * @brief deinitialization of RS communication
     */
    void deinit(void);

    /**
     * @brief Flush RS RX buffer
     */
    void flush(void);

    /**
     * @brief Reset RS event queue
     */
    void resetEventQueue(void);

    /**
     * @brief Encode message and send  it to the RS bus
     * @param OIMessage message defined in OIProtocole
     * @param uint8_t receiver id
     */
	void writeMessage(OIMessage const& msg, uint8_t id) override;

    /**
     * @brief Receive RS message and decode it
     * @param OIMessage message defined in OIProtocole
     * @param uint8_t sender id
     * @return int 0 success or -1 error
     */
    int readMessage(OIMessage& msg, uint8_t& id, TickType_t timeout=portMAX_DELAY) override;

    /**
     * @brief wait for the return message
     */
    inline static void setWaitAck(bool ack) {
        _waitAck = ack;
    }

    /**
     * @brief wait for the return message
     */
    inline static bool getWaitAck(void) {
        return _waitAck;
    }

private:

    static bool _isInitialized;
    static bool _waitAck;

    static QueueHandle_t _uartEventQueueHandle;

    /**
     * @brief checksum
     * @param uint8_t pointer to buffer
     * @param size size of the buffer
     * @return uint8_t crc calculated
     */
    static uint8_t _crc(uint8_t *data, uint8_t size);
};


class OIBusTWAI : public OIBus
{

public:

    /**
     * @brief initialization of CAN communication
     */
    void init(void);

    /**
     * @brief deinitialization of CAN communication
     */
    void deinit(void);

    /**
     * @brief Send message on the CAN bus
     * @param  OIMessage message defined in OIProtocole
     * @param uint8_t receiver id
     * @param bool flag to indicate if a ack is needed in return
     */
	void writeMessage(OIMessage const& msg, uint8_t id) override;

    /**
     * @brief Receive CAN message 
     * @param  OIMessage message defined in OIProtocole
     * @param uint8_t sender id
     * @return int 0 success or -1 error
     */
    int readMessage(OIMessage& msg, uint8_t& id, TickType_t timeout=portMAX_DELAY) override;

private:

    static bool _isInitialized;
};

extern OIBusRS BusRS;
extern OIBusTWAI BusTWAI;