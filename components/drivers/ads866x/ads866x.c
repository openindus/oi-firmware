#include "ads866x.h"

static const char ADS866x_TAG[] = "Ads866x";

static uint8_t ads866x_mode = ADS866X_MODE_IDLE;
static Ads866x_DeviceConfig_t*_deviceConfig = NULL;
static float ads866x_analogReference = ADS8664_ANALOG_REFERENCE;
static spi_device_handle_t _spiHandler = NULL;

static bool _spiInitialized = false;
static bool _gpioInitialized = false;
static bool _deviceConfigured = false;

void Ads866x_DeviceConfig(Ads866x_DeviceConfig_t *config)
{
    ESP_LOGI(ADS866x_TAG, "Configure Ads866x device");

    _deviceConfig = config;
    _deviceConfigured = true;
}

uint8_t Ads866x_Init(void)
{
    uint8_t ret = 0;

    Ads866x_GpioInit();

    Ads8866x_SpiInit();

    return ret;
}

void Ads866x_GpioInit(void)
{
    uint8_t i = 0;

    ESP_LOGV(ADS866x_TAG, "Initializes the GPIOs");

    assert(_deviceConfig != NULL);

    if (_deviceConfigured == true)
    {
        gpio_config_t cfg;
            
        ESP_LOGI(ADS866x_TAG, "Initializes Ads866x GPIOs");
        ESP_LOGI(ADS866x_TAG, "RST: io%d", _deviceConfig->pin_rst);

        /* Configure Rst, mode pins */
        cfg.pin_bit_mask = (1ULL << _deviceConfig->pin_rst);

        for(i = 0; i < _deviceConfig->adc_analogs_nb; i++)
        {
            cfg.pin_bit_mask |= (1ULL << _deviceConfig->pin_mode[i]);
        }

        cfg.mode = GPIO_MODE_OUTPUT;
        cfg.pull_up_en = GPIO_PULLUP_DISABLE;
        cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
        cfg.intr_type = GPIO_INTR_DISABLE;

        ESP_ERROR_CHECK(gpio_config(&cfg));

        // Set Rst pin to HIGH level
        ESP_ERROR_CHECK(gpio_set_level(_deviceConfig->pin_rst, 1));

        /* Set level LOW for pin mode - Configure analogs in voltage configuration */
        for (i = 0; i < _deviceConfig->adc_analogs_nb; i++)
        {
            gpio_set_level(_deviceConfig->pin_mode[i], ADS866X_VOLTAGE_MODE);
            _deviceConfig->adc_mode[i] = ADS866X_VOLTAGE_MODE;
            _deviceConfig->adc_range[i] = ADS866X_R5;       //10.24V
        }

        // Set default adc resolution
        _deviceConfig->adc_res = ADS866x_RESOLUTION_BITS;

        _gpioInitialized = true;
    }
    else
    {
        ESP_LOGE(ADS866x_TAG, "Device is not configured !!!");
    }
}


void Ads8866x_SpiInit(void)
{
    if (_spiInitialized == false)
    {

        ESP_LOGV(ADS866x_TAG, "Initializes the SPI used by Ads866x");

        if (_deviceConfigured == true)
        {
            assert(_deviceConfig != NULL);

            spi_device_interface_config_t devConfig = {
                .command_bits = 0,
                .address_bits = 0,
                .dummy_bits = 0,
                .mode = 1,
                .duty_cycle_pos = 0,
                .cs_ena_pretrans = 0,
                .cs_ena_posttrans = 2,
                .clock_speed_hz = _deviceConfig->spi_freq,
                .input_delay_ns = 0,
                .spics_io_num = _deviceConfig->spi_pin_cs,
                .flags = 0,
                .queue_size = 1,
                .pre_cb = NULL,
                .post_cb = NULL
            };

            ESP_ERROR_CHECK(spi_bus_add_device(_deviceConfig->spi_host, &devConfig, &_spiHandler));

            _spiInitialized = true;
        }
        else
        {
            ESP_LOGE(ADS866x_TAG, "Device is not configured !!!");
        }
    }
}


uint32_t Ads866x_AnalogRead(uint32_t analogNum)
{
    uint32_t res = 0;

    if (_deviceConfigured)
    {
        if (analogNum < _deviceConfig->adc_analogs_nb)
        {
            // Active channel
            Ads866x_setChannelSPD(1 << analogNum);
            // Change mode of Ads866x
            Ads866x_AutoRst();

            res = Ads866x_NoOp();

            // Puts result in 12 bits format - Cf datasheet ADS8664 Block Table 4 - Page 56
            res = res >> 4;

            if (_deviceConfig->adc_res == ADS866x_ADC_RES_10BITS)
            {
                res = (res >> 2);
            }
        }
        else
        {
            ESP_LOGE(ADS866x_TAG, "Invalid analog index. Must be < %d", _deviceConfig->adc_analogs_nb);            
        }
    }
    else
    {
        ESP_LOGE(ADS866x_TAG, "Device is not configured !!!");
    }
    return res;
}

float Ads866x_AnalogReadUnits(uint32_t analogNum, Ads866x_Units_t units)
{
    float res = 0.0;

    // Check of device configured and valid analogNum is done when analogRead is called
    uint32_t adc_raw = Ads866x_AnalogRead(analogNum);

    if (units == ADS866x_UNITS_RAW)
    {
        res = adc_raw;
    }
    else
    {
        // VOLTAGE MODE
        if (_deviceConfig->adc_mode[analogNum] == ADS866X_VOLTAGE_MODE)
        {
            res = Ads866x_ConvertRaw2Volt(adc_raw, ADS866X_R5);

            if (units == ADS866x_UNITS_VOLTS)
            {

            }
            else if (units == ADS866x_UNITS_MILLIVOLTS)
            {
                res = res * 1000;
            }
            else 
            {
                res = 0;
                ESP_LOGE(ADS866x_TAG, "Invalid units specified. Mode was probably not well configured.");
            }

        }
        else // CURRENT MODE
        {
            res = Ads866x_ConvertRaw2Volt(adc_raw, ADS866X_R6) / ADS866x_CURRENT_MES_RES_VALUE;
            if (units == ADS866x_UNITS_AMPS)
            {

            }
            else if (units == ADS866x_UNITS_MILLIAMPS)
            {
                res = res * 1000;
            }
            else
            {
                res = 0;
                ESP_LOGE(ADS866x_TAG, "Invalid units specified. Mode was probably not well configured.");
            }

        }
    }

    return res;
}

void Ads866x_setAdcMode(uint32_t analogNum, Ads866x_AdcMode_t mode)
{
    if (_deviceConfigured)
    {
        if (analogNum < _deviceConfig->adc_analogs_nb)
        {
            _deviceConfig->adc_mode[analogNum] = mode;
            gpio_set_level(_deviceConfig->pin_mode[analogNum], mode);

            if (mode == ADS866X_CURRENT_MODE)
            {
                Ads866x_setChannelRange(analogNum, ADS866X_R7); // Range 0 - 2.56V
            }
        }
        else
        {
            ESP_LOGE(ADS866x_TAG, "Invalid analog index. Must be < %d", _deviceConfig->adc_analogs_nb);
        }
    }
    else
    {
        ESP_LOGE(ADS866x_TAG, "Device is not configured !!!");
    }
}

void Ads866x_setAdcResolution(Ads866x_AdcResolutionBits_t analogRes)
{
        _deviceConfig->adc_res = analogRes;
}

uint8_t Ads866x_getAdcResolution(void)
{
    return _deviceConfig->adc_res;
}

float Ads866x_getAnalogReference(void)
{
    return ads866x_analogReference * 1000;
}

void Ads866x_setAnalogReference(float analogReference)
{
    ads866x_analogReference = analogReference / 1000.0;
}

uint16_t Ads866x_NoOp(void)
{
    return Ads866x_SpiWriteCmdRegister(ADS866X_CMD_NO_OP);
}

uint16_t Ads866x_Reset(void)
{
    ESP_LOGV(ADS866x_TAG, "Reset ads866x.");
    return Ads866x_SpiWriteCmdRegister(ADS866X_CMD_RST);
}

uint16_t Ad866x_Standby(void)
{
    uint16_t ret = 0;
    ret = Ads866x_SpiWriteCmdRegister(ADS866X_CMD_STDBY);
    ads866x_mode = ADS866X_MODE_STANDBY;
    return ret;
}

uint16_t Ads866x_PowerDown(void)
{
    uint16_t ret = 0;
    ret = Ads866x_SpiWriteCmdRegister(ADS866X_CMD_PWR_DN);
    ads866x_mode = ADS866X_MODE_POWER_DN;
    return ret;
}

uint16_t Ads866x_AutoRst(void)
{
    uint16_t ret = 0;
    ret = Ads866x_SpiWriteCmdRegister(ADS866X_CMD_AUTO_RST);
    ads866x_mode = ADS866X_MODE_AUTO_RST;
    return ret;
}

uint16_t Ads866x_ManualChannelSelect(uint8_t channelNum)
{
    uint16_t ret = 0;
    uint8_t cmd = 0;

    if (!_deviceConfigured)
    {
        return -1;
    }

    if (channelNum < _deviceConfig->adc_analogs_nb)
    {
        ESP_LOGE(ADS866x_TAG,"Invalid channel number");
        return -1;
    }
    cmd = ADS866X_CMD_MAN_Ch_0 + (channelNum * 4);
    ret = Ads866x_SpiWriteCmdRegister(cmd);
    ads866x_mode = cmd;

    return ret;
}

void Ads866x_setChannelSequence(uint8_t channels_on)
{
    Ads866x_SpiWriteRegister(AUTO_SEQ_EN, channels_on);
}

void Ads866x_setChannelPowerDown(uint8_t channels_off)
{
    Ads866x_SpiWriteRegister(CH_PWR_DN, channels_off);
}

void Ads866x_setChannelSPD(uint8_t channels)
{
    Ads866x_setChannelSequence(channels);
    Ads866x_setChannelPowerDown((uint8_t)~channels);
}

uint8_t Ads866x_getChannelSequence(void)
{
    return Ads866x_SpiReadRegister(AUTO_SEQ_EN);
}

uint8_t Ads866x_getChannelPowerDown(void)
{
    return Ads866x_SpiReadRegister(CH_PWR_DN);
}

/****** Feature Select Register *****************/

uint8_t Ads866x_GetDeviceId(void)
{
    return (Ads866x_getFeatureSelect() >> 6);
}

void Ads866x_SetDeviceId(uint16_t id)
{
    uint8_t reg = 0;
    reg = Ads866x_getFeatureSelect();
    reg = (reg & 0b00010111) | ((id & 0b11) << 6);
    Ads866x_SpiWriteRegister(FT_SEL, reg);
}

bool Ads866x_getAlarm(void)
{
    return (Ads866x_getFeatureSelect() >> 4) & 1;
}

void Ads866x_setAlarm(bool alarm)
{
    uint8_t reg = 0;
    reg = Ads866x_getFeatureSelect();
    reg = (reg & 0b11000111) | ((alarm == true) << 4);
    Ads866x_SpiWriteRegister(FT_SEL, reg);
}

uint8_t Ads866x_getSdo(void)
{
    return (Ads866x_getFeatureSelect() & 0b111);
}

void Ads866x_setSdo(uint8_t sdo)
{
    uint8_t reg = 0;
    reg = Ads866x_getFeatureSelect();
    reg = (reg & 0b11010000) | (sdo & 0b111);
    Ads866x_SpiWriteRegister(FT_SEL, reg);
}

uint8_t Ads866x_getFeatureSelect(void)
{
    return Ads866x_SpiReadRegister(FT_SEL);
}

uint8_t Ads866x_getChannelRange(uint8_t channel)
{
    uint8_t reg = 0;

    if (channel >= _deviceConfig->adc_analogs_nb)
    {
        ESP_LOGE(ADS866x_TAG,"Invalid channel number");
        return -1;
    }
    reg = RG_Ch_0 + channel;

    return Ads866x_SpiReadRegister(reg);
}

void Ads866x_setChannelRange(uint8_t channel, uint8_t range)
{
    uint8_t reg = 0;
    if (channel >= _deviceConfig->adc_analogs_nb)
    {
        ESP_LOGE(ADS866x_TAG,"Invalid channel number");
    }
    reg = RG_Ch_0 + channel;
    Ads866x_SpiWriteRegister(reg, (range & 0x0F));
}

void Ads866x_setGlobalRange(uint8_t range)
{
    uint8_t i = 0;

    for (i = 0; i < _deviceConfig->adc_analogs_nb; i++)
    {
        Ads866x_setChannelRange(i, range);
    }
}


/************** Alarm Registers ***********************/

uint8_t Ads866x_getAlarmOverview()
{
    return Ads866x_SpiReadRegister(ALARM_OVERVIEW);
}

uint8_t Ads866x_getFirstTrippedFlag()
{
    return Ads866x_SpiReadRegister(ALARM_CH0_TRIPPED_FLAG);
}

uint8_t Ads866x_getSecondTrippedFlag()
{
    return Ads866x_SpiReadRegister(ALARM_CH4_TRIPPED_FLAG);
}

uint16_t Ads866x_getTrippedFlags()
{
    uint8_t MSB = Ads866x_SpiReadRegister(ALARM_CH0_TRIPPED_FLAG);
    uint8_t LSB = Ads866x_SpiReadRegister(ALARM_CH4_TRIPPED_FLAG);

    return (MSB << 8) | LSB;
}

uint8_t Ads866x_getFirstActiveFlag()
{
    return Ads866x_SpiReadRegister(ALARM_CH0_ACTIVE_FLAG);
}

uint8_t Ads866x_getSecondActiveFlag()
{
    return Ads866x_SpiReadRegister(ALARM_CH4_ACTIVE_FLAG);
}

uint16_t Ads866x_getActiveFlags()
{
    uint8_t MSB = Ads866x_SpiReadRegister(ALARM_CH0_ACTIVE_FLAG);
    uint8_t LSB = Ads866x_SpiReadRegister(ALARM_CH4_ACTIVE_FLAG);

    return (MSB << 8) | LSB;    
}

uint8_t Ads866x_getChannelHysteresis(uint8_t channel)
{
    uint8_t reg = 0;
    if (channel < _deviceConfig->adc_analogs_nb)
    {
        ESP_LOGE(ADS866x_TAG,"Invalid channel number");
        return -1;
    }
    reg = CH0_HYST + (5*channel);
    return Ads866x_SpiReadRegister(reg);
}

uint16_t Ads866x_getChanneLowThreshold(uint8_t channel)
{
    uint8_t reg = 0;
    uint8_t MSB = 0;
    uint8_t LSB = 0;
    
    if (channel < _deviceConfig->adc_analogs_nb)
    {
        ESP_LOGE(ADS866x_TAG,"Invalid channel number");
        return -1;
    }
    reg = CH0_LT_MSB + (5*channel);
    MSB = Ads866x_SpiReadRegister(reg);
    LSB =Ads866x_SpiReadRegister(reg+1);

    return (MSB << 8) | LSB;
}

uint16_t Ads866x_getChanneHighThreshold(uint8_t channel)
{
    uint8_t reg = 0;
    uint8_t MSB = 0;
    uint8_t LSB = 0;
    
    if (channel < _deviceConfig->adc_analogs_nb)
    {
        ESP_LOGE(ADS866x_TAG,"Invalid channel number");
        return -1;
    }
    reg = CH0_HT_MSB + (5*channel);
    MSB = Ads866x_SpiReadRegister(reg);
    LSB = Ads866x_SpiReadRegister(reg+1);

    return (MSB << 8) | LSB;
}

void Ads866x_setChannelHysteresis(uint8_t channel, uint8_t hysteresis)
{
    uint8_t reg = 0;
    if (channel < _deviceConfig->adc_analogs_nb)
    {
        ESP_LOGE(ADS866x_TAG,"Invalid channel number");
    }
    reg = CH0_HYST + (5*channel);
    Ads866x_SpiWriteRegister(reg, (hysteresis & 0x0F) << 4);
}

void Ads866x_setChannelLowThreshold(uint8_t channel, uint16_t threshold)
{
    uint8_t reg = 0;
    if (channel < _deviceConfig->adc_analogs_nb)
    {
        ESP_LOGE(ADS866x_TAG,"Invalid channel number");
    }
    reg = CH0_LT_MSB + (5*channel);
    Ads866x_SpiWriteRegister(reg, (threshold & 0xFFF) >> 8);
    Ads866x_SpiWriteRegister(reg+1, (threshold & 0x0F) << 4);
}

void Ads866x_setChannelHighThreshold(uint8_t channel, uint16_t threshold)
{
    uint8_t reg = 0;
    if (channel < _deviceConfig->adc_analogs_nb)
    {
        ESP_LOGE(ADS866x_TAG,"Invalid channel number");
    }
    reg = CH0_HT_MSB + (5*channel);
    Ads866x_SpiWriteRegister(reg, (threshold & 0xFFF) >> 8);
    Ads866x_SpiWriteRegister(reg+1, (threshold & 0x0F) << 4);    
}

uint8_t Ads866x_getCommandReadBack(void)
{
    return Ads866x_SpiReadRegister(CMD_READBACK);
}

float Ads866x_ConvertRaw2Volt(uint16_t rawValue, uint8_t inputRange)
{
    float out_min, out_max;
    switch (inputRange)
    {
        case ADS866X_R1:
            out_min = -1.25 * ads866x_analogReference;
            out_max = 1.25 * ads866x_analogReference;
            break;
        case ADS866X_R2:
            out_min = -0.625 * ads866x_analogReference;
            out_max = 0.625 * ads866x_analogReference;
            break;
        case ADS866X_R3:
            out_min = -0.3125 * ads866x_analogReference;
            out_max = 0.3125 * ads866x_analogReference;
            break;
        case ADS866X_R4:
            out_min = -0.15625 * ads866x_analogReference;
            out_max = 0.15625 * ads866x_analogReference;
            break;
        case ADS866X_R5:
            out_min = 0 * ads866x_analogReference;
            out_max = 2.5 * ads866x_analogReference;
            break;
        case ADS866X_R6:
            out_min = 0 * ads866x_analogReference;
            out_max = 1.25 * ads866x_analogReference;
            break;
        case ADS866X_R7:
            out_min = 0 * ads866x_analogReference;
            out_max = 0.625 * ads866x_analogReference;
            break;
        case ADS866X_R8:
            out_min = 0 * ads866x_analogReference;
            out_max = 0.3125 * ads866x_analogReference;
            break;
        default:
            out_min = -2.5 * ads866x_analogReference;
            out_max = 2.5 * ads866x_analogReference;
            break;
    }
    return (float)rawValue * (out_max - out_min) / (float)(1 << _deviceConfig->adc_res) + out_min;
}

uint16_t Ads866x_ConvertVolt2Raw(float voltageValue, uint8_t inputRange)
{
    float in_min, in_max;
    switch (inputRange)
    {
        case ADS866X_R1:
            in_min = -1.25 * ads866x_analogReference;
            in_max = 1.25 * ads866x_analogReference;
            break;
        case ADS866X_R2:
            in_min = -0.625 * ads866x_analogReference;
            in_max = 0.625 * ads866x_analogReference;
            break;
        case ADS866X_R3:
            in_min = -0.3125 * ads866x_analogReference;
            in_max = 0.3125 * ads866x_analogReference;
            break;
        case ADS866X_R4:
            in_min = -0.15625 * ads866x_analogReference;
            in_max = 0.15625 * ads866x_analogReference;
            break;
        case ADS866X_R5:
            in_min = 0 * ads866x_analogReference;
            in_max = 2.5 * ads866x_analogReference;
            break;
        case ADS866X_R6:
            in_min = 0 * ads866x_analogReference;
            in_max = 1.25 * ads866x_analogReference;
            break;
        case ADS866X_R7:
            in_min = 0 * ads866x_analogReference;
            in_max = 0.625 * ads866x_analogReference;
            break;
        case ADS866X_R8:
            in_min = 0 * ads866x_analogReference;
            in_max = 0.3125 * ads866x_analogReference;
            break;
        default:
            in_min = -2.5 * ads866x_analogReference;
            in_max = 2.5 * ads866x_analogReference;
            break;
    }
    return (voltageValue-in_min) * (1 << _deviceConfig->adc_res) / (in_max-in_min);    
}

esp_err_t Ads866x_SpiWriteRegister(uint8_t reg, uint8_t value)
{
    uint8_t txBuffer[3] = {((reg << 1) | 0x01), value, 0x00};

    if (_spiInitialized == true)
    {
        spi_transaction_t trans = {
            .flags = 0,
            .cmd = 0,
            .addr = 0,
            .length = 24,
            .rxlength = 0,
            .user = NULL,
            .tx_buffer = txBuffer,
            .rx_buffer = NULL
        };

        ESP_ERROR_CHECK(spi_device_polling_transmit(_spiHandler, &trans));

        ads866x_mode = ADS866X_MODE_PROG;
    }
    else
    {
        ESP_LOGE(ADS866x_TAG, "SPI is not initialized !!!");
    }
    return 0;
}

uint8_t Ads866x_SpiReadRegister(uint8_t reg)
{
    uint8_t txBuffer[2] = {((reg << 1) | 0x00), 0x00};
    uint8_t rxBuffer;

    if (_spiInitialized == true)
    {
        spi_transaction_t trans = {
            .flags = 0,
            .cmd = 0,
            .addr = 0,
            .length = 16,
            .rxlength = 0,
            .user = NULL,
            .tx_buffer = txBuffer,
            .rx_buffer = &rxBuffer
        };

        ESP_ERROR_CHECK(spi_device_polling_transmit(_spiHandler, &trans));

        ads866x_mode = ADS866X_MODE_PROG;
    }
    else
    {
        ESP_LOGE(ADS866x_TAG, "SPI is not initialized !!!");
    }
    return rxBuffer;
}

uint16_t Ads866x_SpiWriteCmdRegister(uint8_t reg)
{
    uint8_t txBuffer[4] = {reg, 0x00, 0x00, 0x00};
    uint8_t rxBuffer[4];

    if (_spiInitialized == true)
    {
        spi_transaction_t trans = {
            .flags = 0,
            .cmd = 0,
            .addr = 0,
            .length = 16,
            .rxlength = 0,
            .user = NULL,
            .tx_buffer = txBuffer,
            .rx_buffer = rxBuffer
        };

        if (ads866x_mode > ADS866X_MODE_PROG)
        {
            // only 16 bit if POWERDOWN or STDBY or RST or IDLE
            trans.length = 32;
        }

        ESP_ERROR_CHECK(spi_device_polling_transmit(_spiHandler, &trans));
    }
    else
    {
        ESP_LOGE(ADS866x_TAG, "SPI is not initialized !!!");
    }

    if (ads866x_mode == ADS866X_MODE_POWER_DN)
    {
        vTaskDelay(15 / portTICK_PERIOD_MS);
    }

    switch (reg)
    {
        case ADS866X_CMD_NO_OP:
            switch (ads866x_mode) {
                case ADS866X_MODE_RESET:    ads866x_mode = ADS866X_MODE_IDLE;
                    break;
                case ADS866X_MODE_PROG :    ads866x_mode = ADS866X_MODE_IDLE;
                    break;
                case ADS866X_MODE_AUTO_RST: ads866x_mode = ADS866X_MODE_AUTO;
                    break;
            }
            break;
        case ADS866X_CMD_STDBY:
            ads866x_mode = ADS866X_MODE_STANDBY;
            break;
        case ADS866X_CMD_PWR_DN:
            ads866x_mode = ADS866X_MODE_POWER_DN;
            break;
        case ADS866X_CMD_RST:
            ads866x_mode = ADS866X_MODE_RESET;
            break;
        case ADS866X_CMD_AUTO_RST:
            ads866x_mode = ADS866X_MODE_AUTO_RST;
            break;
        default:
            ads866x_mode = ADS866X_MODE_MANUAL;
            break;
    }

    return ((rxBuffer[2] << 8) | rxBuffer[3]);
}