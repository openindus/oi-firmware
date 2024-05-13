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

#if defined(OI_CORE)

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

const AdcNumChannel_t _ainChannel[] = {
    CORE_CHANNEL_AIN_1,
    CORE_CHANNEL_AIN_2
};

ioex_device_t* CoreStandalone::_ioex;

OI::CAN CoreStandalone::can(CORE_SPI_USER_HOST, CORE_PIN_CAN_SPI_CS, CORE_PIN_CAN_INTERRUPT);
OI::RS CoreStandalone::rs(CORE_SPI_USER_HOST, CORE_PIN_RS_SPI_CS, CORE_PIN_RS_INTERRUPT);

int CoreStandalone::init()
{
    int err = ModuleStandalone::init();

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

    err |= i2c_param_config(CORE_I2C_PORT_NUM, &i2c_config);
    err |= i2c_driver_install(CORE_I2C_PORT_NUM, i2c_config.mode, 0, 0, 0);

    /**
     * @brief SPI Init - RS User + CAN User
     * 
     */
    ESP_LOGI(CORE_TAG, "Initializes the bus SPI%u", CORE_SPI_USER_HOST+1);
    ESP_LOGI(CORE_TAG, "MOSI:  GPIO_NUM_%u | MISO:  GPIO_NUM_%u | CLK:  GPIO_NUM_%u",
        CORE_PIN_SPI_USER_MOSI, CORE_PIN_SPI_USER_MISO, CORE_PIN_SPI_USER_SCK);

    spi_bus_config_t spi_user_config;
    spi_user_config.mosi_io_num = CORE_PIN_SPI_USER_MOSI;
    spi_user_config.miso_io_num = CORE_PIN_SPI_USER_MISO;
    spi_user_config.sclk_io_num = CORE_PIN_SPI_USER_SCK;
    spi_user_config.quadwp_io_num = -1;
    spi_user_config.quadhd_io_num = -1;
    spi_user_config.data4_io_num = -1;
    spi_user_config.data5_io_num = -1;
    spi_user_config.data6_io_num = -1;
    spi_user_config.data7_io_num = -1;
    spi_user_config.max_transfer_sz = 0;
    spi_user_config.flags = 0;
    spi_user_config.intr_flags = 0;

    err |= spi_bus_initialize(CORE_SPI_USER_HOST, &spi_user_config, SPI_DMA_CH_AUTO);

    /**
     * @brief SPI Init - SD Card + Ethernet + USB Host
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
    spi_config.data4_io_num = -1;
    spi_config.data5_io_num = -1;
    spi_config.data6_io_num = -1;
    spi_config.data7_io_num = -1;
    spi_config.max_transfer_sz = 0;
    spi_config.flags = 0;
    spi_config.intr_flags = 0;

    err |= spi_bus_initialize(CORE_SPI_HOST, &spi_config, SPI_DMA_CH_AUTO);

    /**
     * @brief IO Expander init
     * 
     */
    _ioex = ioex_create(CORE_I2C_PORT_NUM, CORE_I2C_IOEXPANDER_ADDRESS, true, CORE_PIN_DIGITAL_INTERRUPT);
    if (_ioex == NULL) {
        ESP_LOGE(CORE_TAG, "Cannot initialize ioexpander");
        err |= -1;
        return err;
    }

    
    /**
     * @brief Command mosfet alim Init
     * 
     */

    // /!\ Set level before setting to output
    err |= ioex_set_level(_ioex, CORE_IOEX_PIN_ALIM_AUX, IOEX_HIGH);

    ioex_config_t config;
    config.mode = IOEX_OUTPUT;
    config.pull_mode = IOEX_FLOATING;
    config.interrupt_type = IOEX_INTERRUPT_DISABLE;
    config.pin_bit_mask = (1ULL<<CORE_IOEX_PIN_ALIM_AUX);
    err |= ioex_config(_ioex, &config);

    /**
     * @brief DIN Init
     * 
     */
    err |= DigitalInputs::init(&_ioex, _dinGpio, 4);


    /**
     * @brief DOUT Init
     * 
     */
    err |= DigitalOutputs::init(&_ioex, _doutGpio, _doutCurrentGpio, 4);

    /**
     * @brief AIN Init 
     * 
     */
    err |= AnalogInputsHV::init(_ainChannel, 2);

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
    err |= ioex_set_level(_ioex, CORE_IOEX_PIN_CAN_RESET, IOEX_HIGH);
    vTaskDelay(10);
    err |= ioex_set_level(_ioex, CORE_IOEX_PIN_CAN_RESET, IOEX_LOW);
    vTaskDelay(10);
    err |= ioex_set_level(_ioex, CORE_IOEX_PIN_CAN_RESET, IOEX_HIGH);

    /**
     * @brief RS Ext Init
     * 
     */
    ioex_config_t io_rs_conf;

    io_rs_conf.mode = IOEX_OUTPUT;
    io_rs_conf.pull_mode = IOEX_FLOATING;
    io_rs_conf.interrupt_type = IOEX_INTERRUPT_DISABLE;
    io_rs_conf.pin_bit_mask = (1ULL<<CORE_IOEX_PIN_RS_RESET);
    err |= ioex_config(_ioex, &io_rs_conf);
    err |= ioex_set_level(_ioex, CORE_IOEX_PIN_RS_RESET, IOEX_HIGH);

    /**
     * @brief USB Host Init
     * 
     */
    ioex_config_t usb_host_conf;

    usb_host_conf.mode = IOEX_OUTPUT;
    usb_host_conf.pull_mode = IOEX_FLOATING;
    usb_host_conf.interrupt_type = IOEX_INTERRUPT_DISABLE;
    usb_host_conf.pin_bit_mask = (1ULL<<CORE_IOEX_PIN_USB_HOST_RESET) | (1ULL<<CORE_IOEX_PIN_VBUS_EN);
    err |= ioex_config(_ioex, &usb_host_conf);
    err |= ioex_set_level(_ioex, CORE_IOEX_PIN_USB_HOST_RESET, IOEX_HIGH);
    err |= ioex_set_level(_ioex, CORE_IOEX_PIN_VBUS_EN, IOEX_LOW);

    usb_host_conf.mode = IOEX_INPUT;
    usb_host_conf.pull_mode = IOEX_PULLUP;
    usb_host_conf.interrupt_type = IOEX_INTERRUPT_DISABLE;
    usb_host_conf.pin_bit_mask = (1ULL<<CORE_IOEX_PIN_VBUS_OC);
    err |= ioex_config(_ioex, &usb_host_conf);

    /**
     * @brief 5V User Init
     *
     */
    ioex_config_t v_user_conf;

    v_user_conf.mode = IOEX_INPUT;
    v_user_conf.pull_mode = IOEX_PULLUP;
    v_user_conf.interrupt_type = IOEX_INTERRUPT_DISABLE;
    v_user_conf.pin_bit_mask = (1ULL<<CORE_IOEX_PIN_5V_USER_PG);
    err |= ioex_config(_ioex, &v_user_conf);

    /* 5V user EN is set as floating input for normal operation. Set as OUTPUT LOW to disable 5V Output */
    v_user_conf.mode = IOEX_INPUT;
    v_user_conf.pull_mode = IOEX_FLOATING;
    v_user_conf.interrupt_type = IOEX_INTERRUPT_DISABLE;
    v_user_conf.pin_bit_mask = (1ULL<<CORE_IOEX_PIN_5V_USER_PG);
    v_user_conf.pin_bit_mask = (1ULL<<CORE_IOEX_PIN_5V_USER_EN);
    err |= ioex_config(_ioex, &v_user_conf);

    /**
     * @brief RTC Init
     * Configure IOExpander RTC Interrupt pin
     */

    ioex_config_t io_rtc_conf;
    io_rtc_conf.mode = IOEX_INPUT;
    io_rtc_conf.pull_mode = IOEX_PULLUP;
    io_rtc_conf.interrupt_type = IOEX_INTERRUPT_NEGEDGE;
    io_rtc_conf.pin_bit_mask = (1ULL<<CORE_PIN_RTC_INTERRUPT);

    err |= ioex_config(_ioex, &io_rtc_conf);

    ESP_LOGI(CORE_TAG, "Create control task");
    xTaskCreate(_controlTask, "Control task", 4096, NULL, 1, NULL);

    /* Init RTC */
    RTC.init(CORE_I2C_PORT_NUM);

    return err;
}

void CoreStandalone::_controlTask(void *pvParameters)
{

    /* Every 500ms check if there is a power error (5V User or 5V USB)
    If output is in error: desactivate for 5 secondes then retry */

    uint8_t user_power = 0;
    uint8_t usb_power = 0;

    while(1) 
    {

        /* Checking if user power is in overcurrent */
        // // If error happened
        // if (ioex_get_level(_ioex, CORE_IOEX_PIN_5V_USER_PG) == 0)
        // {
        //     ESP_LOGE(CORE_TAG, "Overcurrent on 5V User");
        //     ioex_set_direction(_ioex, CORE_IOEX_PIN_5V_USER_EN, IOEX_OUTPUT);
        //     ioex_set_level(_ioex, CORE_IOEX_PIN_5V_USER_EN, IOEX_LOW);
        //     user_power = 1;
        // }
        // // Retry after 10 loops
        // else if (user_power == 10)
        // {
        //     user_power = 0;
        //     ioex_set_direction(_ioex, CORE_IOEX_PIN_5V_USER_EN, IOEX_INPUT);
        // }
        // // increase error counter
        // else if (user_power != 0)
        // {
        //     user_power++;
        // }

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
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

#endif