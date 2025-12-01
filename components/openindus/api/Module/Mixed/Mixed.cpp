/**
 * @file Mixed.cpp
 * @brief Mixed Module
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "Mixed.h"
#include "MixedPinout.h"

#if defined(CONFIG_OI_MIXED)

static const char TAG[] = "Mixed";

static const gpio_num_t _dinGpio[] = {
    MIXED_GPIO_DIN_1,
    MIXED_GPIO_DIN_2,
    MIXED_GPIO_DIN_3,
    MIXED_GPIO_DIN_4
};

static const gpio_num_t _doutGpio[] = {
    MIXED_GPIO_DOUT_1,
    MIXED_GPIO_DOUT_2,
    MIXED_GPIO_DOUT_3,
    MIXED_GPIO_DOUT_4
};

static const adc_channel_t _doutAdcChannels[] = {
    MIXED_ADC_CHANNEL_DOUT_1,
    MIXED_ADC_CHANNEL_DOUT_2,
    MIXED_ADC_CHANNEL_DOUT_3,
    MIXED_ADC_CHANNEL_DOUT_4
};

static const adc_unit_t _doutAdcUnits[] = {
    MIXED_ADC_UNIT_DOUT_1,
    MIXED_ADC_UNIT_DOUT_2,
    MIXED_ADC_UNIT_DOUT_3,
    MIXED_ADC_UNIT_DOUT_4
};

static const ain_num_t _ainChannel[] = {
    MIXED_PIN_EANA_1,
    MIXED_PIN_EANA_2,
    MIXED_PIN_EANA_3,
    MIXED_PIN_EANA_4
};

static const gpio_num_t _ainCmdGpio[] = {
    MIXED_PIN_EANA_CMD_1,
    MIXED_PIN_EANA_CMD_2,
    MIXED_PIN_EANA_CMD_3,
    MIXED_PIN_EANA_CMD_4
};

static ads866x_config_t adcSPIConfig = {
    .spi_host = MIXED_SPI_HOST,
    .spi_freq = MIXED_SPI_FREQ,
    .spi_pin_cs = MIXED_ADC_PIN_CS,
    .pin_rst = MIXED_ADC_PIN_RST,
    .pin_alarm = MIXED_ADC_PIN_ALARM,
    .adc_channel_nb = MIXED_ADC_NB
};

int Mixed::init(void)
{
    int err = 0;

    err |= Module::init(TYPE_OI_MIXED);

    /* Initialize digital IOs */
    err |= DigitalInputs::init(_dinGpio, sizeof(_dinGpio)/sizeof(_dinGpio[0])); // 4
    err |= DigitalOutputs::init(_doutGpio, _doutAdcUnits, _doutAdcChannels, sizeof(_doutGpio)/sizeof(_doutGpio[0]), adcHandles[0], adcHandles[1]); // 4

    /* Initialize the SPI bus */
    spi_bus_config_t busConfig = {};
    busConfig.mosi_io_num = MIXED_SPI_PIN_MOSI;
    busConfig.miso_io_num = MIXED_SPI_PIN_MISO;
    busConfig.sclk_io_num = MIXED_SPI_PIN_SCK;
    busConfig.quadwp_io_num = -1;
    busConfig.quadhd_io_num = -1;
    busConfig.data4_io_num = -1;
    busConfig.data5_io_num = -1;
    busConfig.data6_io_num = -1;
    busConfig.data7_io_num = -1;
    busConfig.max_transfer_sz = 32;
    busConfig.flags = 0;
    err |= spi_bus_initialize(MIXED_SPI_HOST, &busConfig, SPI_DMA_CH_AUTO);

    /* Initialize analog outputs */
    ad5413_config_t dacConfig[] = {
        {MIXED_SPI_HOST, MIXED_SPI_FREQ, MIXED_DAC_PIN_SYNC_1, MIXED_DAC_PIN_LDAC_1, 0, 0},
        {MIXED_SPI_HOST, MIXED_SPI_FREQ, MIXED_DAC_PIN_SYNC_2, MIXED_DAC_PIN_LDAC_2, 1, 1}
    };
    err |= AnalogOutputs::init(2, dacConfig);
    
    /* Initialize analog inputs */
    err |= AnalogInputsLV::init(&adcSPIConfig, _ainChannel, _ainCmdGpio, 4);

#if defined(CONFIG_MODULE_SLAVE)
    err |= AnalogOutputsCmdHandler::init();
    err |= AnalogInputsLVCmdHandler::init();
    err |= DigitalOutputsCmdHandler::init();
    err |= DigitalInputsCmdHandler::init();
#endif

    /* CLI */
    err |= DigitalInputsCLI::init();
    err |= DigitalOutputsCLI::init();

    return err;
}

#endif