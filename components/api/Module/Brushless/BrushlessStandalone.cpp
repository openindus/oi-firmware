/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file BrushlessStandalone.h
 * @brief Functions for Brushless module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "BrushlessStandalone.h"
#include "BrushlessConfig.h"

#if defined(OI_BRUSHLESS)

static const char BRUSHLESS_TAG[] = "Brushless";

gpio_num_t _doutNum[] = {
    BRUSHLESS_PIN_DOUT_1,
    BRUSHLESS_PIN_DOUT_2
};

gpio_num_t _dinNum[] = {
    BRUSHLESS_PIN_DIN_1,
    BRUSHLESS_PIN_DIN_2
};

int BrushlessStandalone::init(void)
{
    int err = 0;

    ESP_LOGI(BRUSHLESS_TAG, "Init");
    err |= ModuleStandalone::init();
    vTaskDelay(10 / portTICK_PERIOD_MS);

    /* Init du moteur*/
    err |= MotorBLDC::init(BRUSHLESS_PIN_BRAKE, BRUSHLESS_PIN_DIRECTION, BRUSHLESS_PIN_ENABLE_CHIP, BRUSHLESS_PIN_SPEED_CTRL, BRUSHLESS_PIN_NFAULT, BRUSHLESS_PIN_FGOUT);
    vTaskDelay(10 / portTICK_PERIOD_MS);

    /* Init de l'encoder*/
    err |= Encoder::init(BRUSHLESS_PIN_CODEURA, BRUSHLESS_PIN_CODEURB);
    vTaskDelay(10 / portTICK_PERIOD_MS);

    /* Init DOUT */
    gpio_config_t doutConf = BRUSHLESS_CONFIG_DOUT_DEFAULT();
    err |= DigitalOutputs::init(&doutConf, _doutNum);

    DigitalOutputs::digitalWrite(DOUT_1,1);
    vTaskDelay(50 / portTICK_PERIOD_MS);

    return err;
}

void BrushlessStandalone::setSpeed(uint32_t duty_cycle)
{
    MotorBLDC::setSpeed(duty_cycle);    // the duty cycle need to be set between 0 (0%) and 510(100%) (because of PWM declaration)
}

void BrushlessStandalone::setBrake(bool brake)
{
    MotorBLDC::setBrake(brake);
}

void BrushlessStandalone::setDirection(bool direction)
{
    MotorBLDC::setDirection(direction);
}

float BrushlessStandalone::getSpeed(void)
{
    return MotorBLDC::getSpeed();
}

bool BrushlessStandalone::getDirection(void)
{
    return Encoder::getDirection();
}

float BrushlessStandalone::getSpeedEncoder(void)
{
    return Encoder::getSpeed();
}

float BrushlessStandalone::getPosition(void)
{
    return Encoder::getPosition();
}
#endif
