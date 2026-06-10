/**
 * @file Dc.cpp
 * @brief DC Motor Module
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2026] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "Dc.h"

#if defined(CONFIG_OI_DC)

#include "DcConfig.h"
#include "DigitalInputsCLI.h"

static const char TAG[] = "Dc";

const gpio_num_t dinGpio[] = {
    DC_GPIO_PIN_DIN_1,
    DC_GPIO_PIN_DIN_2,
    DC_GPIO_PIN_DIN_3,
    DC_GPIO_PIN_DIN_4,
    DC_GPIO_PIN_DIN_5,
    DC_GPIO_PIN_DIN_6,
    DC_GPIO_PIN_DIN_7,
    DC_GPIO_PIN_DIN_8
};

static const std::map<DinNum_t, gpio_num_t> dinGpioMap = {
    {DIN_1, DC_GPIO_PIN_DIN_1},
    {DIN_2, DC_GPIO_PIN_DIN_2},
    {DIN_3, DC_GPIO_PIN_DIN_3},
    {DIN_4, DC_GPIO_PIN_DIN_4},
    {DIN_5, DC_GPIO_PIN_DIN_5},
    {DIN_6, DC_GPIO_PIN_DIN_6},
    {DIN_7, DC_GPIO_PIN_DIN_7},
    {DIN_8, DC_GPIO_PIN_DIN_8}
};

Encoder *Dc::encoder[DC_ENCODER_MAX] = {nullptr};

int Dc::init(void)
{
    int err = 0;

    ESP_LOGI(TAG, "Dc Module initialization");

    err |= Module::init(TYPE_OI_DC);

    /* Digital inputs */
    err |= DigitalInputs::init(dinGpio, sizeof(dinGpio)/sizeof(dinGpio[0]));

    /* DC motor */
    std::vector<MotorDcPinConfig_t> motorsConfig;
    motorsConfig.push_back({DC_MOTOR1_IN1, LEDC_CHANNEL_0, DC_MOTOR1_IN2, LEDC_CHANNEL_1, DC_MOTOR1_DISABLE});
    motorsConfig.push_back({DC_MOTOR2_IN1, LEDC_CHANNEL_2, DC_MOTOR2_IN2, LEDC_CHANNEL_3, DC_MOTOR2_DISABLE});
    motorsConfig.push_back({DC_MOTOR3_IN1, LEDC_CHANNEL_4, DC_MOTOR3_IN2, LEDC_CHANNEL_5, DC_MOTOR3_DISABLE});
    motorsConfig.push_back({DC_MOTOR4_IN1, LEDC_CHANNEL_6, DC_MOTOR4_IN2, LEDC_CHANNEL_7, DC_MOTOR4_DISABLE});
    err |= MotorDcPidCtrl::init(motorsConfig, DC_MOTOR_FAULT);

    /* Encoder */
    for (int i = 0; i < DC_ENCODER_MAX; i++) {
        encoder[i] = new Encoder(i, dinGpioMap);
    }

#if defined(CONFIG_MODULE_SLAVE)
    err |= DigitalInputsCmdHandler::init();
    err |= MotorDcPidCtrlCmdHandler::init(encoder);
    err |= EncoderCmdHandler::init(encoder);
#endif

    err |= DigitalInputsCLI::init();

    return err;
}

#endif