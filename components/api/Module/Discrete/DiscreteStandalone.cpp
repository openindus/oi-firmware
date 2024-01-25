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

gpio_num_t _storNum[] = {
    DISCRETE_PIN_DOUT_1,
    DISCRETE_PIN_DOUT_2,
    DISCRETE_PIN_DOUT_3,
    DISCRETE_PIN_DOUT_4,
    DISCRETE_PIN_DOUT_5,
    DISCRETE_PIN_DOUT_6,
    DISCRETE_PIN_DOUT_7,
    DISCRETE_PIN_DOUT_8
};

gpio_num_t _etorGpio[] = {
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
adc2_channel_t _eanaChannel[] = {
    DISCRETE_CHANNEL_AIN_1,
    DISCRETE_CHANNEL_AIN_2,
    DISCRETE_CHANNEL_AIN_3
};
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
adc1_channel_t _eanaChannel[] = {
    DISCRETE_CHANNEL_AIN_1
};
#endif

esp_adc_cal_characteristics_t* _eanaAdcChar;

DigitalInput* DiscreteStandalone::etor = new DigitalInput(_etorGpio, 10); 

void DiscreteStandalone::init()
{
    ESP_LOGI(DISCRETE_TAG, "Init");
    ModuleStandalone::init();

    /* Init DOUT */
    gpio_config_t storConf = DISCRETE_CONFIG_DOUT_DEFAULT();
    DigitalOutput::init(&storConf, _storNum);

    etor->init();

    /* Init AIN */
    ESP_LOGI(DISCRETE_TAG, "Init AIN");
    for (int i=0; i<AIN_MAX; i++) {
#if defined(CONFIG_IDF_TARGET_ESP32S3)
        ESP_ERROR_CHECK(adc2_config_channel_atten(_eanaChannel[i], ADC_ATTEN_DB_11));
// #elif defined(CONFIG_IDF_TARGET_ESP32S2)
//         ESP_ERROR_CHECK(adc1_config_channel_atten(_eanaChannel[i], ADC_ATTEN_DB_11));
#endif
    }
}

void DiscreteStandalone::digitalWrite(DigitalOutputNum_t stor, uint8_t level) 
{
    DigitalOutput::digitalWrite(stor, level);
}

int DiscreteStandalone::digitalRead(DigitalInputNum_t etorNum)
{
    return etor->digitalRead(etorNum);
}

void DiscreteStandalone::attachInterrupt(DigitalInputNum_t etorNum, IsrCallback_t callback, InterruptMode_t mode, void* arg) 
{
    etor->attachInterrupt(etorNum, callback, mode, arg);
}

void DiscreteStandalone::detachInterrupt(DigitalInputNum_t etorNum)
{
    etor->detachInterrupt(etorNum);
}

int DiscreteStandalone::analogRead(AnalogInputNum_t eana) 
{
    int raw = -1;
    if (eana < AIN_MAX) {
#if defined(CONFIG_IDF_TARGET_ESP32S3)
        adc2_get_raw(_eanaChannel[eana], (adc_bits_width_t)(ADC_WIDTH_MAX-1), &raw);
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
        raw = adc1_get_raw(_eanaChannel[eana]);
#endif
    } else {
        ESP_LOGE(DISCRETE_TAG, "Invalid AIN_%d", eana+1);
    }
    return raw;
}

int DiscreteStandalone::analogReadMilliVolts(AnalogInputNum_t eana)
{
    /** @todo */
    return -1;
}

void DiscreteStandalone::analogWrite(DigitalOutputNum_t stor, uint8_t duty) 
{
    DigitalOutput::analogWrite(stor, duty);
}

#endif