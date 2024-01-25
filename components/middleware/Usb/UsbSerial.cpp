/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file UsbSerial.c
 * @brief OpenIndus USB Serial
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "UsbSerial.h"

static const char USB_SERIAL_TAG[] = "Usb Serial";

QueueHandle_t UsbSerial::_eventQueue;

void UsbSerial::begin(int baudrate)
{
    ESP_LOGI(USB_SERIAL_TAG, "Configuring uart");
    uart_config_t uart_config = {
        .baud_rate = baudrate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0,
        .source_clk = UART_SCLK_APB
    };

    if (uart_is_driver_installed(UART_NUM_0)) {
        uart_driver_delete(UART_NUM_0);
    }

    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0, 1024, 1024, 20, &_eventQueue, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_0, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_0, GPIO_NUM_1, GPIO_NUM_3, 
        UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
}

void UsbSerial::read(void* buf, uint32_t length, TickType_t timeout)
{
    uart_read_bytes(UART_NUM_0, buf, length, timeout);
}

void UsbSerial::write(const void* src, size_t size)
{
    uart_write_bytes(UART_NUM_0, src, size);
}

void UsbSerial::flush(void)
{
    uart_flush_input(UART_NUM_0);
}

void UsbSerialProtocol::begin(int baudrate)
{
    UsbSerial::begin(baudrate);
}

void _slipDataUnframing(uint8_t* buffer, size_t* length)
{
    for (int i=0; i<*length-1; i++) {
        if (buffer[i] == 0xDB && buffer[i+1] == 0xDC) {
            buffer[i] = 0xC0;
            *length = *length-1;
            memmove(&buffer[i+1], &buffer[i+2], *length-i); 
        } else if (buffer[i] == 0xDB && buffer[i+1] == 0xDD) {
            buffer[i] = 0xDB;
            *length = *length-1;
            memmove(&buffer[i+1], &buffer[i+2], *length-i); 
        }
    }
}

int UsbSerialProtocol::read(Packet_t* packet, TickType_t timeout)
{
    uart_event_t event;
    uint8_t buffer[1024];
    size_t length = 0;
    int index = 0;
    while (1) {
        if (xQueueReceive(_eventQueue, (void*)&event, timeout) == pdTRUE) {
            if (event.type == UART_DATA) {
                if (event.size > 0 && event.size <= 1024) {
                    memset(buffer, 0, 1024);
                    length = event.size;
                    uart_read_bytes(UART_NUM_0, buffer, length, 1000/portTICK_PERIOD_MS);
                    if (buffer[length-1] == 0xC0) {  // SLIP data end
                        length -= 1;
                    }
                    if (buffer[0] == 0xC0) { // SLIP data start
                        length -= 1;
                        memmove(buffer, &buffer[1], length);
                        _slipDataUnframing(buffer, &length);
                        memcpy(packet, buffer, 8);
                        length -= 8;
                        memcpy(&packet->data[index], &buffer[8], length);
                        index += length;
                    } else {
                        _slipDataUnframing(buffer, &length);
                        if (packet->data[index-1] == 0xDB && buffer[0] == 0xDC) {
                            packet->data[index-1] = 0xC0;
                            length -= 1;
                            memmove(buffer, &buffer[1], length);
                        } else if (packet->data[index-1] == 0xDB && buffer[0] == 0xDD) {
                            packet->data[index-1] = 0xDB;
                            length -= 1;
                            memmove(buffer, &buffer[1], length);
                        }
                        memcpy(&packet->data[index], buffer, length);
                        index += length;
                    }
                    if (index >= packet->size) {
                        return index;
                    }
                } else {
                    return -3;
                }
            } else {
                uart_flush_input(UART_NUM_0);
                xQueueReset(_eventQueue);
                return -2;
            }
        } else {
            return -1;
        } 
    }
}

void UsbSerialProtocol::write(Packet_t* packet)
{
    const uint8_t pat = 0xC0;
    const uint8_t esc1[2] = {0xDB, 0xDC};
    const uint8_t esc2[2] = {0xDB, 0xDD};
    uart_write_bytes(UART_NUM_0, &pat, sizeof(pat));
    uart_write_bytes(UART_NUM_0, packet, 8);
    /* Replace occurrences */
    for (int i=0; i<packet->size; i++) {
        if (packet->data[i] == 0xC0) {
            memmove(&packet->data[i+2], &packet->data[i+1], packet->size-i-1);
            memcpy(&packet->data[i], esc1, 2);
            i += 2;
            packet->size += 1;
        } else if (packet->data[i] == 0xDB) {
            memmove(&packet->data[i+2], &packet->data[i+1], packet->size-i-1);
            memcpy(&packet->data[i], esc2, 2);
            i += 2;
            packet->size += 1;  
        }
    }
    uart_write_bytes(UART_NUM_0, packet->data, packet->size);
    uart_write_bytes(UART_NUM_0, &pat, sizeof(pat));
}
