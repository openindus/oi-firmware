/**
 * @file AnalogLS.cpp
 * @brief Analog LS
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "AnalogLS.h"

#if defined(OI_ANALOG_LS)

static const char TAG[] = "AnalogLS";

i2c_master_bus_handle_t AnalogLS::_i2cBusHandle = NULL;

int AnalogLS::init(void)
{
    int ret = 0;

    ESP_LOGI(TAG, "Init.");

    /* Module */
    ret |= Module::init(TYPE_OI_ANALOG_LS);

    /* Initialize SPI bus */
    spi_bus_config_t spiConfig = {
        .mosi_io_num = ANALOG_LS_SPI_PIN_MOSI,
        .miso_io_num = ANALOG_LS_SPI_PIN_MISO,
        .sclk_io_num = ANALOG_LS_SPI_PIN_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .data4_io_num = -1,
        .data5_io_num = -1,
        .data6_io_num = -1,
        .data7_io_num = -1,
        .max_transfer_sz = 0,
        .flags = 0,
        .intr_flags = 0
    };
    ret |= spi_bus_initialize(ANALOG_LS_SPI_HOST, &spiConfig, SPI_DMA_CH_AUTO);

    /**
     * @brief I2C init - IOExpander + RTC
     * 
     */
    ESP_LOGI(TAG, "Initializes the bus I2C (I2C_NUM_%u)", CORE_I2C_PORT_NUM);
    ESP_LOGI(TAG, "SDA: GPIO_NUM_%u | SCL: GPIO_NUM_%u",
        CORE_PIN_I2C_SDA, CORE_PIN_I2C_SCL);

    i2c_master_bus_config_t i2cBusConfig = {
        .i2c_port = ANALOG_LS_I2C_PORT_NUM,
        .sda_io_num = ANALOG_LS_I2C_PIN_SDA,
        .scl_io_num = ANALOG_LS_I2C_PIN_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .intr_priority = 0,
        .trans_queue_depth = 0,
        .flags = {
            .enable_internal_pullup = true,
            .allow_pd = false
        }
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2cBusConfig, &_i2cBusHandle));

    /* Initialize analog inputs low signal */
    _adc = new ADS114S0X {
        .device = NULL,
        .config = {
            .host_id = ANALOG_LS_SPI_HOST,
            .sclk_freq = ANALOG_LS_ADC_SPI_FREQ,
            .start_sync = ANALOG_LS_ADC_PIN_START_SYNC,
            .reset = ANALOG_LS_ADC_PIN_RESET,
            .cs = ANALOG_LS_ADC_PIN_CS,
            .drdy = ANALOG_LS_ADC_PIN_DRDY
        }
    };

    _highSideMux = new Multiplexer {
        {ANALOG_LS_MUX_PIN_HS1_A0, ANALOG_LS_MUX_PIN_HS1_A1, ANALOG_LS_MUX_PIN_HS1_A2},
        {ANALOG_LS_MUX_PIN_HS2_A0, ANALOG_LS_MUX_PIN_HS2_A1, ANALOG_LS_MUX_PIN_HS2_A2}
    };

    _lowSideMux = new Multiplexer(
        {ANALOG_LS_MUX_PIN_LS1_A0, ANALOG_LS_MUX_PIN_LS1_A1, ANALOG_LS_MUX_PIN_LS1_A2},
        {ANALOG_LS_MUX_PIN_LS2_A0, ANALOG_LS_MUX_PIN_LS2_A1, ANALOG_LS_MUX_PIN_LS2_A2}
    );

    /* Initialize digital temperature sensor */
    ret |= STDS75_init(_i2cBusHandle, ANALOG_LS_THERM_I2C_ADDR, ANALOG_LS_THERM_PIN_OS_INT);

    ret |= AnalogInputsLS::_init();

#if defined(CONFIG_MODULE_SLAVE)
    ret |= AnalogInputsLSCmdHandler::init();
#endif

    return ret;
}

#endif