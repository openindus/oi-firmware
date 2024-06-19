/**
 * @file Discrete.cpp
 * @brief Discrete
 * @author
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "Discrete.h"

#if (defined(OI_DISCRETE) || defined(OI_DISCRETE_VE))

static const char TAG[] = "Discrete";

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

const adc_num_t _doutAdcChannel[] = {
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

adc_num_t _ainChannel[] = {
    DISCRETE_CHANNEL_AIN_1,
    DISCRETE_CHANNEL_AIN_2
};

int Discrete::init(void)
{
    int err = 0;
    
    ESP_LOGI(TAG, "Discrete init.");

#if defined(OI_DISCRETE)
    err |= Module::init(TYPE_OI_DISCRETE);
#elif defined(OI_DISCRETE_VE)
    err |= Module::init(TYPE_OI_DISCRETE_VE);
#endif
    err |= DigitalOutputs::init(_doutGpio, _doutAdcChannel, 8);
    err |= DigitalInputs::init(_dinGpio, 10);
    err |= AnalogInputsHV::init(_ainChannel, 2);

#if defined(MODULE_SLAVE)
    err |= ResponseAInHV::init();
    err |= ResponseDOut::init();
    err |= ResponseDIn::init();
#endif

    return err;
}

#endif