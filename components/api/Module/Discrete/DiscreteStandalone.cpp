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
#include "DiscreteConfig.h"

#if defined(CONFIG_DISCRETE) || defined(CONFIG_DISCRETE_VE)

static const char DISCRETE_TAG[] = "Discrete";

gpio_num_t _doutNum[] = {
    DISCRETE_PIN_DOUT_1,
    DISCRETE_PIN_DOUT_2,
    DISCRETE_PIN_DOUT_3,
    DISCRETE_PIN_DOUT_4,
    DISCRETE_PIN_DOUT_5,
    DISCRETE_PIN_DOUT_6,
    DISCRETE_PIN_DOUT_7,
    DISCRETE_PIN_DOUT_8
};

gpio_num_t _dinGpio[] = {
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
    DISCRETE_CHANNEL_AIN_2,
    DISCRETE_CHANNEL_AIN_3
};
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
adc1_channel_t _ainChannel[] = {
    DISCRETE_CHANNEL_AIN_1
};
#endif

esp_adc_cal_characteristics_t* _ainAdcChar;

DigitalInput* DiscreteStandalone::din = new DigitalInput(_dinGpio, 10); 

void DiscreteStandalone::init()
{
    ESP_LOGI(DISCRETE_TAG, "Init");
    ModuleStandalone::init();

    /* Init DOUT */
    gpio_config_t doutConf = DISCRETE_CONFIG_DOUT_DEFAULT();
    DigitalOutput::init(&doutConf, _doutNum);

    din->init();

    /* Init AIN */
    ESP_LOGI(DISCRETE_TAG, "Init AIN");
    for (int i=0; i<AIN_MAX; i++) {
#if defined(CONFIG_IDF_TARGET_ESP32S3)
        ESP_ERROR_CHECK(adc2_config_channel_atten(_ainChannel[i], ADC_ATTEN_DB_11));
// #elif defined(CONFIG_IDF_TARGET_ESP32S2)
//         ESP_ERROR_CHECK(adc1_config_channel_atten(_ainChannel[i], ADC_ATTEN_DB_11));
#endif
    }
}

void DiscreteStandalone::digitalWrite(DigitalOutputNum_t dout, uint8_t level) 
{
    DigitalOutput::digitalWrite(dout, level);
}

int DiscreteStandalone::digitalRead(DigitalInputNum_t dinNum)
{
    return din->digitalRead(dinNum);
}

void DiscreteStandalone::attachInterrupt(DigitalInputNum_t dinNum, IsrCallback_t callback, InterruptMode_t mode, void* arg) 
{
    din->attachInterrupt(dinNum, callback, mode, arg);
}

void DiscreteStandalone::detachInterrupt(DigitalInputNum_t dinNum)
{
    din->detachInterrupt(dinNum);
}

int DiscreteStandalone::analogRead(AnalogInputNum_t ain) 
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

int DiscreteStandalone::analogReadMilliVolts(AnalogInputNum_t ain)
{
    /** @todo */
    return -1;
}

void DiscreteStandalone::analogWrite(DigitalOutputNum_t dout, uint8_t duty) 
{
    DigitalOutput::analogWrite(dout, duty);
}

#endif