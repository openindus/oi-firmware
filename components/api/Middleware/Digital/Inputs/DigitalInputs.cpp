/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DigitalInputs.cpp
 * @brief Functions for din
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "DigitalInputs.h"

static const char DIN_TAG[] = "DigitalInputs";

uint8_t DigitalInputs::_nb;
gpio_num_t* DigitalInputs::_gpio_nums;
IsrCallback_t* DigitalInputs::_callbacks;
void** DigitalInputs::_args;
xQueueHandle DigitalInputs::_event;


int DigitalInputs::init(const gpio_num_t *gpio, int nb)
{
    int err = 0;

    /* Save number of DIN */
    _nb = nb;
    
    /* Init memory and copy gpio numbers in _gpio_nums table */
    _gpio_nums = (gpio_num_t*) calloc(nb, sizeof(gpio_num_t));
    memcpy(_gpio_nums, gpio, nb * sizeof(gpio_num_t));

    _callbacks = (IsrCallback_t*) calloc(nb, sizeof(IsrCallback_t));
    _args = (void**) calloc(nb, sizeof(void*));

    /* Init DIN Gpio */
    ESP_LOGI(DIN_TAG, "Init DIN");
    gpio_config_t dinConf = {
        .pin_bit_mask = 0,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    for (uint8_t i = 0; i < _nb; i++) {
        dinConf.pin_bit_mask |= (1ULL <<_gpio_nums[i]);
    }
    err |= gpio_config(&dinConf);

    _event = xQueueCreate(1, sizeof(uint32_t));
    ESP_LOGI(DIN_TAG, "Create interrupt task");
    xTaskCreate(_task, "DIN intr task", 4096, NULL, 10, NULL);

    return err;
}

int DigitalInputs::digitalRead(DIn_Num_t num)
{
    if (num < _nb) {
        return gpio_get_level(_gpio_nums[num]);
    } else {
        ESP_LOGE(DIN_TAG, "Invalid DIN_%d", num+1);
        return -1;
    }
}

void DigitalInputs::attachInterrupt(DIn_Num_t num, IsrCallback_t callback, InterruptMode_t mode, void* arg)
{
    if (num < _nb) {
        /* Remove previous callback if it exists */
        if (_callbacks[num] != NULL) {
            detachInterrupt(num);
        }

        _callbacks[num] = callback;
        _args[num] = arg;

        gpio_isr_handler_add(_gpio_nums[num], _isr, (void *)num);
        gpio_set_intr_type(_gpio_nums[num], (gpio_int_type_t)mode);
        gpio_intr_enable(_gpio_nums[num]);
    } else {
        ESP_LOGE(DIN_TAG, "Invalid DIN_%d", num+1);
    }
}

void DigitalInputs::detachInterrupt(DIn_Num_t num)
{
    if (num < _nb) {
        _callbacks[num] = NULL;
        _args[num] = NULL;

        gpio_intr_disable(_gpio_nums[num]);
        gpio_isr_handler_remove(_gpio_nums[num]);
    } else {
        ESP_LOGE(DIN_TAG, "Invalid DIN_%d", num+1);
    }
}

void IRAM_ATTR DigitalInputs::_isr(void* pvParameters)
{
    uint32_t din = (uint32_t)pvParameters;
    xQueueSendFromISR(_event, &din, NULL);
}

void DigitalInputs::_task(void* pvParameters)
{
    DIn_Num_t din;

    while(1) {
        if(xQueueReceive(_event, &din, portMAX_DELAY)) {
            /* Disable interrupt */
            gpio_intr_disable(DigitalInputs::_gpio_nums[din]);

            /* Call user function */
            DigitalInputs::_callbacks[din](DigitalInputs::_args[din]);
            
            /* Re-enable interrupt */
            gpio_intr_enable(DigitalInputs::_gpio_nums[din]);

            /* Empty queue to avoid overflow */
            xQueueReset(_event);
        }
    }
}