/**
 * @file Stepper.cpp
 * @brief Stepper class implementation
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "Stepper.h"

#if defined(OI_STEPPER) || defined(OI_STEPPER_VE)

static const char TAG[] = "Stepper";

const gpio_num_t _dinGpio[] = {
    STEPPER_GPIO_PIN_DIN_1, STEPPER_GPIO_PIN_DIN_2, STEPPER_GPIO_PIN_DIN_3, STEPPER_GPIO_PIN_DIN_4,
#if defined(OI_STEPPER_VE)
    STEPPER_GPIO_PIN_DIN_5, STEPPER_GPIO_PIN_DIN_6
#endif
};

static const std::map<DIn_Num_t, gpio_num_t> dinGpioMap = {
    {DIN_1, STEPPER_GPIO_PIN_DIN_1},
    {DIN_2, STEPPER_GPIO_PIN_DIN_2},
    {DIN_3, STEPPER_GPIO_PIN_DIN_3},
    {DIN_4, STEPPER_GPIO_PIN_DIN_4},
#if defined(OI_STEPPER_VE)
    {DIN_5, STEPPER_GPIO_PIN_DIN_5},
    {DIN_6, STEPPER_GPIO_PIN_DIN_6}
#endif
};

Encoder *Stepper::encoder[STEPPER_ENCODER_MAX] = {nullptr};

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
    err |= DigitalInputs::init(_dinGpio, sizeof(_dinGpio) / sizeof(gpio_num_t));

    /* Stepper motor */
    PS01_Hal_Config_t ps01Conf = STEPPER_CONFIG_MOTOR_DEFAULT();
    PS01_Param_t ps01Param     = STEPPER_PARAM_MOTOR_DEFAULT();
    err |= MotorStepper::init(&ps01Conf, &ps01Param);
    err |= MotorStepperParam::initNVSParam();

    /* Encoder */
    for (int i = 0; i < STEPPER_ENCODER_MAX; i++) {
        encoder[i] = new Encoder(i, dinGpioMap);
    }

#if defined(MODULE_SLAVE)
    err |= DigitalInputsCmdHandler::init();
    err |= MotorStepperCmdHandler::init();
    err |= EncoderCmdHandler::init(encoder);
#endif

    return err;
}

#endif