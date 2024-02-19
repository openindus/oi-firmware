/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DigitalInput.c
 * @brief Functions for din
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "DigitalInput.h"

static const char DIN_TAG[] = "DigitalInput";

xQueueHandle DigitalInput::_event;

DigitalInput::DigitalInput(const gpio_num_t *gpio, int num) 
{
    _type = DIGITAL_INPUT_GPIO;

    /* Save number of DOUT */
    _num = num;
    
    /* Init memory and copy gpio numbers in _gpio_num table */
    _gpio_num = (gpio_num_t*) calloc(num, sizeof(gpio_num_t));
    memcpy(_gpio_num, gpio, num * sizeof(gpio_num_t));

    _callback = (IsrCallback_t*) calloc(num, sizeof(IsrCallback_t));
    _arg = (void**) calloc(num, sizeof(void*));
}

DigitalInput::DigitalInput(ioex_device_t *ioex, const ioex_num_t *ioex_num, int num) 
{
    _type = DIGITAL_INPUT_IOEX;

    /* Save number of DOUT */
    _num = num;

    /* Save pointer to ioex */
    _ioex = ioex;
    
    /* Init memory and copy gpio numbers in _gpio_num table */
    _ioex_num = (ioex_num_t*) calloc(num, sizeof(ioex_num_t));
    memcpy(_ioex_num, ioex_num, num * sizeof(ioex_num_t));

    _callback = (IsrCallback_t*) calloc(num, sizeof(IsrCallback_t));
    _arg = (void**) calloc(num, sizeof(void*));
}

DigitalInput::~DigitalInput()
{
    if (_type == DIGITAL_INPUT_GPIO) {  
        free(_gpio_num);
    } else { // DIGITAL_INTPUT_IOEX
        free(_ioex_num);
    }
    free(_callback);
    free(_arg);
}

void DigitalInput::init(void)
{
    if (_type == DIGITAL_INPUT_GPIO) 
    {
        /* Init DIN Gpio */
        ESP_LOGI(DIN_TAG, "Init DIN");
        gpio_config_t dinConf = {
            .pin_bit_mask = 0,
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
        };
        for (uint8_t i = 0; i < _num; i++) {
            dinConf.pin_bit_mask |= (1ULL <<_gpio_num[i]);
        }
        ESP_ERROR_CHECK(gpio_config(&dinConf));
    }
    else // DIGITAL_INPUT_IOEX
    {
        /* Init DIN */
        ESP_LOGI(DIN_TAG, "Init DOUT");
        ioex_config_t dinConf = {
            .pin_bit_mask = 0,
            .mode = IOEX_INPUT,
            .pull_mode = IOEX_FLOATING,
            .interrupt_type = IOEX_INTERRUPT_DISABLE,
        };
        for (uint8_t i = 0; i < _num; i++) {
            dinConf.pin_bit_mask |= (1ULL <<_ioex_num[i]);
            // /!\ Set level before setting to output
            ESP_ERROR_CHECK(ioex_set_level(_ioex, _ioex_num[i], IOEX_LOW));
        }
        ESP_ERROR_CHECK(ioex_config(_ioex, &dinConf));
    }

    _event = xQueueCreate(1, sizeof(uint32_t));
    ESP_LOGI(DIN_TAG, "Create control task");
    xTaskCreate(_task, "DIN interrupt task", 2048, NULL, 10, NULL);
}

int DigitalInput::digitalRead(DigitalInputNum_t din)
{
    if (din < DIN_MAX) {
        return _common_get_level(din);
    } else {
        ESP_LOGE(DIN_TAG, "Invalid DIN_%d", din+1);
        return -1;
    }
}

void DigitalInput::attachInterrupt(DigitalInputNum_t din, IsrCallback_t callback, InterruptMode_t mode, void* arg)
{
    _callback[din] = callback;
    _arg[din] = arg;

    if (_type == DIGITAL_INPUT_GPIO)
    {
        gpio_isr_handler_add(_gpio_num[din], _isr, (void *)din);
        gpio_set_intr_type(_gpio_num[din], (gpio_int_type_t)mode);
        gpio_intr_enable(_gpio_num[din]);
    }
    else // DIGITAL_INPUT_IOEX
    {
        ioex_isr_handler_add(_ioex, _ioex_num[din], (ioex_isr_t)callback, (void *)din, 1);
        ioex_set_interrupt_type(_ioex, _ioex_num[din], (ioex_interrupt_type_t)(mode));
        ioex_interrupt_enable(_ioex, _ioex_num[din]);
    }
}

void DigitalInput::detachInterrupt(DigitalInputNum_t din)
{
    if (_type == DIGITAL_INPUT_GPIO)
    {
        gpio_isr_handler_remove(_gpio_num[din]);
    }
    else // DIGITAL_INPUT_IOEX
    {
        ioex_isr_handler_remove(_ioex, _ioex_num[din]);
    }
}

void IRAM_ATTR DigitalInput::_isr(void* pvParameters)
{
    uint32_t din = (uint32_t)pvParameters;
    xQueueSendFromISR(_event, &din, NULL);
}

void DigitalInput::_task(void* pvParameters)
{
    DigitalInputNum_t din;
    DigitalInput* dout = (DigitalInput*) pvParameters;

    while(1) 
    {
        if(xQueueReceive(dout->_event, &din, portMAX_DELAY))
        {
            /* Disable interrupt */
            if (dout->_type == DIGITAL_INPUT_GPIO)
                gpio_intr_disable(dout->_gpio_num[din]);
            else // DIGITAL_INPUT_IOEX
                ioex_interrupt_disable(dout->_ioex, dout->_ioex_num[din]);

            /* Call user function */
            dout->_callback[din](dout->_arg[din]);

            /* Re-enable interrupt */
            if (dout->_type == DIGITAL_INPUT_GPIO)
                gpio_intr_enable(dout->_gpio_num[din]);
            else // DIGITAL_INPUT_IOEX
                ioex_interrupt_enable(dout->_ioex, dout->_ioex_num[din]);
        }
    }
}

int DigitalInput::_common_get_level(DigitalInputNum_t din)
{
    if (_type == DIGITAL_INPUT_GPIO) {
        return gpio_get_level(_gpio_num[din]);
    } else { // DIGITAL_OUTPUT_IOEX
        return ioex_get_level(_ioex, _ioex_num[din]);
    }
}