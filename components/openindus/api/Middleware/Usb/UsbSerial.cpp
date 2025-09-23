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
        .source_clk = UART_SCLK_APB,
        .flags = {
            .allow_pd = 0,
            .backup_before_sleep = 0
        }
    };

    if (uart_is_driver_installed(UART_NUM_0)) {
        uart_driver_delete(UART_NUM_0);
    }

    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0, 1024, 1024, 20, &_eventQueue, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_0, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_0, GPIO_NUM_43, GPIO_NUM_44, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
}

void UsbSerial::setBaudrate(int baudrate)
{
    uart_wait_tx_done(UART_NUM_0, pdMS_TO_TICKS(5));
    uart_set_baudrate(UART_NUM_0, baudrate);
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

void UsbSerialProtocol::setBaudrate(int baudrate)
{
    UsbSerial::setBaudrate(baudrate);
}

void _slipDataUnframing(uint8_t* buffer, size_t* length)
{
    for (int i=0; i<*length-1; i++) {
        if (buffer[i] == 0XDB) {
            if (buffer[i+1] == 0xDC) {
                buffer[i] = 0xC0;
                *length = *length-1;
                memmove(&buffer[i+1], &buffer[i+2], *length-i);
            } else if (buffer[i+1] == 0xDD) {
                buffer[i] = 0xDB;
                *length = *length-1;
                memmove(&buffer[i+1], &buffer[i+2], *length-i);
            }
        }
    }
}

int UsbSerialProtocol::read(Packet_t* packet, TickType_t timeout)
{
    uart_event_t event;
    uint8_t buffer[1024];
    size_t length = 0;
    int index = 0;
    bool slip_end_received = false;
    while (1) {
        if (xQueueReceive(_eventQueue, (void*)&event, timeout) == pdTRUE) {
            if (event.type == UART_DATA) {
                if (event.size > 0 && event.size <= 1024) {
                    length = uart_read_bytes(UART_NUM_0, buffer, event.size, 1000/portTICK_PERIOD_MS);
                    if (buffer[length-1] == 0xC0) {  // SLIP data end
                        length -= 1;
                        slip_end_received = true;
                        // Packet was composed of only the end character
                        if (length == 0) {
                            return index;
                        }
                    }
                    if (buffer[0] == 0xC0) { // SLIP data start
                        length -= 1;
                        // Remove slip data
                        _slipDataUnframing(&buffer[1], &length);
                        // Copy 8 bytes of header
                        memcpy(packet, &buffer[1], 8);
                        length -= 8;
                        // Copy remaining data
                        memcpy(&packet->data[index], &buffer[9], length);
                        index += length;
                    } else {
                        // Remove slip data
                        _slipDataUnframing(buffer, &length);
                        if (packet->data[index-1] == 0xDB && buffer[0] == 0xDC) {
                            packet->data[index-1] = 0xC0;
                            length -= 1;
                            memcpy(&packet->data[index], &buffer[1], length);
                        } else if (packet->data[index-1] == 0xDB && buffer[0] == 0xDD) {
                            packet->data[index-1] = 0xDB;
                            length -= 1;
                            memcpy(&packet->data[index], &buffer[1], length);
                        } else {
                            memcpy(&packet->data[index], buffer, length);
                        }
                        index += length;
                    }
                    if (index >= packet->size && slip_end_received) {
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
    
    // SLIP Start
    uart_write_bytes(UART_NUM_0, &pat, sizeof(pat));

    // DIRECTION
    uart_write_bytes(UART_NUM_0, &packet->direction, sizeof(packet->direction));

    // COMMAND
    uart_write_bytes(UART_NUM_0, &packet->command, sizeof(packet->command));


    /* Replace occurrences in size */
    uint8_t size[4];
    uint8_t size_size = 2;
    memcpy(size, &packet->size, 2);

    for (int i = 0; i < size_size; i++) {
        if (size[i] == 0xC0) {
            memmove(&size[i+2], &size[i+1], size_size-i-1);
            memcpy(&size[i], esc1, 2);
            i++;
            size_size++;
        } else if (size[i] == 0xDB) {
            memmove(&size[i+2], &size[i+1], size_size-i-1);
            memcpy(&size[i], esc2, 2);
            i++;
            size_size++;
        }
    }

    // SIZE
    uart_write_bytes(UART_NUM_0, size, size_size);


    /* Replace occurrences in value */
    uint8_t value[8];
    uint8_t value_size = 4;
    memcpy(value, &packet->value, 4);

    for (int i = 0; i < value_size; i++) {
        if (value[i] == 0xC0) {
            memmove(&value[i+2], &value[i+1], value_size-i-1);
            memcpy(&value[i], esc1, 2);
            i++;
            value_size++;
        } else if (value[i] == 0xDB) {
            memmove(&value[i+2], &value[i+1], value_size-i-1);
            memcpy(&value[i], esc2, 2);
            i++;
            value_size++;
        }
    }

    // VALUE
    uart_write_bytes(UART_NUM_0, value, value_size);


    /* Replace occurrences in data */
    for (int i = 0; i < packet->size; i++) {
        if (packet->data[i] == 0xC0) {
            memmove(&packet->data[i+2], &packet->data[i+1], packet->size-i-1);
            memcpy(&packet->data[i], esc1, 2);
            i++;
            packet->size++;
        } else if (packet->data[i] == 0xDB) {
            memmove(&packet->data[i+2], &packet->data[i+1], packet->size-i-1);
            memcpy(&packet->data[i], esc2, 2);
            i++;
            packet->size++;
        }
    }

    // DATA
    uart_write_bytes(UART_NUM_0, packet->data, packet->size);

    // SLIP End
    uart_write_bytes(UART_NUM_0, &pat, sizeof(pat));
}
