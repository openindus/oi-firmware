#if defined(CONFIG_CORE)

#include "RS.h"

void OIRS::begin(unsigned long baudrate, spi_host_device_t host, gpio_num_t cs, gpio_num_t interrupt)
{
    SC16IS750_DeviceConfig_t device_config;
    device_config.spi_host = host;
    device_config.spi_pin_cs = cs;
    device_config.spi_freq = SC16IS750_SPI_FREQ;
    device_config.spi_pin_int = interrupt;

    SC16IS750_create(&device_config); 

    SC16IS750_SerialParams_t conf;
    conf.baudrate = baudrate;
    conf.flow_type = Disabled;
    conf.format_bits = 8;
    conf.format_stop_bits = 1;
    conf.parity = None;
    conf.trigger_halt = TCR_HALT_DEFAULT;
    conf.trigger_resume = TCR_RESUME_DEFAULT;

    SC16IS750_ioSetDirection(OIRS_GPIO_SET);

    if (_mode == RS_485) {
        SC16IS750_ioSetState(OIRS_GPIO_MODE485);
    }
    else if (_mode == RS_232) {
        SC16IS750_ioSetState(OIRS_GPIO_MODE232);
    }
    
    SC16IS750_setAutoRts();
    SC16IS750_init(conf);
}

void OIRS::end(void)
{
    SC16IS750_delete();
}

int OIRS::available(void)
{
    return SC16IS750_queue_available();
}

int OIRS::availableForWrite(void)
{
    /** @todo */
    return -1;
}

int OIRS::read(void)
{
    return SC16IS750_queue_read();
}

size_t OIRS::read(uint8_t *buffer, size_t size)
{
    SC16IS750_queue_read_bytes(buffer, size);
    return size;
}

size_t OIRS::write(uint8_t byte)
{
    SC16IS750_writeBytes(&byte, 1);
    return 1;
}

size_t OIRS::write(uint8_t* data, size_t len)
{
    SC16IS750_writeBytes(data, len);
    return len;
}

#if defined CONFIG_GLOBAL_INSTANCE_RS485
OIRS RS485(RS_485);
#elif defined CONFIG_GLOBAL_INSTANCE_RS232
OIRS RS232(RS_232);
#endif

#endif