/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DigitalInputs.c
 * @brief Functions for din
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "DigitalInputs.h"

static const char DIN_TAG[] = "DigitalInputs";

DigitalInputType_t DigitalInputs::_type;
uint8_t DigitalInputs::_nb;
gpio_num_t* DigitalInputs::_gpio_nums;
ioex_num_t* DigitalInputs::_ioex_nums;
IsrCallback_t* DigitalInputs::_callbacks;
void** DigitalInputs::_args;
ioex_device_t** DigitalInputs::_ioex;
xQueueHandle DigitalInputs::_event;


int DigitalInputs::init(const gpio_num_t *gpio, int nb)
{
    int err = 0;

    _type = DIGITAL_INPUT_GPIO;

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
    xTaskCreate(_task, "DIN interrupt task", 2048, NULL, 10, NULL);

    return err;
}

int DigitalInputs::init(ioex_device_t **ioex, const ioex_num_t *ioex_num, int nb)
{
    int err = 0;

    _type = DIGITAL_INPUT_IOEX;

    /* Save number of DOUT */
    _nb = nb;

    /* Save pointer to ioex */
    _ioex = ioex;
    
    /* Init memory and copy gpio numbers in _gpio_nums table */
    _ioex_nums = (ioex_num_t*) calloc(nb, sizeof(ioex_num_t));
    memcpy(_ioex_nums, ioex_num, nb * sizeof(ioex_num_t));

    _callbacks = (IsrCallback_t*) calloc(nb, sizeof(IsrCallback_t));
    _args = (void**) calloc(nb, sizeof(void*));

    /* Init DIN */
    ESP_LOGI(DIN_TAG, "Init DIN");
    ioex_config_t dinConf = {
        .pin_bit_mask = 0,
        .mode = IOEX_INPUT,
        .pull_mode = IOEX_FLOATING,
        .interrupt_type = IOEX_INTERRUPT_DISABLE,
    };
    for (uint8_t i = 0; i < _nb; i++) {
        dinConf.pin_bit_mask |= (1ULL <<_ioex_nums[i]);
        // /!\ Set level before setting to output
        err |= ioex_set_level(*_ioex, _ioex_nums[i], IOEX_LOW);
    }
    err |= ioex_config(*_ioex, &dinConf);

    _event = xQueueCreate(1, sizeof(uint32_t));
    ESP_LOGI(DIN_TAG, "Create interrupt task");
    xTaskCreate(_task, "DIN interrupt task", 2048, NULL, 10, NULL);

    return err;
}

int DigitalInputs::digitalRead(DigitalInputNum_t num)
{
    if (num < DIN_MAX) {
        if (_type == DIGITAL_INPUT_GPIO) {
            return gpio_get_level(_gpio_nums[num]);
        } else { // DIGITAL_OUTPUT_IOEX
            return ioex_get_level(*_ioex, _ioex_nums[num]);
        }
    } else {
        ESP_LOGE(DIN_TAG, "Invalid DIN_%d", num+1);
        return -1;
    }
}

void DigitalInputs::attachInterrupt(DigitalInputNum_t num, IsrCallback_t callback, InterruptMode_t mode, void* arg)
{
    /* Remove previous callback if it exists */
    if (_callbacks[num] != NULL) {
        detachInterrupt(num);
    }

    _callbacks[num] = callback;
    _args[num] = arg;

    if (_type == DIGITAL_INPUT_GPIO) {
        gpio_isr_handler_add(_gpio_nums[num], _isr, (void *)num);
        gpio_set_intr_type(_gpio_nums[num], (gpio_int_type_t)mode);
        gpio_intr_enable(_gpio_nums[num]);
    } else { // DIGITAL_INPUT_IOEX
        ioex_isr_handler_add(*_ioex, _ioex_nums[num], (ioex_isr_t)callback, (void *)num, 1);
        ioex_set_interrupt_type(*_ioex, _ioex_nums[num], (ioex_interrupt_type_t)(mode));
        ioex_interrupt_enable(*_ioex, _ioex_nums[num]);
    }
}

void DigitalInputs::detachInterrupt(DigitalInputNum_t num)
{
    _callbacks[num] = NULL;
    _args[num] = NULL;
    
    if (_type == DIGITAL_INPUT_GPIO) {
        gpio_intr_disable(_gpio_nums[num]);
        gpio_isr_handler_remove(_gpio_nums[num]);
    } else { // DIGITAL_INPUT_IOEX
        ioex_interrupt_disable(*_ioex, _ioex_nums[num]);
        ioex_isr_handler_remove(*_ioex, _ioex_nums[num]);
    }
}

void IRAM_ATTR DigitalInputs::_isr(void* pvParameters)
{
    uint32_t din = (uint32_t)pvParameters;
    xQueueSendFromISR(_event, &din, NULL);
}

void DigitalInputs::_task(void* pvParameters)
{
    DigitalInputNum_t din;

    while(1) {
        if(xQueueReceive(_event, &din, portMAX_DELAY)) {
            /* Disable interrupt */
            if (DigitalInputs::_type == DIGITAL_INPUT_GPIO)
                gpio_intr_disable(DigitalInputs::_gpio_nums[din]);
            else // DIGITAL_INPUT_IOEX
                ioex_interrupt_disable(*(DigitalInputs::_ioex), DigitalInputs::_ioex_nums[din]);

            /* Call user function */
            DigitalInputs::_callbacks[din](DigitalInputs::_args[din]);
            
            /* Re-enable interrupt */
            if (DigitalInputs::_type == DIGITAL_INPUT_GPIO)
                gpio_intr_enable(DigitalInputs::_gpio_nums[din]);
            else // DIGITAL_INPUT_IOEX
                ioex_interrupt_enable(*(DigitalInputs::_ioex), DigitalInputs::_ioex_nums[din]);

            /* Empty queue to avoid overflow */
            xQueueReset(_event);
        }
    }
}