/**
 * @file RS.h
 * @brief RS485/232 bus
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include <stdint.h>
#include "sc16is750/sc16is750.h"

/**
 * @brief RS485/232 bus class
 */
class RS
{
public:
    /**
     * @brief RS communication mode
     */
    enum Mode {
        RS485,
        RS232
    };

    /**
     * @brief RS communication configuration
     * Format: [Data bits] + [Parity] + [Stop bits]
     * Parity: N=None, E=Even, O=Odd
     * Exemple: 8N1 = 8 data bits, No parity, 1 stop bit
     */
    enum Config : uint16_t {
        _8N1 = 0x001c,
        _8E1 = 0x0034,
        _8O1 = 0x0024,
        _7N1 = 0x000c,
        _7E1 = 0x0014,
        _7O1 = 0x0004
    };

    /**
     * @brief Constructor
     * 
     * @param host SPI host device
     * @param cs Chip select pin
     * @param intr Interrupt pin
     */
    RS(spi_host_device_t host, gpio_num_t cs, gpio_num_t intr) :
        _spiHost(host),
        _csPin(cs),
        _intrPin(intr) {}
    
    /**
     * @brief Destructor
     */
    ~RS() {}

    /**
     * @brief Initializes RS communication
     * @param baudrate in bits per second (baud)
     * @param config 8N1 by default
     * @param mode RS485 or RS232
     */
    void begin(uint32_t baudrate = 115200, Config config = _8N1, Mode mode = RS485);

    /**
     * @brief Disables RS communication
     */
    void end(void);

    /**
     * @brief Get the number of bytes available for reading from the serial buffer.
     * 
     * @return The number of bytes available to read.
     */
    int available(void);

    /**
     * @brief Reads incoming serial data.
     * 
     * @return The first byte of incoming serial data available (or -1 if no data is available)
     */
    int read(void);

    /**
     * @brief Reads bytes from the serial port into a buffer.  
     * 
     * @param buffer the buffer to store the bytes in. Allowed data types: array of char or byte.
     * @param size the number of bytes to read. Allowed data types: int.
     * @return The number of bytes placed in the buffer.
     */
    size_t read(uint8_t *buffer, size_t size);

    /**
     * @brief Writes binary data to the serial port.
     * 
     * @param byte byte to send
     * @return The number of bytes written.
     */
    size_t write(uint8_t byte);

    /**
     * @brief Writes binary data to the serial port.
     * 
     * @param buffer an array to send as a series of bytes.
     * @param size the number of bytes to be sent from the array.
     * @return The number of bytes written.
     */
    size_t write(const uint8_t* buffer, size_t size);

private:
    spi_host_device_t _spiHost;
    gpio_num_t _csPin;
    gpio_num_t _intrPin;

};