/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file MixedStandalone.cpp
 * @brief Functions for Mixed module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "MixedStandalone.h"

#if defined(OI_MIXED)

static const char TAG[] = "MixedStandalone";

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

DigitalInputs* MixedStandalone::_dins = new DigitalInputs(_dinGpio, 4);
DigitalOutputs* MixedStandalone::_douts = new DigitalOutputs(_doutGpio, _doutAdcNumChannel, 4);

/* Analog inputs instances */
AnalogInputs* MixedStandalone::_ains = new AnalogInputs(_ainCmdGpio, 4);

/* Analog outputs instances */
AnalogOutput* MixedStandalone::_aout[MIXED_DAC_NB] = {
    new AnalogOutputAD5413(AOUT_1),
    new AnalogOutputAD5413(AOUT_2)
};

int MixedStandalone::init(void)
{
    ModuleStandalone::init();

    /* Initialize digital inputs */
    _dins->init();

    /* Initialize digital outputs */
    _douts->init();

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

    esp_err_t err = spi_bus_initialize(MIXED_SPI_HOST, &busConfig, SPI_DMA_CH_AUTO);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Init SPI bus error");
    }

    /* Initialize analog outputs */
    ad5413_config_t dacConfig[] = {
        {MIXED_SPI_HOST, MIXED_SPI_FREQ, MIXED_DAC_PIN_SYNC_1, MIXED_DAC_PIN_LDAC_1, 0, 0},
        {MIXED_SPI_HOST, MIXED_SPI_FREQ, MIXED_DAC_PIN_SYNC_2, MIXED_DAC_PIN_LDAC_2, 1, 1}
    };
    AnalogOutputAD5413::init(dacConfig, 2);
    _aout[0]->setMode(AOUT_MODE_M10V5_10V5);
    _aout[1]->setMode(AOUT_MODE_M10V5_10V5);

    /* Initialize analog inputs */
    _ains->init(&adcSPIConfig, AIN_VOLTAGE_RANGE_0_10V24, AIN_MODE_VOLTAGE);

    return 0;
}


/*******  Digital Inputs *******/
int MixedStandalone::digitalRead(DigitalInputNum_t num)
{
    return _dins->read(num);
}

void MixedStandalone::attachInterrupt(DigitalInputNum_t num, IsrCallback_t callback, 
    InterruptMode_t mode, void* arg)
{
    _dins->attachInterrupt(num, callback, mode, arg);
}

void MixedStandalone::detachInterrupt(DigitalInputNum_t num)
{
    _dins->detachInterrupt(num);
}

/*******  Digital Outputs *******/
void MixedStandalone::digitalWrite(DigitalOutputNum_t num, uint8_t level)
{
    _douts->write(num, level);
}

void MixedStandalone::digitalToggle(DigitalOutputNum_t num)
{
    _douts->toggle(num);
}

void MixedStandalone::digitalPWM(DigitalOutputNum_t num, uint8_t duty)
{
    /** @todo */
}

float MixedStandalone::digitalGetCurrent(DigitalOutputNum_t num)
{
    return _douts->digitalGetCurrent(num);
}

/*******  Analog Inputs  *******/
void MixedStandalone::analogInputMode(AnalogInput_Num_t num, AnalogInput_Mode_t mode)
{
    _ains->setMode(num, mode);   
}

uint8_t MixedStandalone::analogInputGetMode(AnalogInput_Num_t num)
{
    return _ains->getMode(num);
}

void MixedStandalone::analogInputVoltageRange(AnalogInput_Num_t num, AnalogInput_VoltageRange_t range)
{
    _ains->setVoltageRange(num, range);
}

uint8_t MixedStandalone::analogInputGetVoltageRange(AnalogInput_Num_t num)
{
    return _ains->getVoltageRange(num);
}

int MixedStandalone::analogRead(AnalogInput_Num_t num)
{
    return _ains->read(num);
}

float MixedStandalone::analogReadVolt(AnalogInput_Num_t num)
{
    return _ains->read(num, AIN_UNIT_VOLT);
}

float MixedStandalone::analogReadMilliVolt(AnalogInput_Num_t num)
{
    return _ains->read(num, AIN_UNIT_MILLIVOLT);
}

float MixedStandalone::analogReadAmp(AnalogInput_Num_t num)
{
    return _ains->read(num, AIN_UNIT_AMP);
}

float MixedStandalone::analogReadMilliAmp(AnalogInput_Num_t num)
{
    return _ains->read(num, AIN_UNIT_MILLIAMP);
}


/*******  Analog Outputs  *******/
void MixedStandalone::analogOutputMode(AnalogOutput_Num_t num, AnalogOutput_Mode_t mode)
{
    if (num < MIXED_DAC_NB) {
        _aout[num]->setMode(mode);
    } else {
        ESP_LOGE(TAG, "Invalid Analog output num");
    }
}

void MixedStandalone::analogWrite(AnalogOutput_Num_t num, float value)
{
    if (num < MIXED_DAC_NB) {
        _aout[num]->write(value);
    } else {
        ESP_LOGE(TAG, "Invalid Analog output num");
    }
}

#endif
