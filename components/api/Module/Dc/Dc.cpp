/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file Dc.cpp
 * @brief Functions for dc module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "Dc.h"

#if defined(OI_DC)

static const char TAG[] = "Dc";

const gpio_num_t _dinGpio[] = {
    DC_GPIO_PIN_DIN_1,
    DC_GPIO_PIN_DIN_2,
    DC_GPIO_PIN_DIN_3,
    DC_GPIO_PIN_DIN_4
};

int Dc::init(void)
{
    int err = 0;

    ESP_LOGI(TAG, "Dc init.");

    err |= Module::init(TYPE_OI_DC);

    /* Digital inputs */
    err |= DigitalInputs::init(_dinGpio, 4);

    /* Stepper motor */
    std::vector<MotorDC_PinConfig_t> motorsConfig;
    motorsConfig.push_back({DC_MOTOR1_IN1, LEDC_CHANNEL_0, DC_MOTOR1_IN2, LEDC_CHANNEL_1, DC_MOTOR1_DISABLE});
    motorsConfig.push_back({DC_MOTOR2_IN1, LEDC_CHANNEL_2, DC_MOTOR2_IN2, LEDC_CHANNEL_3, DC_MOTOR2_DISABLE});
    motorsConfig.push_back({DC_MOTOR3_IN1, LEDC_CHANNEL_4, DC_MOTOR3_IN2, LEDC_CHANNEL_5, DC_MOTOR3_DISABLE});
    motorsConfig.push_back({DC_MOTOR4_IN1, LEDC_CHANNEL_6, DC_MOTOR4_IN2, LEDC_CHANNEL_7, DC_MOTOR4_DISABLE});
    err |= MotorDc::init(motorsConfig, DC_MOTOR_FAULT);

#if defined(MODULE_SLAVE)
    err |= DigitalInputsCmdHandler::init();
    err |= DcCmdHandler::init();
#endif

    return err;
}

#endif