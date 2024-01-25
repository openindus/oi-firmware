/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DigitalInput.c
 * @brief Functions for etor
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "DigitalInput.h"

static const char DIN_TAG[] = "DigitalInput";

gpio_num_t DigitalInput::_gpio[DIN_MAX];
IsrCallback_t DigitalInput::_callback[DIN_MAX];
void* DigitalInput::_arg[DIN_MAX];
xQueueHandle DigitalInput::_event;

void DigitalInput::init(void)
{
    ESP_LOGI(DIN_TAG, "Init");

    /* Init GPIO service */
    ESP_ERROR_CHECK(gpio_install_isr_service(0));

    /* Init Gpio */
    for (int i=0; i<DIN_MAX; i++) {
        if (_gpio[i] != GPIO_NUM_NC) {
            ESP_ERROR_CHECK(gpio_set_direction(_gpio[i], GPIO_MODE_INPUT));
        }
    }

    _event = xQueueCreate(1, sizeof(uint32_t));
    xTaskCreate(_task, "DIN interrupt task", 2048, NULL, 10, NULL);
}

int DigitalInput::digitalRead(DigitalInputNum_t etor)
{
    if (etor < DIN_MAX) {
        return gpio_get_level(_gpio[etor]);
    } else {
        ESP_LOGE(DIN_TAG, "Invalid DIN_%d", etor+1);
        return -1;
    }
}

void DigitalInput::attachInterrupt(DigitalInputNum_t etor, IsrCallback_t callback, InterruptMode_t mode, void* arg) 
{
    _callback[etor] = callback;
    _arg[etor] = arg;
    gpio_isr_handler_add(_gpio[etor], _isr, (void *)etor);
    gpio_set_intr_type(_gpio[etor], (gpio_int_type_t)mode);
    gpio_intr_enable(_gpio[etor]);
}

void DigitalInput::detachInterrupt(DigitalInputNum_t etor)
{
    gpio_isr_handler_remove(_gpio[etor]);
}

void IRAM_ATTR DigitalInput::_isr(void* arg)
{
    uint32_t etor = (uint32_t)arg;
    xQueueSendFromISR(_event, &etor, NULL);
}

void DigitalInput::_task(void* arg)
{
    DigitalInputNum_t etor;
    while(1) {
        if(xQueueReceive(_event, &etor, portMAX_DELAY)) {
            gpio_intr_disable(_gpio[etor]);
            _callback[etor](_arg[etor]);
            gpio_intr_enable(_gpio[etor]);
        }
    }
}
