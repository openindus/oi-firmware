#include "dac8760.h"

static const char DAC8760_TAG[] = "Dac8760";

static Dac8760_DeviceConfig_t* _deviceConfig = NULL;
static spi_device_handle_t _spiHandler[DAC8760_MAX_DEVICES] = {NULL, NULL};

static bool _spiInitialized = false;
static bool _deviceConfigured = false;

void Dac8760_DeviceConfig(Dac8760_DeviceConfig_t *config)
{
    ESP_LOGI(DAC8760_TAG, "Configure DAC8760 devices");

    _deviceConfig = config;
    _deviceConfigured = true;
}

void Dac8760_Init(void)
{
    uint8_t i = 0;

    Dac8760_SpiInit();

    for(i = 0; i < _deviceConfig->nb_devices; i++)
    {
        // Software reset
        Dac8760_SpiWriteRegister(i, DAC8760_ADDR_RESET, 0x01);
        // Set data to 0
        Dac8760_SpiWriteRegister(i, DAC8760_ADDR_DATA, 0x00);

        _deviceConfig->conf[i].mode = DAC8760_MODE_UNDEFINED;
        _deviceConfig->conf[i].min = 0;
        _deviceConfig->conf[i].max = 0;
        _deviceConfig->conf[i].res = 65535;
    }
}

void Dac8760_SpiInit(void)
{
    if (_spiInitialized == false)
    {

        uint8_t i = 0;

        if (_deviceConfigured == true)
        {
            assert(_deviceConfig != NULL);

            for(i = 0; i < _deviceConfig->nb_devices; i++)
            {
                spi_device_interface_config_t devConfig = {
                    .command_bits = 0,
                    .address_bits = 0,
                    .dummy_bits = 0,
                    .mode = 0,
                    .duty_cycle_pos = 0,
                    .cs_ena_pretrans = 0,
                    .cs_ena_posttrans = 0,
                    .clock_speed_hz = _deviceConfig->spi_freq,
                    .input_delay_ns = 0,
                    .spics_io_num = _deviceConfig->spi_pin_cs[i],
                    .flags = 0,
                    .queue_size = 1,
                    .pre_cb = NULL,
                    .post_cb = NULL
                };

                ESP_ERROR_CHECK(spi_bus_add_device(_deviceConfig->spi_host, &devConfig, &_spiHandler[i]));
            }
            _spiInitialized = true;
            
        }
        else
        {
            ESP_LOGE(DAC8760_TAG, "Device is not configured !!!");
        }
    }
}


void Dac8760_setVoltageMode(uint8_t deviceId, Dac8760_VoltageMode_t mode)
{
    // Configure min and max values
    if (mode == DAC8760_VOLTAGE_RANGE_0_5_V)
    {
        _deviceConfig->conf[deviceId].min = 0;
        _deviceConfig->conf[deviceId].max = 5;
    }
    else if (mode == DAC8760_VOLTAGE_RANGE_0_10_V)
    {
        _deviceConfig->conf[deviceId].min = 0;
        _deviceConfig->conf[deviceId].max = 10;       
    }
    else if (mode == DAC8760_VOLTAGE_RANGE_N5_5_V)
    {
        _deviceConfig->conf[deviceId].min = -5;
        _deviceConfig->conf[deviceId].max = 5;         
    }
    else if (mode == DAC8760_VOLTAGE_RANGE_N10_10_V)
    {
        _deviceConfig->conf[deviceId].min = -10;
        _deviceConfig->conf[deviceId].max = 10;          
    }
    else
    {
        ESP_LOGE(DAC8760_TAG, "Invalid voltage range !!!");
        return;
    }
    
    _deviceConfig->conf[deviceId].mode = mode;

    // Set data to 0
    Dac8760_SpiWriteRegister(deviceId, DAC8760_ADDR_DATA, 0x00);
    // Configure mode    
    Dac8760_SpiWriteRegister(deviceId, DAC8760_ADDR_CTRL, (_deviceConfig->conf[deviceId].mode << DAC8760_CTRL_RANGE) | (1 << DAC8760_CTRL_OUTEN));
}

Dac8760_VoltageMode_t Dac8760_getVoltageMode(uint8_t deviceId)
{
    if (deviceId < _deviceConfig->nb_devices)
    {
        return (Dac8760_VoltageMode_t) _deviceConfig->conf[deviceId].mode;
    }
    else
    {
        ESP_LOGE(DAC8760_TAG, "Invalid device id !!!");   
        return (Dac8760_VoltageMode_t) DAC8760_VOLTAGE_RANGE_INVALID; 
    }
}

void Dac8760_VoltageWrite(uint8_t deviceId, float value, Dac8760_Units_t units)
{
    uint32_t raw_value = 0;

    if (deviceId < _deviceConfig->nb_devices)
    {
        // Compute raw value according to units
        if (units == DAC8760_UNITS_RAW)
        {
            raw_value = (uint32_t)value;
        }
        else if (units == DAC8760_UNITS_MILLIVOLTS)
        {
            raw_value = (((value / 1000.0) - _deviceConfig->conf[deviceId].min) * _deviceConfig->conf[deviceId].res) / (float)(_deviceConfig->conf[deviceId].max - _deviceConfig->conf[deviceId].min);            
        }
        else if (units == DAC8760_UNITS_VOLTS)
        {
            raw_value = ((value - _deviceConfig->conf[deviceId].min) * _deviceConfig->conf[deviceId].res) / (float)(_deviceConfig->conf[deviceId].max - _deviceConfig->conf[deviceId].min);            
        }
        else
        {
            /* Invalid units */
            ESP_LOGE(DAC8760_TAG, "VoltageWrite : Units specified is invalid !!!"); 
            return;         
        }

        // Write raw value
        if (value <= _deviceConfig->conf[deviceId].res)
        {
            Dac8760_SpiWriteRegister(deviceId, DAC8760_ADDR_DATA, raw_value);
        }
        else
        {
            ESP_LOGE(DAC8760_TAG, "Value : %.02f is not in configured range !!!", value);           
        }
    }
    else
    {
        ESP_LOGE(DAC8760_TAG, "Invalid device id !!!");           
    }
}

void Dac8760_setCurrentMode(uint8_t deviceId, Dac8760_CurrentMode_t mode)
{
    // Configure min and max values
    if (mode == DAC8760_CURRENT_RANGE_4_20_MA)
    {
        _deviceConfig->conf[deviceId].min = 4;
        _deviceConfig->conf[deviceId].max = 20;
    }
    else if (mode == DAC8760_CURRENT_RANGE_0_20_MA)
    {
        _deviceConfig->conf[deviceId].min = 0;
        _deviceConfig->conf[deviceId].max = 20;       
    }
    else if (mode == DAC8760_CURRENT_RANGE_0_24_MA)
    {
        _deviceConfig->conf[deviceId].min = 0;
        _deviceConfig->conf[deviceId].max = 24;         
    }
    else
    {
        ESP_LOGE(DAC8760_TAG, "Invalid current range !!!");
        return;
    }
    
    _deviceConfig->conf[deviceId].mode = mode;

    // Set data to 0
    Dac8760_SpiWriteRegister(deviceId, DAC8760_ADDR_DATA, 0x00);
    // Configure mode    
    Dac8760_SpiWriteRegister(deviceId, DAC8760_ADDR_CTRL, (_deviceConfig->conf[deviceId].mode << DAC8760_CTRL_RANGE) | (1 << DAC8760_CTRL_OUTEN));    
}

Dac8760_CurrentMode_t Dac8760_getCurrentMode(uint8_t deviceId)
{
    if (deviceId < _deviceConfig->nb_devices)
    {
        return (Dac8760_CurrentMode_t) _deviceConfig->conf[deviceId].mode;
    }
    else
    {
        ESP_LOGE(DAC8760_TAG, "Invalid device id !!!");   
        return (Dac8760_CurrentMode_t) DAC8760_CURRENT_RANGE_INVALID; 
    }    
}

void Dac8760_CurrentWrite(uint8_t deviceId, float value, Dac8760_Units_t units)
{
    uint32_t raw_value = 0;

    if (deviceId < _deviceConfig->nb_devices)
    {
        // Compute raw value according to units
        if (units == DAC8760_UNITS_RAW)
        {
            raw_value = (uint32_t)value;
        }
        else if (units == DAC8760_UNITS_MILLIAMPS)
        {
            raw_value = ((value - _deviceConfig->conf[deviceId].min) * _deviceConfig->conf[deviceId].res) / (float)(_deviceConfig->conf[deviceId].max - _deviceConfig->conf[deviceId].min);            
        }
        else if (units == DAC8760_UNITS_AMPS)
        {
            raw_value = (((value * 1000) - _deviceConfig->conf[deviceId].min) * _deviceConfig->conf[deviceId].res) / (float)(_deviceConfig->conf[deviceId].max - _deviceConfig->conf[deviceId].min);            
        }
        else
        {
            /* Invalid units */
            ESP_LOGE(DAC8760_TAG, "CurrentWrite : Units specified is invalid !!!"); 
            return;         
        }

        // Write raw value
        if (value <= _deviceConfig->conf[deviceId].res)
        {
            Dac8760_SpiWriteRegister(deviceId, DAC8760_ADDR_DATA, raw_value);
        }
        else
        {
            ESP_LOGE(DAC8760_TAG, "Value : %.02f is not in configured range !!!", value);           
        }
    }
    else
    {
        ESP_LOGE(DAC8760_TAG, "Invalid device id !!!");           
    }
}

esp_err_t Dac8760_SpiWriteRegister(uint8_t deviceId, uint8_t reg, uint16_t data)
{
    uint8_t spiBuf[3];
    spiBuf[0] = reg;
    spiBuf[1] = data >> 8;
    spiBuf[2] = (uint8_t)data;

    if (_spiInitialized == true)
    {
        spi_transaction_t trans = {
            .flags = 0,
            .cmd = 0,
            .addr = 0,
            .length = 24,
            .rxlength = 0,
            .user = NULL,
            .tx_buffer = spiBuf,
            .rx_buffer = NULL
        };

        ESP_ERROR_CHECK(spi_device_polling_transmit(_spiHandler[deviceId], &trans));
    }
    else
    {
        ESP_LOGE(DAC8760_TAG, "SPI is not initialized !!!");
    }

    return 0;
}