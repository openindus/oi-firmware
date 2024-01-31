/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file CoreStandalone.cpp
 * @brief Functions for core module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "CoreStandalone.h"
#include "CorePinout.h"

#if defined(CONFIG_CORE)

static const char CORE_TAG[] = "Core";

const ioex_num_t CoreStandalone::_dout[4] = { 
    CORE_IOEX_PIN_DOUT_1,
    CORE_IOEX_PIN_DOUT_2,
    CORE_IOEX_PIN_DOUT_3,
    CORE_IOEX_PIN_DOUT_4,
};

const ioex_num_t CoreStandalone::_doutSensor[4] = { 
    CORE_IOEX_PIN_DOUT_CURRENT_1,
    CORE_IOEX_PIN_DOUT_CURRENT_2,
    CORE_IOEX_PIN_DOUT_CURRENT_3,
    CORE_IOEX_PIN_DOUT_CURRENT_4,
};

const ioex_num_t CoreStandalone::_din[4] = { 
    CORE_IOEX_PIN_DIN_1,
    CORE_IOEX_PIN_DIN_2,
    CORE_IOEX_PIN_DIN_3,
    CORE_IOEX_PIN_DIN_4,
};

const adc1_channel_t CoreStandalone::_eana[2] = {
    CORE_CHANNEL_AIN_1,
    CORE_CHANNEL_AIN_2
};

ioex_device_t* CoreStandalone::_ioex;
esp_adc_cal_characteristics_t CoreStandalone::_adc1Characteristics;
std::map<DigitalInputNum_t, InterruptMode_t> CoreStandalone::_dinCurrentMode;

void CoreStandalone::init()
{
    ModuleStandalone::init();

    /**
     * @brief I2C init
     * 
     */
    ESP_LOGI(CORE_TAG, "Initializes the bus I2C (I2C_NUM_%u)", CORE_I2C_PORT_NUM);
    ESP_LOGI(CORE_TAG, "SDA: GPIO_NUM_%u | SCL: GPIO_NUM_%u",
        CORE_PIN_I2C_SDA, CORE_PIN_I2C_SCL);

    i2c_config_t i2c_config;
	memset(&i2c_config, 0, sizeof(i2c_config_t));
    i2c_config.mode = I2C_MODE_MASTER;
    i2c_config.sda_io_num = CORE_PIN_I2C_SDA;
    i2c_config.sda_pullup_en = GPIO_PULLUP_ENABLE;
    i2c_config.scl_io_num = CORE_PIN_I2C_SCL;
    i2c_config.scl_pullup_en = GPIO_PULLUP_ENABLE;
    i2c_config.master.clk_speed = CORE_DEFAULT_I2C_SPEED;

    ESP_ERROR_CHECK(i2c_param_config(CORE_I2C_PORT_NUM, &i2c_config));
    ESP_ERROR_CHECK(i2c_driver_install(CORE_I2C_PORT_NUM, i2c_config.mode, 0, 0, 0));

    // /**
    //  * @brief SPI Init --> This SPI is used for SDCard, Ethernet and USBHost: it should be initialized directly by user using Arduino Lib
    //  * 
    //  */
    // ESP_LOGI(CORE_TAG, "Initializes the bus SPI%u", CORE_SPI_HOST+1);
    // ESP_LOGI(CORE_TAG, "MOSI:  GPIO_NUM_%u | MISO:  GPIO_NUM_%u | CLK:  GPIO_NUM_%u",
    //     CORE_PIN_SPI_MOSI, CORE_PIN_SPI_MISO, CORE_PIN_SPI_SCK);

    // spi_bus_config_t spi_config;
    // spi_config.mosi_io_num = CORE_PIN_SPI_MOSI;
    // spi_config.miso_io_num = CORE_PIN_SPI_MISO;
    // spi_config.sclk_io_num = CORE_PIN_SPI_SCK;
    // spi_config.quadwp_io_num = -1;
    // spi_config.quadhd_io_num = -1;
    // spi_config.max_transfer_sz = 0;
    // spi_config.flags = 0;
    // spi_config.intr_flags = 0;

    // ESP_ERROR_CHECK(spi_bus_initialize(CORE_SPI_HOST, &spi_config, 1));

    /**
     * @brief SPI Init
     * 
     */
    ESP_LOGI(CORE_TAG, "Initializes the bus SPI%u", CORE_SPI_USER_HOST+1);
    ESP_LOGI(CORE_TAG, "MOSI:  GPIO_NUM_%u | MISO:  GPIO_NUM_%u | CLK:  GPIO_NUM_%u",
        CORE_PIN_SPI_USER_MOSI, CORE_PIN_SPI_USER_MISO, CORE_PIN_SPI_USER_SCK);

    spi_bus_config_t spi_config;
    spi_config.mosi_io_num = CORE_PIN_SPI_USER_MOSI;
    spi_config.miso_io_num = CORE_PIN_SPI_USER_MISO;
    spi_config.sclk_io_num = CORE_PIN_SPI_USER_SCK;
    spi_config.quadwp_io_num = -1;
    spi_config.quadhd_io_num = -1;
    spi_config.max_transfer_sz = 0;
    spi_config.flags = 0;
    spi_config.intr_flags = 0;

    ESP_ERROR_CHECK(spi_bus_initialize(CORE_SPI_USER_HOST, &spi_config, SPI_DMA_CH_AUTO));

    /**
     * @brief IO Expander init
     * 
     */
    _ioex = ioex_create(CORE_I2C_PORT_NUM, CORE_I2C_IOEXPANDER_ADDRESS, true, CORE_PIN_DIGITAL_INTERRUPT);

    
    /**
     * @brief Command mosfet alim Init
     * 
     */

    // /!\ Set level before setting to output
    ESP_ERROR_CHECK(ioex_set_level(_ioex, CORE_IOEX_PIN_ALIM_AUX, IOEX_HIGH));

    ioex_config_t config;
    config.mode = IOEX_OUTPUT;
    config.pull_mode = IOEX_FLOATING;
    config.interrupt_type = IOEX_INTERRUPT_DISABLE;
    config.pin_bit_mask = (1ULL<<CORE_IOEX_PIN_ALIM_AUX);
    ESP_ERROR_CHECK(ioex_config(_ioex, &config));

    /**
     * @brief DIN Init
     * 
     */
    ioex_config_t din_config;
    din_config.mode = IOEX_INPUT;
    din_config.pull_mode = IOEX_PULLDOWN;
    din_config.interrupt_type = IOEX_INTERRUPT_DISABLE;
    din_config.pin_bit_mask = (1ULL<<_din[0]) | 
                              (1ULL<<_din[1]) | 
                              (1ULL<<_din[2]) | 
                              (1ULL<<_din[3]);
    
    ESP_ERROR_CHECK(ioex_config(_ioex, &din_config));

    _dinCurrentMode[DIN_1] = NONE_MODE;
    _dinCurrentMode[DIN_2] = NONE_MODE;
    _dinCurrentMode[DIN_3] = NONE_MODE;
    _dinCurrentMode[DIN_4] = NONE_MODE;

    /**
     * @brief DOUT Init
     * 
     */
    // /!\ Set level before setting to output
    ESP_ERROR_CHECK(ioex_set_level(_ioex, _dout[0], IOEX_LOW));
    ESP_ERROR_CHECK(ioex_set_level(_ioex, _dout[1], IOEX_LOW));
    ESP_ERROR_CHECK(ioex_set_level(_ioex, _dout[2], IOEX_LOW));
    ESP_ERROR_CHECK(ioex_set_level(_ioex, _dout[3], IOEX_LOW));
    
    ioex_config_t dout_config;
    dout_config.mode = IOEX_OUTPUT;
    dout_config.pull_mode = IOEX_FLOATING;
    dout_config.interrupt_type = IOEX_INTERRUPT_DISABLE;
    dout_config.pin_bit_mask = (1ULL<<_dout[0]) | 
                               (1ULL<<_dout[1]) | 
                               (1ULL<<_dout[2]) | 
                               (1ULL<<_dout[3]);
    
    ESP_ERROR_CHECK(ioex_config(_ioex, &dout_config));

    /**
     * @brief DOUT Sensor init
     * 
     */
    ioex_config_t dout_sensor_config;        
    dout_sensor_config.mode = IOEX_INPUT;
    dout_sensor_config.pull_mode = IOEX_PULLDOWN;
    dout_sensor_config.interrupt_type = IOEX_INTERRUPT_POSEDGE;
    dout_sensor_config.pin_bit_mask = (1ULL<<_doutSensor[0]) | 
                                      (1ULL<<_doutSensor[0]) | 
                                      (1ULL<<_doutSensor[0]) | 
                                      (1ULL<<_doutSensor[0]);

    ESP_ERROR_CHECK(ioex_config(_ioex, &dout_sensor_config));

    /**
     * @brief AIN Init
     * 
     */
    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_12));
    for (auto i: _eana) {
        ESP_ERROR_CHECK(adc1_config_channel_atten(_eana[i], ADC_ATTEN_DB_11));
    }
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &_adc1Characteristics);

    /**
     * @brief CAN EXT init
     * 
     */
    ioex_config_t io_can_conf;

    io_can_conf.mode = IOEX_OUTPUT;
    io_can_conf.pull_mode = IOEX_FLOATING;
    io_can_conf.interrupt_type = IOEX_INTERRUPT_DISABLE;
    io_can_conf.pin_bit_mask = (1ULL<<CORE_IOEX_PIN_CAN_RESET);
    ioex_config(_ioex, &io_can_conf);
    ioex_set_level(_ioex, CORE_IOEX_PIN_CAN_RESET, IOEX_HIGH);
    vTaskDelay(10);
    ioex_set_level(_ioex, CORE_IOEX_PIN_CAN_RESET, IOEX_LOW);
    vTaskDelay(10);
    ioex_set_level(_ioex, CORE_IOEX_PIN_CAN_RESET, IOEX_HIGH);

    /**
     * @brief RS Ext Init
     * 
     */
    ioex_config_t io_rs_conf;

    io_rs_conf.mode = IOEX_OUTPUT;
    io_rs_conf.pull_mode = IOEX_FLOATING;
    io_rs_conf.interrupt_type = IOEX_INTERRUPT_DISABLE;
    io_rs_conf.pin_bit_mask = (1ULL<<CORE_IOEX_PIN_RS_RESET);
    ioex_config(_ioex, &io_rs_conf);
    ioex_set_level(_ioex, CORE_IOEX_PIN_RS_RESET, IOEX_HIGH);

    /**
     * @brief RTC Init
     * Configure IOExpander RTC Interrupt pin
     */

    ioex_config_t io_rtc_conf;    
    io_rtc_conf.mode = IOEX_INPUT;
    io_rtc_conf.pull_mode = IOEX_PULLUP;
    io_rtc_conf.interrupt_type = IOEX_INTERRUPT_NEGEDGE;
    io_rtc_conf.pin_bit_mask = (1ULL<<CORE_PIN_RTC_INTERRUPT);

    ESP_ERROR_CHECK(ioex_config(_ioex, &io_rtc_conf));
}

void CoreStandalone::digitalWrite(DigitalOutputNum_t dout, uint8_t level)
{
    ioex_set_level(_ioex, _dout[dout], static_cast<ioex_level_t>(level));
}

int CoreStandalone::digitalRead(DigitalInputNum_t din)
{
    return ioex_get_level(_ioex, _din[din]);
}

int CoreStandalone::analogRead(AnalogInputNum_t ana)
{
    int adc_reading = 0;

    for (int i = 0; i < CORE_ADC_NO_OF_SAMPLES; i++)
    {
        adc_reading += adc1_get_raw(_eana[ana]);
    }

    adc_reading /= CORE_ADC_NO_OF_SAMPLES;

    return adc_reading;
}

float CoreStandalone::analogReadMilliVolts(AnalogInputNum_t ana)
{
    int adc_reading = CoreStandalone::analogRead(ana);

    // Convert adc_reading to voltage in mV
    uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, &_adc1Characteristics);

    return static_cast<float>(voltage * CORE_ADC_REDUCTION_FACTOR);
}

void CoreStandalone::attachInterrupt(DigitalInputNum_t din, void (*callback)(void *), void * args, InterruptMode_t mode)
{
    InterruptMode_t newMode = (InterruptMode_t)((int)_dinCurrentMode.at(din) | (int)mode);

    if(newMode == mode)
    {
        ioex_set_interrupt_type(_ioex, _din[din], (ioex_interrupt_type_t)(mode));
    }
    
    else if(newMode == CHANGE_MODE)
    {
        ioex_set_interrupt_type(_ioex, _din[din], IOEX_INTERRUPT_ANYEDGE);
    }

     _dinCurrentMode[din] = newMode;
    ioex_isr_handler_add(_ioex, _din[din], (ioex_isr_t)callback, args, CORE_DIGITAL_INTERRUPT_PRIORITY);
    ioex_interrupt_enable(_ioex, _din[din]);
}

void CoreStandalone::detachInterrupt(DigitalInputNum_t din, InterruptMode_t mode)
{
    
    if(_dinCurrentMode.at(din) == CHANGE_MODE)
    {
        if(mode == FALLING_MODE)
        {
            ioex_set_interrupt_type(_ioex, _din[din], IOEX_INTERRUPT_POSEDGE);
            _dinCurrentMode[din] = RISING_MODE;
        }
        else if(mode == RISING_MODE)
        {
            ioex_set_interrupt_type(_ioex, _din[din], IOEX_INTERRUPT_NEGEDGE);
            _dinCurrentMode[din] = FALLING_MODE;
        }
    }

    else if(_dinCurrentMode.at(din) == mode)
    {
        ioex_set_interrupt_type(_ioex, _din[din], IOEX_INTERRUPT_DISABLE);
        ioex_isr_handler_remove(_ioex, _din[din]);
        _dinCurrentMode[din] = NONE_MODE;
    }
}

#endif
