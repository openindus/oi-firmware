/**
 * @file Core.cpp
 * @brief Module Core
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "Core.h"
#include "CorePinout.h"
#include "MotorStepperCmd.h"
#include "DigitalOutputsCmd.h"
#include "DigitalInputsCLI.h"
#include "DigitalOutputsCLI.h"
#include "RSCLI.h"
#include "CANCLI.h"

#if defined(CONFIG_OI_CORE)

static const char TAG[] = "Core";

static const ioex_num_t _doutGpio[] = {
    CORE_IOEX_PIN_DOUT_1,
    CORE_IOEX_PIN_DOUT_2,
    CORE_IOEX_PIN_DOUT_3,
    CORE_IOEX_PIN_DOUT_4,
};

static const ioex_num_t _doutCurrentGpio[] = {
    CORE_IOEX_PIN_DOUT_CURRENT_1,
    CORE_IOEX_PIN_DOUT_CURRENT_2,
    CORE_IOEX_PIN_DOUT_CURRENT_3,
    CORE_IOEX_PIN_DOUT_CURRENT_4,
};

static const ioex_num_t _dinGpio[] = {
    CORE_IOEX_PIN_DIN_1,
    CORE_IOEX_PIN_DIN_2,
    CORE_IOEX_PIN_DIN_3,
    CORE_IOEX_PIN_DIN_4,
};

static const adc_channel_t _ainChannel[] = {
    CORE_CHANNEL_AIN_1,
    CORE_CHANNEL_AIN_2
};

ioex_device_t* Core::_ioex = NULL;
CAN Core::can(CORE_SPI_USER_HOST, CORE_PIN_CAN_SPI_CS, CORE_PIN_CAN_INTERRUPT);
RS Core::rs(CORE_SPI_USER_HOST, CORE_PIN_RS_SPI_CS, CORE_PIN_RS_INTERRUPT);
RTClock Core::rtc(CORE_I2C_PORT_NUM, CORE_PIN_RTC_INTERRUPT);
Modem *Core::modem = NULL;

int Core::init(void)
{
    int err = Module::init(TYPE_OI_CORE);

    /**
     * @brief I2C init
     * 
     */
    ESP_LOGI(TAG, "Initializes the bus I2C (I2C_NUM_%u)", CORE_I2C_PORT_NUM);
    ESP_LOGI(TAG, "SDA: GPIO_NUM_%u | SCL: GPIO_NUM_%u",
        CORE_PIN_I2C_SDA, CORE_PIN_I2C_SCL);

    i2c_config_t i2cConfig;
	memset(&i2cConfig, 0, sizeof(i2c_config_t));
    i2cConfig.mode = I2C_MODE_MASTER;
    i2cConfig.sda_io_num = CORE_PIN_I2C_SDA;
    i2cConfig.sda_pullup_en = GPIO_PULLUP_ENABLE;
    i2cConfig.scl_io_num = CORE_PIN_I2C_SCL;
    i2cConfig.scl_pullup_en = GPIO_PULLUP_ENABLE;
    i2cConfig.master.clk_speed = CORE_DEFAULT_I2C_SPEED;
    err |= i2c_param_config(CORE_I2C_PORT_NUM, &i2cConfig);
    err |= i2c_driver_install(CORE_I2C_PORT_NUM, i2cConfig.mode, 0, 0, 0);

    /**
     * @brief SPI Init - RS User + CAN User
     * 
     */
    ESP_LOGI(TAG, "Initializes the bus SPI%u", CORE_SPI_USER_HOST+1);
    ESP_LOGI(TAG, "MOSI:  GPIO_NUM_%u | MISO:  GPIO_NUM_%u | CLK:  GPIO_NUM_%u",
        CORE_PIN_SPI_USER_MOSI, CORE_PIN_SPI_USER_MISO, CORE_PIN_SPI_USER_SCK);

    spi_bus_config_t spiConfig1;
    spiConfig1.mosi_io_num = CORE_PIN_SPI_USER_MOSI;
    spiConfig1.miso_io_num = CORE_PIN_SPI_USER_MISO;
    spiConfig1.sclk_io_num = CORE_PIN_SPI_USER_SCK;
    spiConfig1.quadwp_io_num = -1;
    spiConfig1.quadhd_io_num = -1;
    spiConfig1.data4_io_num = -1;
    spiConfig1.data5_io_num = -1;
    spiConfig1.data6_io_num = -1;
    spiConfig1.data7_io_num = -1;
    spiConfig1.max_transfer_sz = 0;
    spiConfig1.flags = 0;
    spiConfig1.isr_cpu_id = ESP_INTR_CPU_AFFINITY_AUTO;
    spiConfig1.intr_flags = 0;
    err |= spi_bus_initialize(CORE_SPI_USER_HOST, &spiConfig1, SPI_DMA_CH_AUTO);

    /**
     * @brief SPI Init - SD Card + Ethernet + USB Host
     * 
     */
    ESP_LOGI(TAG, "Initializes the bus SPI%u", CORE_SPI_HOST+1);
    ESP_LOGI(TAG, "MOSI:  GPIO_NUM_%u | MISO:  GPIO_NUM_%u | CLK:  GPIO_NUM_%u",
        CORE_PIN_SPI_MOSI, CORE_PIN_SPI_MISO, CORE_PIN_SPI_SCK);

    spi_bus_config_t spiConfig2;
    spiConfig2.mosi_io_num = CORE_PIN_SPI_MOSI;
    spiConfig2.miso_io_num = CORE_PIN_SPI_MISO;
    spiConfig2.sclk_io_num = CORE_PIN_SPI_SCK;
    spiConfig2.quadwp_io_num = -1;
    spiConfig2.quadhd_io_num = -1;
    spiConfig2.data4_io_num = -1;
    spiConfig2.data5_io_num = -1;
    spiConfig2.data6_io_num = -1;
    spiConfig2.data7_io_num = -1;
    spiConfig2.max_transfer_sz = 0;
    spiConfig2.flags = 0;
    spiConfig1.isr_cpu_id = ESP_INTR_CPU_AFFINITY_AUTO;
    spiConfig2.intr_flags = 0;
    err |= spi_bus_initialize(CORE_SPI_HOST, &spiConfig2, SPI_DMA_CH_AUTO);

    /**
     * @brief IO Init
     * 
     */
    ESP_LOGI(TAG, "Initializes GPIO and IO Expander (PCAL6524)");
    _ioex = ioex_create(CORE_I2C_PORT_NUM, CORE_I2C_IOEXPANDER_ADDRESS, true, CORE_PIN_DIGITAL_INTERRUPT);
    if (_ioex == NULL) {
        ESP_LOGE(TAG, "Cannot initialize ioexpander");
        err |= -1;
        return err;
    }
    err |= ioex_set_level(_ioex, CORE_IOEX_PIN_ALIM_AUX, IOEX_HIGH); // Set level before setting to output

    /* Outputs */
    ioex_config_t ioexConfig1;
    ioexConfig1.mode = IOEX_OUTPUT;
    ioexConfig1.pull_mode = IOEX_FLOATING;
    ioexConfig1.interrupt_type = IOEX_INTERRUPT_DISABLE;
    ioexConfig1.pin_bit_mask = (1ULL << CORE_MODEM_PIN_RESET) | 
                               (1ULL << CORE_IOEX_PIN_ALIM_AUX) | 
                               (1ULL << CORE_IOEX_PIN_CAN_RESET) | 
                               (1ULL << CORE_IOEX_PIN_RS_RESET) |
                               (1ULL << CORE_IOEX_PIN_USB_HOST_RESET) | 
                               (1ULL << CORE_IOEX_PIN_VBUS_EN);
    err |= ioex_config(_ioex, &ioexConfig1);

    // Set default levels
    err |= ioex_set_level(_ioex, CORE_MODEM_PIN_RESET, IOEX_LOW);
    err |= ioex_set_level(_ioex, CORE_IOEX_PIN_CAN_RESET, IOEX_LOW);
    err |= ioex_set_level(_ioex, CORE_IOEX_PIN_RS_RESET, IOEX_HIGH);
    err |= ioex_set_level(_ioex, CORE_IOEX_PIN_USB_HOST_RESET, IOEX_HIGH);
    err |= ioex_set_level(_ioex, CORE_IOEX_PIN_VBUS_EN, IOEX_LOW);
    vTaskDelay(1);
    err |= ioex_set_level(_ioex, CORE_IOEX_PIN_CAN_RESET, IOEX_HIGH);

    /* Inputs */
    ioex_config_t ioexConfig2;
    ioexConfig2.mode = IOEX_INPUT;
    ioexConfig2.pull_mode = IOEX_FLOATING;
    ioexConfig2.interrupt_type = IOEX_INTERRUPT_DISABLE;
    ioexConfig2.pin_bit_mask = (1ULL << CORE_IOEX_PIN_5V_USER_PG);
    err |= ioex_config(_ioex, &ioexConfig2);

    ioexConfig2.pull_mode = IOEX_PULLUP;
    ioexConfig2.pin_bit_mask = (1ULL << CORE_IOEX_PIN_VBUS_OC);
    err |= ioex_config(_ioex, &ioexConfig2);

    gpio_config_t gpioConfig;
    gpioConfig.mode = GPIO_MODE_INPUT;
    gpioConfig.pull_up_en = GPIO_PULLUP_ENABLE;
    gpioConfig.intr_type = GPIO_INTR_NEGEDGE;
    gpioConfig.pin_bit_mask = (1ULL << CORE_PIN_RTC_INTERRUPT);
    err |= gpio_config(&gpioConfig);


    /**
     * @brief Modem Init
     * 
     */
    ESP_LOGI(TAG, "Initializes the modem");
    Modem::setHwConfig(CORE_MODEM_UART, CORE_MODEM_PIN_TX, CORE_MODEM_PIN_RX, CORE_MODEM_PIN_POWER_ON);
    
    /**
     * @brief DIN, DOUT, AIN Init
     * 
     */
    err |= DigitalInputs::init(&_ioex, _dinGpio, 4);
    err |= DigitalOutputs::init(&_ioex, _doutGpio, _doutCurrentGpio, 4);
    err |= AnalogInputsHV::init(ADC_UNIT_1, _ainChannel, 2, adcHandles[0]);

    ESP_LOGI(TAG, "Create a power monitoring task");
    xTaskCreate(_powerMonitoringTask, "Power monitoring task", 4096, NULL, 1, NULL);

    /* Command line interface */
    err |= DigitalInputsCLI::init();
    err |= DigitalOutputsCLI::init();
    err |= RSCLI::init();
    err |= CANCLI::init();
#if defined(CONFIG_MODULE_MASTER)
    MotorStepperCmd::_registerCLI();
#endif

    return err;
}

/**
 * @brief Every 500ms check if there is a power error (5V User or 5V USB)
 * If output is in error: desactivate for 5 secondes then retry
 * 
 * @param pvParameters 
 */
void Core::_powerMonitoringTask(void *pvParameters)
{
    uint8_t usb_power = 0;

    while(1) {
        // /* Checking if user power is in overcurrent */
        // if (ioex_get_level(_ioex, CORE_IOEX_PIN_5V_USER_PG) == 0) { // If error happened
        //     ESP_LOGE(TAG, "Overcurrent on 5V User");
        //     ioex_set_direction(_ioex, CORE_IOEX_PIN_5V_USER_EN, IOEX_OUTPUT);
        //     ioex_set_level(_ioex, CORE_IOEX_PIN_5V_USER_EN, IOEX_LOW);
        //     user_power = 1;
        // } else if (user_power == 10) { // Retry after 10 loops
        //     user_power = 0;
        //     ioex_set_direction(_ioex, CORE_IOEX_PIN_5V_USER_EN, IOEX_INPUT);
        // } else if (user_power != 0) { // increase error counter
        //     user_power++;
        // }

        /* Checking if usb power is in overcurrent */
        if (ioex_get_level(_ioex, CORE_IOEX_PIN_VBUS_OC) == 0) { // If error happened
            ESP_LOGE(TAG, "Overcurrent on USB Host");
            ioex_set_level(_ioex, CORE_IOEX_PIN_VBUS_EN, IOEX_HIGH);
            usb_power = 1;
        } else if (usb_power == 10) { // Retry after 10 loops
            usb_power = 0;
            ioex_set_level(_ioex, CORE_IOEX_PIN_VBUS_EN, IOEX_LOW);
        } else if (usb_power != 0) { // increase error counter
            usb_power++;
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

#endif