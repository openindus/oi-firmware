/**
 * @file Discrete.cpp
 * @brief Discrete Module
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "Discrete.h"

#if (defined(CONFIG_OI_DISCRETE) || defined(CONFIG_OI_DISCRETE_VE))

static const char TAG[] = "Discrete";

const gpio_num_t _dinGpio[] = {
    DISCRETE_GPIO_DIN_1,
    DISCRETE_GPIO_DIN_2,
    DISCRETE_GPIO_DIN_3,
    DISCRETE_GPIO_DIN_4,
    DISCRETE_GPIO_DIN_5,
    DISCRETE_GPIO_DIN_6,
    DISCRETE_GPIO_DIN_7,
    DISCRETE_GPIO_DIN_8,
    DISCRETE_GPIO_DIN_9,
    DISCRETE_GPIO_DIN_10
};

const gpio_num_t _doutGpio[] = {
    DISCRETE_GPIO_DOUT_1,
    DISCRETE_GPIO_DOUT_2,
    DISCRETE_GPIO_DOUT_3,
    DISCRETE_GPIO_DOUT_4,
    DISCRETE_GPIO_DOUT_5,
    DISCRETE_GPIO_DOUT_6,
    DISCRETE_GPIO_DOUT_7,
    DISCRETE_GPIO_DOUT_8
};

const adc_unit_t _doutAdcUnits[] = {
    DISCRETE_ADC_UNIT_DOUT,
    DISCRETE_ADC_UNIT_DOUT,
    DISCRETE_ADC_UNIT_DOUT,
    DISCRETE_ADC_UNIT_DOUT,
    DISCRETE_ADC_UNIT_DOUT,
    DISCRETE_ADC_UNIT_DOUT,
    DISCRETE_ADC_UNIT_DOUT,
    DISCRETE_ADC_UNIT_DOUT
};

const adc_channel_t _doutAdcChannels[] = {
    DISCRETE_ADC_CHANNEL_DOUT_1,
    DISCRETE_ADC_CHANNEL_DOUT_2,
    DISCRETE_ADC_CHANNEL_DOUT_3,
    DISCRETE_ADC_CHANNEL_DOUT_4,
    DISCRETE_ADC_CHANNEL_DOUT_5,
    DISCRETE_ADC_CHANNEL_DOUT_6,
    DISCRETE_ADC_CHANNEL_DOUT_7,
    DISCRETE_ADC_CHANNEL_DOUT_8
};
const adc_unit_t _ainUnits = DISCRETE_ADC_UNIT_AIN;

const adc_channel_t _ainChannels[] = {
    DISCRETE_ADC_CHANNEL_AIN_1,
    DISCRETE_ADC_CHANNEL_AIN_2
};

int Discrete::init(void)
{
    int err = 0;
    
    ESP_LOGI(TAG, "Discrete init.");

#if defined(CONFIG_OI_DISCRETE)
    err |= Module::init(TYPE_OI_DISCRETE);
#elif defined(CONFIG_OI_DISCRETE_VE)
    err |= Module::init(TYPE_OI_DISCRETE_VE);
#endif

    /* Initialize digital and analog IOs */
    err |= DigitalOutputs::init(_doutGpio, _doutAdcUnits, _doutAdcChannels, 8, adcHandles[0], adcHandles[1]);
    err |= DigitalInputs::init(_dinGpio, 10);
    err |= AnalogInputsHV::init(_ainUnits, _ainChannels, 2, adcHandles[0]);

#if defined(CONFIG_MODULE_SLAVE)
    err |= AnalogInputsHVCmdHandler::init();
    err |= DigitalOutputsCmdHandler::init();
    err |= DigitalInputsCmdHandler::init();
#endif

    /* CLI */
    err |= DigitalInputsCLI::init();
    err |= DigitalOutputsCLI::init();

    return err;
}

#endif