/**
 * @file RS.h
 * @brief RS485/232 bus
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "RS.h"

/**
 * @brief Initialize the RS communication
 * @note You must use a standard baudrate: [9600; 57600; 115200; 921600]
 * 
 * @param mode RS_485 or RS_232
 * @param baudrate in bits per second (baud)
 * @param config 8N1 by default
 */
void RS::begin(RS_Mode_t mode, unsigned long baudrate, uint16_t config)
{
    _mode = mode;
    
    SC16IS750_DeviceConfig_t device_config;
    device_config.spi_host = _spi_host;
    device_config.spi_pin_cs = _pin_cs;
    device_config.spi_freq = SC16IS750_SPI_FREQ;
    device_config.spi_pin_int = _pin_intr;

    SC16IS750_create(&device_config); 

    SC16IS750_SerialParams_t conf;
    
    conf.baudrate = baudrate;
    conf.flow_type = Disabled;
    conf.format_bits = (((config >> 2) & 0x03)+5);
    conf.format_stop_bits = (((config >> 5) & 0x01)+1); 
    uint8_t parity = (config & 0x03);
    conf.parity = ((parity == 0x00) ? None : (parity == 0x11) ? Odd : Even);
    conf.trigger_halt = TCR_HALT_DEFAULT;
    conf.trigger_resume = TCR_RESUME_DEFAULT;

    SC16IS750_init(conf);

    SC16IS750_setAutoRts();

    SC16IS750_ioSetDirection(SC16IS750_IO0_OUTPUT | SC16IS750_IO1_OUTPUT);

    if (_mode == RS_485) {
        SC16IS750_ioSetState(SC16IS750_IO0 | SC16IS750_IO1);
    }
    else if (_mode == RS_232) {
        SC16IS750_ioSetState(0x00);
    }
    
    
}

/**
 * @brief Disables RS communication
 * 
 */
void RS::end(void)
{
    SC16IS750_delete();
}

/**
 * @brief Get the number of bytes available for reading
 * 
 * @return the number of bytes available to read
 */
int RS::available(void)
{
    return SC16IS750_queue_available();
}

/**
 * @brief Get the number of bytes available for writing in the serial buffer 
 * without blocking the write operation.
 * 
 * @return The number of bytes available to write.
 */
int RS::availableForWrite(void)
{
    /** @todo */
    return -1;
}

/**
 * @brief Reads incoming serial data.
 * 
 * @return The first byte of incoming serial data available (or -1 if no data is available)
 */
int RS::read(void)
{
    return SC16IS750_queue_read();
}

/**
 * @brief Reads bytes from the serial port into a buffer. 
 * 
 * @param buffer the buffer to store the bytes in. Allowed data types: array of char or byte.
 * @param size the number of bytes to read. Allowed data types: int.
 * @return The number of bytes placed in the buffer.
 */
size_t RS::read(uint8_t *buffer, size_t size)
{
    SC16IS750_queue_read_bytes(buffer, size);
    return size;
}

/**
 * @brief Writes binary data to the serial port.
 * 
 * @param byte byte to send
 * @return The number of bytes written.
 */
size_t RS::write(uint8_t byte)
{
    SC16IS750_writeBytes(&byte, 1);
    return 1;
}

/**
 * @brief Writes binary data to the serial port.
 * 
 * @param data an array to send as a series of bytes.
 * @param len the number of bytes to be sent from the array.
 * @return The number of bytes written.
 */
size_t RS::write(uint8_t* data, size_t len)
{
    SC16IS750_writeBytes(data, len);
    return len;
}