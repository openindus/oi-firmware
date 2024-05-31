/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file Stepper.cpp
 * @brief Functions for Stepper module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#if defined(OI_STEPPER) || defined(OI_STEPPER_VE)

#include "Stepper.h"

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

int StepperCLI::init(void)
{
    int err = 0;

    err |= CLI_DIn::init();
    err |= CLI_Stepper::init();
    err |= CLI_StepperParam::init();

    return err;
}

int StepperStandalone::init(void)
{
    int err = 0;

#if defined(OI_STEPPER)
    err |= ModuleStandalone::init(TYPE_OI_STEPPER);
#elif defined(OI_STEPPER_VE)
    err |= ModuleStandalone::init(TYPE_OI_STEPPER_VE);
#endif

    /**
     * @brief DIN Init
     * 
     */
    err |= DigitalInputs::init(_dinGpio, 4);

/**
     * @brief Init Motor stepper
     * 
     */
    ESP_LOGI(TAG, "Init Motor stepper");

    PS01_Hal_Config_t ps01Conf = STEPPER_CONFIG_MOTOR_DEFAULT();
    PS01_Param_t ps01Param = STEPPER_PARAM_MOTOR_DEFAULT();
    err |= MotorStepper::init(&ps01Conf, &ps01Param);

    /**
     * @brief Init Motor stepper NVS param
     * 
     */
    err |= MotorStepperParam::initNVSParam();

    return err;
}

#endif

#if (defined(OI_STEPPER) || defined(OI_STEPPER_VE)) && defined(MODULE_SLAVE)

int StepperSlave::init(void)
{
    int err = 0;

    err |= ControlSlave::init();
    err |= StepperStandalone::init();
    err |= ControlSlave_DIn::init();
    err |= ControlSlave_Stepper::init();

    return err;
}

#endif
