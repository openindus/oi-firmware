/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file Mixed.cpp
 * @brief Functions for Mixed module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "Mixed.h"

#if defined(OI_MIXED)

static const char TAG[] = "Mixed";

static const gpio_num_t _dinGpio[] = {
    MIXED_PIN_DIN_1,
    MIXED_PIN_DIN_2,
    MIXED_PIN_DIN_3,
    MIXED_PIN_DIN_4
};

static const gpio_num_t _doutGpio[] = {
    MIXED_PIN_DOUT_1,
    MIXED_PIN_DOUT_2,
    MIXED_PIN_DOUT_3,
    MIXED_PIN_DOUT_4
};

static const AdcNumChannel_t _doutAdcNumChannel[] = {
    MIXED_CHANNEL_DOUT_CURRENT_1,
    MIXED_CHANNEL_DOUT_CURRENT_2,
    MIXED_CHANNEL_DOUT_CURRENT_3,
    MIXED_CHANNEL_DOUT_CURRENT_4
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

int MixedCLI::init(void)
{
    int err = 0;

    err |= AnalogOutputsCLI::init();
    err |= AnalogInputsLVCLI::init();
    err |= DigitalOutputsCLI::init();
    err |= DigitalInputsCLI::init();

    return err;
}

int MixedStandalone::init(void)
{
    int err = 0;

    err |= ModuleStandalone::init();

    /* Initialize digital inputs */
    err |= DigitalInputs::init(_dinGpio, 4);

    /* Initialize digital outputs */
    err |= DigitalOutputs::init(_doutGpio, _doutAdcNumChannel, 4);

    /* Initialize the SPI bus */
    spi_bus_config_t busConfig = {
        .mosi_io_num = MIXED_SPI_PIN_MOSI,
        .miso_io_num = MIXED_SPI_PIN_MISO,
        .sclk_io_num = MIXED_SPI_PIN_SCK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .data4_io_num = -1,
        .data5_io_num = -1,
        .data6_io_num = -1,
        .data7_io_num = -1,
        .max_transfer_sz = 32,
        .flags = 0,
        .intr_flags = 0
    };

    err |= spi_bus_initialize(MIXED_SPI_HOST, &busConfig, SPI_DMA_CH_AUTO);

    /* Initialize analog outputs */
    ad5413_config_t dacConfig[] = {
        {MIXED_SPI_HOST, MIXED_SPI_FREQ, MIXED_DAC_PIN_SYNC_1, MIXED_DAC_PIN_LDAC_1, 0, 0},
        {MIXED_SPI_HOST, MIXED_SPI_FREQ, MIXED_DAC_PIN_SYNC_2, MIXED_DAC_PIN_LDAC_2, 1, 1}
    };
    err |= AnalogOutputs::init(2, dacConfig);
    
    /* Initialize analog inputs */
    err |= AnalogInputsLV::init(&adcSPIConfig, _ainCmdGpio, 4);

    /* Enable analog outputs */
    AnalogOutputs::start();

    return err;
}
#endif

#if defined(OI_MIXED) && defined(MODULE_SLAVE)

int MixedSlave::init(void)
{
    int err = 0;

    err |= ModuleSlave::init();
    err |= MixedStandalone::init();
    err |= AnalogOutputsSlave::init();
    err |= AnalogInputsLVSlave::init();
    err |= DigitalOutputsSlave::init();
    err |= DigitalInputsSlave::init();

    return err;
}

#endif