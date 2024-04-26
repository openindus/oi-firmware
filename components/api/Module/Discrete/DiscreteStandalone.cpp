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

adc_channel_t _ainChannel[] = {
    DISCRETE_CHANNEL_AIN_1,
    DISCRETE_CHANNEL_AIN_2
};

DigitalInputs* DiscreteStandalone::din = new DigitalInputs(_dinGpio, 10);
DigitalOutputs* DiscreteStandalone::dout = new DigitalOutputs(_doutGpio, _doutAdcChannel, 8);

void DiscreteStandalone::init()
{
    ESP_LOGI(DISCRETE_TAG, "Init");

    ModuleStandalone::init();

    /**
     * @brief DOUT Init
     * 
     */
    dout->init();

    /**
     * @brief DIN Init
     * 
     */
    din->init();

    /**
     * @brief AIN Init
     * 
     */
    AnalogInputs::init(_ainChannel, DISCRETE_ADC_UNIT_AIN, 2);
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


int DiscreteStandalone::analogRead(AnalogInput_Num_t ainNum)
{
    return (int) ain->read(ainNum, AIN_UNIT_RAW);
}

float DiscreteStandalone::analogReadVolt(AnalogInput_Num_t ainNum)
{
    return ain->read(ainNum, AIN_UNIT_VOLT);
}

float DiscreteStandalone::analogReadMilliVolt(AnalogInput_Num_t ainNum)
{
    return ain->read(ainNum, AIN_UNIT_MILLIVOLT);
}

void DiscreteStandalone::digitalPWM(DigitalOutputNum_t doutNum, uint8_t duty)
{
    /** @todo */
}

float DiscreteStandalone::digitalGetCurrent(DigitalOutputNum_t doutNum)
{
    return dout->digitalGetCurrent(doutNum);
}

#endif