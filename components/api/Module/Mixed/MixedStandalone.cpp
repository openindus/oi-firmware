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

static const adc1_channel_t _doutAdcChannel[] = {
    MIXED_CHANNEL_DOUT_CURRENT_1,
    MIXED_CHANNEL_DOUT_CURRENT_2,
    MIXED_CHANNEL_DOUT_CURRENT_3,
    MIXED_CHANNEL_DOUT_CURRENT_4
};

DigitalInput* MixedStandalone::_din = new DigitalInput(_dinGpio, 4);
DigitalOutput* MixedStandalone::_dout = new DigitalOutput(_doutGpio, _doutAdcChannel, 4);

/* Analog inputs instances */
AnalogInput* MixedStandalone::_ain[MIXED_ADC_NB] = {
    new AnalogInputAds866x(AIN_3),
    new AnalogInputAds866x(AIN_4),
    new AnalogInputAds866x(AIN_2),
    new AnalogInputAds866x(AIN_1),
};

/* Analog outputs instances */
AnalogOutput* MixedStandalone::_aout[MIXED_DAC_NB] = {
    new AnalogOutputAD5413(AIN_1),
    new AnalogOutputAD5413(AIN_2)
};

int MixedStandalone::init(void)
{
    ModuleStandalone::init();

    /* Initialize digital inputs */
    _din->init();

    /* Initialize digital outputs */
    _dout->init();

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

    /* Initialize analog inputs */
    Ads866x_DeviceConfig_t adcConfig = {
        .spi_host = MIXED_SPI_HOST,
        .spi_freq = MIXED_SPI_FREQ,
        .spi_pin_cs = MIXED_ADC_PIN_CS,
        .pin_rst = MIXED_ADC_PIN_RST,
        .pin_mode = {},
        .adc_analogs_nb = MIXED_ADC_NB,
        .adc_res = 12,
        .adc_mode = {1, 1, 1, 1},
        .adc_range = {}
    };
    AnalogInputAds866x::init(&adcConfig);

    /* Initialize analog outputs */
    ad5413_config_t dacConfig[] = {
        {MIXED_SPI_HOST, MIXED_SPI_FREQ, MIXED_DAC_PIN_SYNC_1, 0, 0},
        {MIXED_SPI_HOST, MIXED_SPI_FREQ, MIXED_DAC_PIN_SYNC_2, 1, 1}
    };
    AnalogOutputAD5413::init(dacConfig, 2);

    return 0;
}



/*******  Digital Inputs *******/
int MixedStandalone::digitalRead(DigitalInputNum_t num)
{
    return _din->read(num);
}

void MixedStandalone::attachInterrupt(DigitalInputNum_t num, IsrCallback_t callback, 
    InterruptMode_t mode, void* arg)
{
    _din->attachInterrupt(num, callback, mode, arg);
}

void MixedStandalone::detachInterrupt(DigitalInputNum_t num)
{
    _din->detachInterrupt(num);
}

/*******  Digital Outputs *******/
void MixedStandalone::digitalWrite(DigitalOutputNum_t num, uint8_t level)
{
    _dout->write(num, level);
}

void MixedStandalone::digitalToggle(DigitalOutputNum_t num)
{
    _dout->toggle(num);
}

void MixedStandalone::analogWrite(DigitalOutputNum_t num, uint8_t duty)
{
    /** @todo */
}

float MixedStandalone::getCurrent(DigitalOutputNum_t num)
{
    return _dout->getCurrent(num);
}

/*******  Analog Inputs *******/
void MixedStandalone::analogInputMode(AnalogInput_Num_t num, AnalogInput_Mode_t mode)
{
    if (num < MIXED_ADC_NB) {
        _ain[num]->setMode(mode);
    } else {
        ESP_LOGE(TAG, "Invalid Analog input num");
    }    
}

void MixedStandalone::analogInputVoltageRange(AnalogInput_Num_t num, AnalogInput_VoltageRange_t range)
{
    if (num < MIXED_ADC_NB) {
        _ain[num]->setVoltageRange(range);
    } else {
        ESP_LOGE(TAG, "Invalid Analog input num");
    }    
}

void MixedStandalone::analogInputResolution(AnalogInput_Resolution_t res)
{
    _ain[0]->setResolution(res);
}

int MixedStandalone::analogRead(AnalogInput_Num_t num)
{
    int value = 0;
    if (num < MIXED_ADC_NB) {
        value = _ain[num]->read();
    } else {
        ESP_LOGE(TAG, "Invalid Analog input num");
    }
    return value;
}

float MixedStandalone::analogReadMilliVolts(AnalogInput_Num_t num)
{
    float value = 0;
    if (num < MIXED_ADC_NB) {
        value = _ain[num]->read(AIN_UNIT_MILLIVOLTS);
    } else {
        ESP_LOGE(TAG, "Invalid Analog input num");
    }
    return value;
}

/*******  Analog Outputs *******/
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
