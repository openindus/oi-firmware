/**
 * @file BusIO.cpp
 * @brief Bus Control
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "BusIO.h"

static const char BUS_IO_TAG[] = "BusIO";

BusIO::Config_s* BusIO::_config;

int BusIO::init(Config_s* config)
{
    int err = 0;

    _config = (BusIO::Config_s*)malloc(sizeof(BusIO::Config_s));
    memcpy(_config, config, sizeof(BusIO::Config_s));

    /* OI-ID */
    ESP_LOGI(BUS_IO_TAG, "Init ADC channel for OI-ID");
    // Configure ADC channel
    adc_oneshot_chan_cfg_t adcConfig = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12,
    };
    err |= adc_oneshot_config_channel(_config->adcHandle, _config->adcChannelId, &adcConfig);
    
    // Initialize ADC calibration
    adc_cali_curve_fitting_config_t adcCaliConfig = {
        .unit_id = ADC_UNIT_1,
        .chan = _config->adcChannelId,
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12,
    };
    err |= adc_cali_create_scheme_curve_fitting(&adcCaliConfig, &_config->adcCaliHandle);

    /* OI-GPIO */
    ESP_LOGI(BUS_IO_TAG, "Init GPIO for OI-SYNC");
    gpio_config_t gpioConfig = {
        .pin_bit_mask = (1ULL<<_config->gpioNumSync),
        .mode = _config->gpioModeSync,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    err |= gpio_config(&gpioConfig);

    /* Cmd MOSFET Alim */
    ESP_LOGI(BUS_IO_TAG, "Init Command mosfet alim");
    gpio_config_t cmdMosfetAlimConfig = {
        .pin_bit_mask = (1ULL<<_config->gpioNumPower),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    err |= gpio_config(&cmdMosfetAlimConfig);
    err |= gpio_set_level(_config->gpioNumPower, 0);

    return err;
}

uint32_t BusIO::readId(void)
{
    int adc_reading = 0;
    int raw_value = 0;
    int voltage = 0;
    
    // Take multiple readings for averaging
    for (int i = 0; i < 64; i++) {
        adc_oneshot_read(_config->adcHandle, _config->adcChannelId, &raw_value);
        adc_reading += raw_value;
    }
    adc_reading /= 64;
    
    // Convert raw value to voltage
    adc_cali_raw_to_voltage(_config->adcCaliHandle, adc_reading, &voltage);
    return voltage;
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

void BusIO::toggleSync(void)
{
    BusIO::writeSync(0);
    // vTaskDelay(100 / portTICK_PERIOD_MS);
    BusIO::writeSync(1);
    BusIO::readSync();
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