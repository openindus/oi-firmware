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

const gpio_num_t _dinGpio[] = {
    MIXED_PIN_DIN_1,
    MIXED_PIN_DIN_2,
    MIXED_PIN_DIN_3,
    MIXED_PIN_DIN_4
};

const gpio_num_t _doutGpio[] = {
    MIXED_PIN_DOUT_1,
    MIXED_PIN_DOUT_2,
    MIXED_PIN_DOUT_3,
    MIXED_PIN_DOUT_4
};

const adc1_channel_t _doutAdcChannel[] = {
    MIXED_CHANNEL_DOUT_CURRENT_1,
    MIXED_CHANNEL_DOUT_CURRENT_2,
    MIXED_CHANNEL_DOUT_CURRENT_3,
    MIXED_CHANNEL_DOUT_CURRENT_4
};

uint32_t _ainToNum[] = {2, 3, 1, 0};

DigitalInput* MixedStandalone::din = new DigitalInput(_dinGpio, 4);
DigitalOutput* MixedStandalone::dout = new DigitalOutput(_doutGpio, _doutAdcChannel, 4);

void MixedStandalone::init()
{
    ModuleStandalone::init();

    /* Init DOUT */
    dout->init();

    /* Init DIN */
    din->init();

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

    // /* Init AOUT */
    // ESP_LOGI(TAG, "initializing AOUT");
    // Dac8760_DeviceConfig_t dac8760DeviceConfig =  {
    //     .spi_host = MIXED_SPI_HOST,        
    //     .spi_freq = MIXED_SPI_FREQ,
    //     .spi_pin_cs = {MIXED_DAC_PIN_CS1, MIXED_DAC_PIN_CS2},        
    //     .nb_devices = MIXED_DAC_NB,
    //     .conf = {}
    // };
    // Dac8760_DeviceConfig(&dac8760DeviceConfig);

    // Dac8760_Init();

    // /* Init pwm for analog write */
    // ledc_timer_config_t ledc_timer; 
    // ledc_timer.duty_resolution = LEDC_TIMER_8_BIT;
    // ledc_timer.freq_hz = 50;
    // ledc_timer.speed_mode = LEDC_LOW_SPEED_MODE;
    // ledc_timer.timer_num = LEDC_TIMER_1;
    // ledc_timer.clk_cfg = LEDC_AUTO_CLK;
    // ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
    // gpio_install_isr_service(ESP_INTR_FLAG_LEVEL3);
}

void MixedStandalone::digitalWrite(DigitalOutputNum_t doutNum, uint8_t level)
{
    dout->digitalWrite(doutNum, level);
}

void MixedStandalone::digitalToggle(DigitalOutputNum_t doutNum)
{
    dout->digitalToggle(doutNum);
}

int MixedStandalone::digitalRead(DigitalInputNum_t dinNum)
{
    return din->digitalRead(dinNum);
}

void MixedStandalone::attachInterrupt(DigitalInputNum_t dinNum, IsrCallback_t callback, InterruptMode_t mode, void* arg)
{
    din->attachInterrupt(dinNum, callback, mode, arg);
}

void MixedStandalone::detachInterrupt(DigitalInputNum_t dinNum)
{
    din->detachInterrupt(dinNum);
}

int MixedStandalone::analogRead(AnalogInputNum_t ana)
{
    return (int)Ads866x_AnalogRead(_ainToNum[ana]);
}

int MixedStandalone::analogReadMilliVolts(AnalogInputNum_t ana)
{
    return (int)Ads866x_AnalogReadUnits(_ainToNum[ana], ADS866x_UNITS_MILLIVOLTS);
}

void MixedStandalone::analogReadMode(AnalogInputNum_t ana, AdcMode_t mode)
{
    Ads866x_setAdcMode(_ainToNum[ana], (Ads866x_AdcMode_t)mode);
}

void MixedStandalone::analogReadResolution(AdcResBits_t res)
{
    Ads866x_setAdcResolution((Ads866x_AdcResolutionBits_t)res);
}

void MixedStandalone::analogReadReference(float ref)
{
    Ads866x_setAnalogReference(ref);
}

void MixedStandalone::analogWriteVoltage(AnalogOutputNum_t sana, uint32_t value) 
{
    Dac8760_VoltageWrite(sana, value, DAC8760_UNITS_RAW);
}

void MixedStandalone::analogWriteVoltageMilliVolts(AnalogOutputNum_t sana, uint32_t value)
{
    Dac8760_VoltageWrite(sana, value, DAC8760_UNITS_MILLIVOLTS);
}

void MixedStandalone::analogWriteVoltageMode(AnalogOutputNum_t sana, DacVoltageMode_t mode)
{
    Dac8760_setVoltageMode(sana, (Dac8760_VoltageMode_t)mode);
}

void MixedStandalone::analogWriteCurrent(AnalogOutputNum_t sana, uint32_t value)
{
    Dac8760_CurrentWrite(sana, value, DAC8760_UNITS_RAW);
}

void MixedStandalone::analogWriteCurrentMilliAmps(AnalogOutputNum_t sana, uint32_t value)
{
    Dac8760_CurrentWrite(sana, value, DAC8760_UNITS_MILLIAMPS);
}

void MixedStandalone::analogWriteCurrentMode(AnalogOutputNum_t sana, DacCurrentMode_t mode)
{
    Dac8760_setCurrentMode(sana, (Dac8760_CurrentMode_t) mode);
}

void MixedStandalone::analogWrite(DigitalOutputNum_t doutNum, uint8_t duty)
{
    dout->analogWrite(doutNum, duty);
}

float MixedStandalone::getCurrent(DigitalOutputNum_t doutNum)
{
    return dout->getCurrent(doutNum);
}

#endif