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

    /* Initialize I2C bus */
    i2c_config_t i2c_cfg = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = ANALOG_LS_I2C_PIN_SDA,
        .scl_io_num = ANALOG_LS_I2C_PIN_SCL,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master = {
            .clk_speed = 400000,
        },
        .clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL
    };
    ret |= i2c_param_config(ANALOG_LS_I2C_PORT_NUM, &i2c_cfg);
    ret |= i2c_driver_install(ANALOG_LS_I2C_PORT_NUM, i2c_cfg.mode, 0, 0, 0);


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
    ret |= STDS75_init(ANALOG_LS_I2C_PORT_NUM, ANALOG_LS_THERM_I2C_ADDR, ANALOG_LS_THERM_PIN_OS_INT);

    ret |= AnalogInputsLS::_init();

#if defined(MODULE_SLAVE)
    ret |= AnalogInputsLSCmdHandler::init();
#endif

    return ret;
}

#endif