/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file BusIO.c
 * @brief this class control the bus
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "BusIO.h"

static const char BUS_IO_TAG[] = "BusIO";

esp_adc_cal_characteristics_t* BusIO::_adcCharsId;
BusIO::Config_t* BusIO::_config;

void BusIO::init(Config_t* config)
{
    /* Config */
    _config = (BusIO::Config_t*)malloc(sizeof(BusIO::Config_t));
    memcpy(_config, config, sizeof(BusIO::Config_t));

    /* OI-ID */
    ESP_LOGI(BUS_IO_TAG, "Init OI-ID");
    adc1_config_width(_config->adcWidthId);
    adc1_config_channel_atten((adc1_channel_t)_config->adcChannelId, ADC_ATTEN_DB_11);
    // Characterize ADC
    _adcCharsId = (esp_adc_cal_characteristics_t*)calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, _config->adcWidthId, 1100, _adcCharsId);

    /* OI-GPIO */
    ESP_LOGI(BUS_IO_TAG, "Init SYNC (OI-GPIO)");
    gpio_config_t gpioConf = {
        .pin_bit_mask = (1ULL<<_config->gpioNumSync),
        .mode = _config->gpioModeSync,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&gpioConf);

    /* Cmd MOSFET Alim */
    ESP_LOGI(BUS_IO_TAG, "Init Command mosfet alim");
    gpio_config_t cmdMosfetAlimConf = {
        .pin_bit_mask = (1ULL<<_config->gpioNumPower),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&cmdMosfetAlimConf);
    gpio_set_level(_config->gpioNumPower, 0);
}

uint32_t BusIO::readId(void)
{
    uint32_t adc_reading = 0;
    for (int i = 0; i < 64; i++)
    {
        adc_reading += adc1_get_raw((adc1_channel_t)_config->adcChannelId);
    }
    adc_reading /= 64;
    return esp_adc_cal_raw_to_voltage(adc_reading, _adcCharsId);
}

void BusIO::powerOn(void)
{
    gpio_set_level(_config->gpioNumPower, 0);
}

void BusIO::powerOff(void)
{
    gpio_set_level(_config->gpioNumPower, 1);
}

uint8_t BusIO::readSync(void)
{
    return gpio_get_level(_config->gpioNumSync);
}

void BusIO::writeSync(uint8_t level)
{
    if (_config->gpioModeSync != GPIO_MODE_INPUT_OUTPUT)
    {
        ESP_LOGW(BUS_IO_TAG, "Sync cannot be write in slave mode !");
    } else {
        gpio_set_level(_config->gpioNumSync, level);
    }
}