/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file RS.h
 * @brief 
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#if defined(CONFIG_CORE)

#include <stdint.h>
#include "sc16is750/sc16is750.h"

// Driver is specific to OICore, we use CorePinout.h for hadrware gpio number
#include "CorePinout.h"

#define CONFIG_GLOBAL_INSTANCE_RS485

/* SPI Device configuration */
#define OIRS_DEFAULT_SPI_HOST                   CORE_SPI_USER_HOST
#define OIRS_DEFAULT_SPI_PIN_CS                 CORE_PIN_RS_SPI_CS
#define OIRS_DEFAULT_SPI_PIN_INTERRUPT          CORE_PIN_RS_INTERRUPT

/* IO Configuration */
#define OIRS_GPIO_SET                           (SC16IS750_IO0_OUTPUT | SC16IS750_IO1_OUTPUT)
#define OIRS_GPIO_MODE485                       (SC16IS750_IO0 | SC16IS750_IO1)
#define OIRS_GPIO_MODE232                       (0x00)

typedef enum {
    RS_485,
    RS_232
} RS_Mode_t;

/**
 * @brief OIRS class
 * 
 */
class OIRS
{
public:

    /**
     * @brief Construct a new OIRS object
     * 
     * @param mode Select RS mode : RS_485 or RS_232
     */
    inline OIRS(RS_Mode_t mode) {
        _mode = mode;
    }

    /**
     * @brief Initialize the RS communication
     * @note You must use a standard baudrate: [9600; 57600; 115200; 921600]
     * 
     * @param baudrate in bits per second (baud)
     * @param host the spi type you would like to use
     * @param cs the pin you would like to use
     * @param interrupt the pin you would like to use
     */
    void begin(unsigned long baudrate = 115200, spi_host_device_t host = OIRS_DEFAULT_SPI_HOST, gpio_num_t cs = OIRS_DEFAULT_SPI_PIN_CS, gpio_num_t interrupt = OIRS_DEFAULT_SPI_PIN_INTERRUPT);

    /**
     * @brief Disables RS communication
     * 
     */
    void end(void);

    /**
     * @brief Get the number of bytes available for reading
     * 
     * @return the number of bytes available to read
     */
    int available(void);

    /**
     * @brief Get the number of bytes available for writing in the serial buffer 
     * without blocking the write operation.
     * 
     * @return The number of bytes available to write.
     */
    int availableForWrite(void);

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

    inline size_t read(char * buffer, size_t size)
    {
        return read((uint8_t*) buffer, size);
    }

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
     * @param data an array to send as a series of bytes.
     * @param len the number of bytes to be sent from the array.
     * @return The number of bytes written.
     */
    size_t write(uint8_t* data, size_t len);

    inline size_t write(const char * buffer, size_t size)
    {
        return write((uint8_t*) buffer, size);
    }
    inline size_t write(const char * s)
    {
        return write((uint8_t*) s, strlen(s));
    }
    inline size_t write(unsigned long n)
    {
        return write((uint8_t) n);
    }
    inline size_t write(long n)
    {
        return write((uint8_t) n);
    }
    inline size_t write(unsigned int n)
    {
        return write((uint8_t) n);
    }
    inline size_t write(int n)
    {
        return write((uint8_t) n);
    }

private:

    RS_Mode_t _mode;
    
};

#if defined CONFIG_GLOBAL_INSTANCE_RS485
extern OIRS RS485;
#elif defined CONFIG_GLOBAL_INSTANCE_RS232
extern OIRS RS232;
#endif

#endif