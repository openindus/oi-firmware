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

int StepperStandalone::init(void)
{
    int err = 0;

    ESP_LOGI(TAG, "Stepper init.");

#if defined(OI_STEPPER)
    err |= ModuleStandalone::init(TYPE_OI_STEPPER);
#elif defined(OI_STEPPER_VE)
    err |= ModuleStandalone::init(TYPE_OI_STEPPER_VE);
#endif

    /* Digital inputs */
    err |= DigitalInputs::init(_dinGpio, 4);

    /* Stepper motor */
    PS01_Hal_Config_t ps01Conf = STEPPER_CONFIG_MOTOR_DEFAULT();
    PS01_Param_t ps01Param = STEPPER_PARAM_MOTOR_DEFAULT();
    err |= MotorStepper::init(&ps01Conf, &ps01Param);

    /* Parameters */
    err |= MotorStepperParam::initNVSParam();

    /* CLI */
    err |= StepperCLI::init();

    return err;
}

int StepperCLI::init(void)
{
    int err = 0;

    /* Command line interface */
    err |= CLI_DIn::init();
    err |= CLI_Stepper::init();
    err |= CLI_StepperParam::init();

    return err;
}

#endif

#if (defined(OI_STEPPER) || defined(OI_STEPPER_VE)) && defined(MODULE_SLAVE)

int StepperSlave::init(void)
{
    int err = 0;

    err |= StepperStandalone::init();
    err |= Slave::init();
    err |= ResponseDIn::init();
    err |= ResponseStepper::init();

    return err;
}

#endif
