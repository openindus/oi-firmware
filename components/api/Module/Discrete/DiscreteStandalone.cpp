/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DiscreteStandalone.cpp
 * @brief Functions for discrete module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "DiscreteStandalone.h"
#include "DiscretePinout.h"

#if defined(OI_DISCRETE) || defined(OI_DISCRETE_VE)

static const char DISCRETE_TAG[] = "Discrete";

const gpio_num_t _doutGpio[] = {
    DISCRETE_PIN_DOUT_1,
    DISCRETE_PIN_DOUT_2,
    DISCRETE_PIN_DOUT_3,
    DISCRETE_PIN_DOUT_4,
    DISCRETE_PIN_DOUT_5,
    DISCRETE_PIN_DOUT_6,
    DISCRETE_PIN_DOUT_7,
    DISCRETE_PIN_DOUT_8
};

const adc1_channel_t _doutAdcChannel[] = {
    DISCRETE_CHANNEL_DOUT_CURRENT_1,
    DISCRETE_CHANNEL_DOUT_CURRENT_2,
    DISCRETE_CHANNEL_DOUT_CURRENT_3,
    DISCRETE_CHANNEL_DOUT_CURRENT_4,
    DISCRETE_CHANNEL_DOUT_CURRENT_5,
    DISCRETE_CHANNEL_DOUT_CURRENT_6,
    DISCRETE_CHANNEL_DOUT_CURRENT_7,
    DISCRETE_CHANNEL_DOUT_CURRENT_8
};

const gpio_num_t _dinGpio[] = {
    DISCRETE_PIN_DIN_1,
    DISCRETE_PIN_DIN_2,
    DISCRETE_PIN_DIN_3,
    DISCRETE_PIN_DIN_4,
    DISCRETE_PIN_DIN_5,
    DISCRETE_PIN_DIN_6,
    DISCRETE_PIN_DIN_7,
    DISCRETE_PIN_DIN_8,
    DISCRETE_PIN_DIN_9,
    DISCRETE_PIN_DIN_10
};

#if defined(CONFIG_IDF_TARGET_ESP32S3)
adc2_channel_t _ainChannel[] = {
    DISCRETE_CHANNEL_AIN_1,
    DISCRETE_CHANNEL_AIN_2
};
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
adc1_channel_t _ainChannel[] = {
    DISCRETE_CHANNEL_AIN_1
};
#endif

esp_adc_cal_characteristics_t* _ainAdcChar;

DigitalInput* DiscreteStandalone::din = new DigitalInput(_dinGpio, 10);
DigitalOutput* DiscreteStandalone::dout = new DigitalOutput(_doutGpio, _doutAdcChannel, 8);

esp_adc_cal_characteristics_t DiscreteStandalone::_adc1Characteristics;

void DiscreteStandalone::init()
{
    ESP_LOGI(DISCRETE_TAG, "Init");
    ModuleStandalone::init();

    /* Init DOUT */
    dout->init();

    /* Init DIN*/
    din->init();

    /* Init AIN */
    ESP_LOGI(DISCRETE_TAG, "Init AIN");
    for (auto i: _ainChannel) {
        ESP_ERROR_CHECK(adc2_config_channel_atten(_ainChannel[i], ADC_ATTEN_DB_11));
    }
    esp_adc_cal_characterize(ADC_UNIT_2, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &_adc1Characteristics);
}

void DiscreteStandalone::digitalWrite(DigitalOutputNum_t doutNum, uint8_t level)
{
    dout->write(doutNum, level);
}

void DiscreteStandalone::digitalToggle(DigitalOutputNum_t doutNum)
{
    dout->toggle(doutNum);
}

int DiscreteStandalone::digitalRead(DigitalInputNum_t dinNum)
{
    return din->read(dinNum);
}

void DiscreteStandalone::attachInterrupt(DigitalInputNum_t dinNum, IsrCallback_t callback, InterruptMode_t mode, void* arg)
{
    din->attachInterrupt(dinNum, callback, mode, arg);
}

void DiscreteStandalone::detachInterrupt(DigitalInputNum_t dinNum)
{
    din->detachInterrupt(dinNum);
}

int DiscreteStandalone::analogRead(AnalogInput_Num_t ain)
{
    int raw = -1;
    if (ain < AIN_MAX) {
#if defined(CONFIG_IDF_TARGET_ESP32S3)
        adc2_get_raw(_ainChannel[ain], (adc_bits_width_t)(ADC_WIDTH_MAX-1), &raw);
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
        raw = adc1_get_raw(_ainChannel[ain]);
#endif
    } else {
        ESP_LOGE(DISCRETE_TAG, "Invalid AIN_%d", ain+1);
    }
    return raw;
}

int DiscreteStandalone::analogReadMilliVolts(AnalogInput_Num_t ain)
{
    int adc_reading = DiscreteStandalone::analogRead(ain);

    // Convert adc_reading to voltage in mV
    uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, &_adc1Characteristics);

    return voltage * DISCRETE_ADC_REDUCTION_FACTOR;
}

void DiscreteStandalone::analogWrite(DigitalOutputNum_t doutNum, uint8_t duty)
{
    /** @todo */
}

float DiscreteStandalone::getCurrent(DigitalOutputNum_t doutNum)
{
    return dout->getCurrent(doutNum);
}

#endif