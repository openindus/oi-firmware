/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file StepperStandalone.h
 * @brief Functions for stepper module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "StepperStandalone.h"
#include "StepperPinout.h"
#include "StepperParam.h"

#if defined(OI_STEPPER) || defined(OI_STEPPER_VE)

static const char STEPPER_TAG[] = "Stepper";

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

int StepperStandalone::init()
{
    int err = 0;

    ESP_LOGI(STEPPER_TAG, "Init");

    err |= ModuleStandalone::init();

    /**
     * @brief DIN Init
     * 
     */
    err |= DigitalInputs::init(_dinGpio, STEPPER_DIN_NUM);

    /**
     * @brief Init Motor stepper
     * 
     */
    ESP_LOGI(STEPPER_TAG, "Init Motor stepper");

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