/**
 * @file Stepper.cpp
 * @brief Stepper
 * @author
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "Stepper.h"

#if defined(OI_STEPPER) || defined(OI_STEPPER_VE)

static const char TAG[] = "Stepper";

const gpio_num_t _dinGpio[] = {
    STEPPER_GPIO_PIN_DIN_1,
    STEPPER_GPIO_PIN_DIN_2,
    STEPPER_GPIO_PIN_DIN_3,
    STEPPER_GPIO_PIN_DIN_4,
#if defined(OI_STEPPER_VE)
    STEPPER_GPIO_PIN_DIN_5,
    STEPPER_GPIO_PIN_DIN_6
#endif
};

int Stepper::init(void)
{
    int err = 0;

    ESP_LOGI(TAG, "Stepper init.");

#if defined(OI_STEPPER)
    err |= Module::init(TYPE_OI_STEPPER);
#elif defined(OI_STEPPER_VE)
    err |= Module::init(TYPE_OI_STEPPER_VE);
#endif

    /* Digital inputs */
    err |= DigitalInputs::init(_dinGpio, sizeof(_dinGpio)/sizeof(gpio_num_t));

    /* Stepper motor */
    PS01_Hal_Config_t ps01Conf = STEPPER_CONFIG_MOTOR_DEFAULT();
    PS01_Param_t ps01Param = STEPPER_PARAM_MOTOR_DEFAULT();
    err |= MotorStepper::init(&ps01Conf, &ps01Param);

    /* Parameters */
    err |= MotorStepperParam::initNVSParam();

#if defined(MODULE_SLAVE)
    err |= DigitalInputsCmdHandler::init();
    err |= StepperCmdHandler::init();
#endif

    return err;
}

#endif