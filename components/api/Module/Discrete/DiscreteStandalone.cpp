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

const AdcNumChannel_t _doutAdcChannel[] = {
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

AdcNumChannel_t _ainChannel[] = {
    DISCRETE_CHANNEL_AIN_1,
    DISCRETE_CHANNEL_AIN_2
};

int DiscreteStandalone::init()
{
    int err = 0;
    
    ESP_LOGI(DISCRETE_TAG, "Init");

    err |= ModuleStandalone::init();

    /**
     * @brief DOUT Init
     * 
     */
    err |= DigitalOutputs::init(_doutGpio, _doutAdcChannel, 8);

    /**
     * @brief DIN Init
     * 
     */
    err |= DigitalInputs::init(_dinGpio, 10);

    /**
     * @brief AIN Init
     * 
     */
    err |= AnalogInputsHV::init(_ainChannel, 2);

    return err;
}

#endif