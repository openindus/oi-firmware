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

const ioex_num_t CoreStandalone::_stor[4] = { 
    CORE_IOEX_PIN_DOUT_1,
    CORE_IOEX_PIN_DOUT_2,
    CORE_IOEX_PIN_DOUT_3,
    CORE_IOEX_PIN_DOUT_4,
};

const ioex_num_t CoreStandalone::_storSensor[4] = { 
    CORE_IOEX_PIN_DOUT_CURRENT_1,
    CORE_IOEX_PIN_DOUT_CURRENT_2,
    CORE_IOEX_PIN_DOUT_CURRENT_3,
    CORE_IOEX_PIN_DOUT_CURRENT_4,
};

const ioex_num_t CoreStandalone::_etor[4] = { 
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
std::map<DigitalInputNum_t, InterruptMode_t> CoreStandalone::_etorCurrentMode;

void CoreStandalone::init()
{
    ModuleStandalone::init();

    /* Init GPIO service */
    gpio_install_isr_service(0);

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

    /**
     * @brief SPI Init
     * 
     */
    ESP_LOGI(CORE_TAG, "Initializes the bus SPI%u", CORE_SPI_HOST+1);
    ESP_LOGI(CORE_TAG, "MOSI:  GPIO_NUM_%u | MISO:  GPIO_NUM_%u | CLK:  GPIO_NUM_%u",
        CORE_PIN_SPI_MOSI, CORE_PIN_SPI_MISO, CORE_PIN_SPI_SCK);

    spi_bus_config_t spi_config;
    spi_config.mosi_io_num = CORE_PIN_SPI_MOSI;
    spi_config.miso_io_num = CORE_PIN_SPI_MISO;
    spi_config.sclk_io_num = CORE_PIN_SPI_SCK;
    spi_config.quadwp_io_num = -1;
    spi_config.quadhd_io_num = -1;
    spi_config.max_transfer_sz = 0;
    spi_config.flags = 0;
    spi_config.intr_flags = 0;

    ESP_ERROR_CHECK(spi_bus_initialize(CORE_SPI_HOST, &spi_config, 1));

    /**
     * @brief IO Expander init
     * 
     */
    _ioex = ioex_create(CORE_I2C_PORT_NUM, CORE_I2C_IOEXPANDER_ADDRESS, true, CORE_PIN_DIGITAL_INTERRUPT);

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
    ioex_config_t etor_config;
    etor_config.mode = IOEX_INPUT;
    etor_config.pull_mode = IOEX_PULLDOWN;
    etor_config.interrupt_type = IOEX_INTERRUPT_DISABLE;
    etor_config.pin_bit_mask = (1ULL<<_etor[0]) | 
                               (1ULL<<_etor[1]) | 
                               (1ULL<<_etor[2]) | 
                               (1ULL<<_etor[3]);
    
    ESP_ERROR_CHECK(ioex_config(_ioex, &etor_config));

    _etorCurrentMode[DIN_1] = NONE_MODE;
    _etorCurrentMode[DIN_2] = NONE_MODE;
    _etorCurrentMode[DIN_3] = NONE_MODE;
    _etorCurrentMode[DIN_4] = NONE_MODE;

    /**
     * @brief DOUT Init
     * 
     */
    // /!\ Set level before setting to output
    ESP_ERROR_CHECK(ioex_set_level(_ioex, _stor[0], IOEX_LOW));
    ESP_ERROR_CHECK(ioex_set_level(_ioex, _stor[1], IOEX_LOW));
    ESP_ERROR_CHECK(ioex_set_level(_ioex, _stor[2], IOEX_LOW));
    ESP_ERROR_CHECK(ioex_set_level(_ioex, _stor[3], IOEX_LOW));
    
    ioex_config_t stor_config;
    stor_config.mode = IOEX_OUTPUT;
    stor_config.pull_mode = IOEX_FLOATING;
    stor_config.interrupt_type = IOEX_INTERRUPT_DISABLE;
    stor_config.pin_bit_mask = (1ULL<<_stor[0]) | 
                               (1ULL<<_stor[1]) | 
                               (1ULL<<_stor[2]) | 
                               (1ULL<<_stor[3]);
    
    ESP_ERROR_CHECK(ioex_config(_ioex, &stor_config));

    /**
     * @brief DOUT Sensor init
     * 
     */
    ioex_config_t stor_sensor_config;        
    stor_sensor_config.mode = IOEX_INPUT;
    stor_sensor_config.pull_mode = IOEX_PULLDOWN;
    stor_sensor_config.interrupt_type = IOEX_INTERRUPT_POSEDGE;
    stor_sensor_config.pin_bit_mask = (1ULL<<_storSensor[0]) | 
                                      (1ULL<<_storSensor[0]) | 
                                      (1ULL<<_storSensor[0]) | 
                                      (1ULL<<_storSensor[0]);

    ESP_ERROR_CHECK(ioex_config(_ioex, &stor_sensor_config));

    /**
     * @brief AIN Init
     * 
     */
    #if CONFIG_IDF_TARGET_ESP32
    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_12));
    ESP_ERROR_CHECK(adc1_config_channel_atten(_eana[0], ADC_ATTEN_11db));
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_11db, ADC_WIDTH_BIT_12, 1100, &_adc1Characteristics);
    #endif

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
}

void CoreStandalone::digitalWrite(DigitalOutputNum_t stor, uint8_t level)
{
    ioex_set_level(_ioex, _stor[stor], static_cast<ioex_level_t>(level));
}

int CoreStandalone::digitalRead(DigitalInputNum_t etor)
{
    return ioex_get_level(_ioex, _etor[etor]);
}

float CoreStandalone::analogRead(AnalogInputNum_t ana)
{
    int adc_reading = 0;

    for (int i = 0; i < CORE_ADC_NO_OF_SAMPLES; i++)
    {
        adc_reading += adc1_get_raw(_eana[ana]);
    }

    adc_reading /= CORE_ADC_NO_OF_SAMPLES;

    // Convert adc_reading to voltage in mV
    uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, &_adc1Characteristics);

    return static_cast<float>(voltage * CORE_ADC_REDUCTION_FACTOR);
}

void CoreStandalone::attachInterrupt(DigitalInputNum_t etor, void (*callback)(void *), void * args, InterruptMode_t mode)
{
    InterruptMode_t newMode = (InterruptMode_t)((int)_etorCurrentMode.at(etor) | (int)mode);

    if(newMode == mode)
    {
        ioex_set_interrupt_type(_ioex, _etor[etor], (ioex_interrupt_type_t)(mode));
    }
    
    else if(newMode == CHANGE_MODE)
    {
        ioex_set_interrupt_type(_ioex, _etor[etor], IOEX_INTERRUPT_ANYEDGE);
    }

     _etorCurrentMode[etor] = newMode;
    ioex_isr_handler_add(_ioex, _etor[etor], (ioex_isr_t)callback, args, CORE_DIGITAL_INTERRUPT_PRIORITY);
    ioex_interrupt_enable(_ioex, _etor[etor]);
}

void CoreStandalone::detachInterrupt(DigitalInputNum_t etor, InterruptMode_t mode)
{
    
    if(_etorCurrentMode.at(etor) == CHANGE_MODE)
    {
        if(mode == FALLING_MODE)
        {
            ioex_set_interrupt_type(_ioex, _etor[etor], IOEX_INTERRUPT_POSEDGE);
            _etorCurrentMode[etor] = RISING_MODE;
        }
        else if(mode == RISING_MODE)
        {
            ioex_set_interrupt_type(_ioex, _etor[etor], IOEX_INTERRUPT_NEGEDGE);
            _etorCurrentMode[etor] = FALLING_MODE;
        }
    }

    else if(_etorCurrentMode.at(etor) == mode)
    {
        ioex_set_interrupt_type(_ioex, _etor[etor], IOEX_INTERRUPT_DISABLE);
        ioex_isr_handler_remove(_ioex, _etor[etor]);
        _etorCurrentMode[etor] = NONE_MODE;
    }
}

#endif
