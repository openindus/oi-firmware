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

const ioex_num_t _doutGpio[] = {
    CORE_IOEX_PIN_DOUT_1,
    CORE_IOEX_PIN_DOUT_2,
    CORE_IOEX_PIN_DOUT_3,
    CORE_IOEX_PIN_DOUT_4,
};

const ioex_num_t _doutCurrentGpio[] = {
    CORE_IOEX_PIN_DOUT_CURRENT_1,
    CORE_IOEX_PIN_DOUT_CURRENT_2,
    CORE_IOEX_PIN_DOUT_CURRENT_3,
    CORE_IOEX_PIN_DOUT_CURRENT_4,
};

const ioex_num_t _dinGpio[] = {
    CORE_IOEX_PIN_DIN_1,
    CORE_IOEX_PIN_DIN_2,
    CORE_IOEX_PIN_DIN_3,
    CORE_IOEX_PIN_DIN_4,
};

const adc1_channel_t CoreStandalone::_eana[] = {
    CORE_CHANNEL_AIN_1,
    CORE_CHANNEL_AIN_2
};

ioex_device_t* CoreStandalone::_ioex;

DigitalInput* CoreStandalone::din = new DigitalInput(_ioex, _dinGpio, 4);
DigitalOutput* CoreStandalone::dout = new DigitalOutput(_ioex, _doutGpio, _doutCurrentGpio, 4);

esp_adc_cal_characteristics_t CoreStandalone::_adc1Characteristics;

void CoreStandalone::init()
{
    ModuleStandalone::init();

#if defined(CONFIG_IDF_TARGET_ESP32)

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

    ESP_ERROR_CHECK(ioex_set_level(_ioex, CORE_IOEX_PIN_CMD_MOSFET, IOEX_LOW));
    ESP_ERROR_CHECK(ioex_set_level(_ioex, CORE_IOEX_PIN_ALIM_EXT, IOEX_HIGH));

    ioex_config_t config;
    config.mode = IOEX_OUTPUT;
    config.pull_mode = IOEX_FLOATING;
    config.interrupt_type = IOEX_INTERRUPT_DISABLE;
    config.pin_bit_mask = (1ULL<<CORE_IOEX_PIN_CMD_MOSFET) | (1ULL<<CORE_IOEX_PIN_ALIM_EXT);
    ESP_ERROR_CHECK(ioex_config(_ioex, &config));

    /**
     * @brief DIN Init
     * 
     */
    din->init();

    /**
     * @brief DOUT Init
     * 
     */
    dout->init();

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

#elif defined(CONFIG_IDF_TARGET_ESP32S3)

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
    din->init();

    /**
     * @brief DOUT Init
     * 
     */
    dout->init();

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
     * @brief USB Host Init
     * 
     */
    ioex_config_t usb_host_conf;

    usb_host_conf.mode = IOEX_OUTPUT;
    usb_host_conf.pull_mode = IOEX_FLOATING;
    usb_host_conf.interrupt_type = IOEX_INTERRUPT_DISABLE;
    usb_host_conf.pin_bit_mask = (1ULL<<CORE_IOEX_PIN_USB_HOST_RESET) | (1ULL<<CORE_IOEX_PIN_VBUS_EN);
    ioex_config(_ioex, &usb_host_conf);
    ioex_set_level(_ioex, CORE_IOEX_PIN_USB_HOST_RESET, IOEX_HIGH);
    ioex_set_level(_ioex, CORE_IOEX_PIN_VBUS_EN, IOEX_LOW);

    usb_host_conf.mode = IOEX_INPUT;
    usb_host_conf.pull_mode = IOEX_PULLUP;
    usb_host_conf.interrupt_type = IOEX_INTERRUPT_DISABLE;
    usb_host_conf.pin_bit_mask = (1ULL<<CORE_IOEX_PIN_VBUS_OC);
    ioex_config(_ioex, &usb_host_conf);

    /**
     * @brief 5V User Init
     *
     */
    ioex_config_t v_user_conf;

    v_user_conf.mode = IOEX_INPUT;
    v_user_conf.pull_mode = IOEX_PULLUP;
    v_user_conf.interrupt_type = IOEX_INTERRUPT_DISABLE;
    v_user_conf.pin_bit_mask = (1ULL<<CORE_IOEX_PIN_5V_USER_PG);
    ioex_config(_ioex, &v_user_conf);

    /* 5V user EN is set as floating input for normal operation. Set as OUTPUT LOW to disable 5V Output */
    v_user_conf.mode = IOEX_INPUT;
    v_user_conf.pull_mode = IOEX_FLOATING;
    v_user_conf.interrupt_type = IOEX_INTERRUPT_DISABLE;
    v_user_conf.pin_bit_mask = (1ULL<<CORE_IOEX_PIN_5V_USER_PG);
    v_user_conf.pin_bit_mask = (1ULL<<CORE_IOEX_PIN_5V_USER_EN);
    ioex_config(_ioex, &v_user_conf);

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

#endif

    ESP_LOGI(CORE_TAG, "Create control task");
    xTaskCreate(_controlTask, "Control task", 4096, NULL, 1, NULL);
}

void CoreStandalone::digitalWrite(DigitalOutputNum_t doutNum, uint8_t level)
{
    dout->digitalWrite(doutNum, level);
}

void CoreStandalone::digitalToggle(DigitalOutputNum_t doutNum)
{
    dout->digitalToggle(doutNum);
}

int CoreStandalone::digitalRead(DigitalInputNum_t dinNum)
{
    return din->digitalRead(dinNum);
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

void CoreStandalone::attachInterrupt(DigitalInputNum_t dinNum, IsrCallback_t callback, InterruptMode_t mode, void* arg)
{
    din->attachInterrupt(dinNum, callback, mode, arg);
}

void CoreStandalone::detachInterrupt(DigitalInputNum_t dinNum)
{
    din->detachInterrupt(dinNum);
}

uint8_t CoreStandalone::getCurrentLevel(DigitalOutputNum_t doutNum)
{
    return dout->getCurrentLevel(doutNum);
}

void CoreStandalone::_controlTask(void *pvParameters)
{

    /* Every 500ms check if there is a power error (5V User or 5V USB)
    If output is in error: desactivate for 5 secondes then retry */

    uint8_t user_power = 0;
    uint8_t usb_power = 0;

    while(1) 
    {

#if defined(CONFIG_IDF_TARGET_ESP32S3)

        /* Checking if user power is in overcurrent */
        // If error happened
        if (ioex_get_level(_ioex, CORE_IOEX_PIN_5V_USER_PG) == 1)
        {
            ESP_LOGE(CORE_TAG, "Overcurrent on 5V User");
            ioex_set_direction(_ioex, CORE_IOEX_PIN_5V_USER_EN, IOEX_OUTPUT);
            ioex_set_level(_ioex, CORE_IOEX_PIN_5V_USER_EN, IOEX_LOW);
            user_power = 1;
        }
        // Retry after 10 loops
        else if (user_power == 10)
        {
            user_power = 0;
            ioex_set_direction(_ioex, CORE_IOEX_PIN_5V_USER_EN, IOEX_INPUT);
        }
        // increase error counter
        else if (user_power != 0)
        {
            user_power++;
        }

        /* Checking if usb power is in overcurrent */
        // If error happened
        if (ioex_get_level(_ioex, CORE_IOEX_PIN_VBUS_OC) == 0)
        {
            ESP_LOGE(CORE_TAG, "Overcurrent on USB Host");
            ioex_set_level(_ioex, CORE_IOEX_PIN_VBUS_EN, IOEX_HIGH);
            usb_power = 1;
        }
        // Retry after 10 loops
        else if (usb_power == 10) 
        {
            usb_power = 0;
            ioex_set_level(_ioex, CORE_IOEX_PIN_VBUS_EN, IOEX_LOW);
        }
        // increase error counter
        else if (usb_power != 0)
        {
            usb_power++;
        }

#endif

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

#endif