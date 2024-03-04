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
#include "MixedPinout.h"

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

static ad5413_config_t _dacConfig[] = {
    {MIXED_SPI_HOST, MIXED_SPI_FREQ, MIXED_DAC_PIN_SYNC_1, 0, 0},
    {MIXED_SPI_HOST, MIXED_SPI_FREQ, MIXED_DAC_PIN_SYNC_2, 1, 1}
};

uint32_t _ainToNum[] = {2, 3, 1, 0};

DigitalInput* MixedStandalone::_din = new DigitalInput(_dinGpio, 4);
DigitalOutput* MixedStandalone::_dout = new DigitalOutput(_doutGpio, _doutAdcChannel, 4);

/* Analog outputs instances */
AnalogOutput* MixedStandalone::_aout[2] = {
    new AnalogOutputAD5413(&_dacConfig[0]),
    new AnalogOutputAD5413(&_dacConfig[1])
};

int MixedStandalone::init(void)
{
    ModuleStandalone::init();

    /* Initialize digital inputs */
    _din->init();

    /* Initialize digital outputs */
    _dout->init();

    /* Initialize the SPI bus */
    spi_bus_config_t busCfg = {
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

    esp_err_t err = spi_bus_initialize(MIXED_SPI_HOST, &busCfg, SPI_DMA_CH_AUTO);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Init SPI bus error");
    }

    // /* Init AIN */
    // ESP_LOGI(TAG, "initializing AIN");
    // Ads866x_DeviceConfig_t ads866xDeviceConfig = {
    //     .spi_host = MIXED_SPI_HOST,
    //     .spi_freq = MIXED_SPI_FREQ,
    //     .spi_pin_cs = MIXED_ADC_PIN_CS,
    //     .pin_rst = MIXED_ADC_PIN_RST,
    //     .pin_mode = {MIXED_ADC_PIN_MODE_AIN_1, 
    //                  MIXED_ADC_PIN_MODE_AIN_2,
    //                  MIXED_ADC_PIN_MODE_AIN_3,
    //                  MIXED_ADC_PIN_MODE_AIN_4},
    //     .adc_analogs_nb = MIXED_ADC_NB,
    //     .adc_res = MIXED_ADC_DEFAULT_RES_BITS,
    //     .adc_mode = {ADS866X_VOLTAGE_MODE,
    //                  ADS866X_VOLTAGE_MODE,
    //                  ADS866X_VOLTAGE_MODE,
    //                  ADS866X_VOLTAGE_MODE}
    // };

    // Ads866x_DeviceConfig(&ads866xDeviceConfig);
    // Ads866x_Init();

    /* Initialize analog outputs */
    _aout[0]->init();
    _aout[1]->init();

    return 0;
}

int MixedStandalone::digitalRead(DigitalInputNum_t num)
{
    return _din->digitalRead(num);
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

void MixedStandalone::digitalWrite(DigitalOutputNum_t num, uint8_t level)
{
    _dout->digitalWrite(num, level);
}

void MixedStandalone::digitalToggle(DigitalOutputNum_t num)
{
    _dout->digitalToggle(num);
}

void MixedStandalone::analogWrite(DigitalOutputNum_t num, uint8_t duty)
{
    _dout->analogWrite(num, duty);
}

float MixedStandalone::getCurrent(DigitalOutputNum_t num)
{
    return _dout->getCurrent(num);
}

int MixedStandalone::analogRead(AnalogInputNum_t num)
{
    return (int)Ads866x_AnalogRead(_ainToNum[num]);
}

int MixedStandalone::analogReadMilliVolts(AnalogInputNum_t num)
{
    return (int)Ads866x_AnalogReadUnits(_ainToNum[num], ADS866x_UNITS_MILLIVOLTS);
}

void MixedStandalone::analogReadMode(AnalogInputNum_t num, AdcMode_t mode)
{
    Ads866x_setAdcMode(_ainToNum[num], (Ads866x_AdcMode_t)mode);
}

void MixedStandalone::analogReadResolution(AdcResBits_t res)
{
    Ads866x_setAdcResolution((Ads866x_AdcResolutionBits_t)res);
}

void MixedStandalone::analogReadReference(float ref)
{
    Ads866x_setAnalogReference(ref);
}

void MixedStandalone::analogOutputMode(AnalogOutput_Num_t num, AnalogOutput_Mode_t mode)
{
    if (num < 2) {
        _aout[num]->setMode(mode);
    } else {
        ESP_LOGE(TAG, "Invalid Analog output num");
    }
}

void MixedStandalone::analogWrite(AnalogOutput_Num_t num, float value)
{
    if (num < 2) {
        _aout[num]->write(value);
    } else {
        ESP_LOGE(TAG, "Invalid Analog output num");
    }
}

#endif
