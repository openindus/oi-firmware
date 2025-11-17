/**
 * @file RS.h
 * @brief RS485/232 bus
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "RS.h"

void RS::begin(unsigned long baudrate, Config config, Mode mode)
{    
    SC16IS750_DeviceConfig_t deviceConfig = {
        .spi_host = _spiHost,
        .spi_freq = SC16IS750_SPI_FREQ,
        .spi_pin_cs = _csPin,
        .spi_pin_int = _intrPin
    };

    SC16IS750_create(&deviceConfig); 

    SC16IS750_SerialParams_t serialConfig = {
        .baudrate = static_cast<int>(baudrate),
        .format_bits = (((config >> 2) & 0x03) + 5),
        .parity = ((config & 0x03) == 0x00) ? None : 
                  ((config & 0x03) == 0x11) ? Odd : Even,
        .format_stop_bits = (((config >> 5) & 0x01) + 1),
        .flow_type = Disabled,
        .trigger_resume = TCR_RESUME_DEFAULT,
        .trigger_halt = TCR_HALT_DEFAULT,
    };

    SC16IS750_init(serialConfig);
    SC16IS750_setAutoRts();
    SC16IS750_ioSetDirection(SC16IS750_IO0_OUTPUT | SC16IS750_IO1_OUTPUT);

    if (mode == RS485) {
        SC16IS750_ioSetState(SC16IS750_IO0 | SC16IS750_IO1);
    } else if (mode == RS232) {
        SC16IS750_ioSetState(0);
    } else {
        // Invalid mode
    }
}

void RS::end(void)
{
    SC16IS750_delete();
}

int RS::available(void)
{
    return SC16IS750_queue_available();
}

int RS::read(void)
{
    return SC16IS750_queue_read();
}

size_t RS::read(uint8_t *buffer, size_t size)
{
    SC16IS750_queue_read_bytes(buffer, size);
    return size;
}

size_t RS::write(uint8_t byte)
{
    SC16IS750_writeBytes(&byte, 1);
    return 1;
}

size_t RS::write(const uint8_t* buffer, size_t size)
{
    uint8_t buf[size];
    memcpy(buf, buffer, size);
    SC16IS750_writeBytes(buf, size);
    return size;
}