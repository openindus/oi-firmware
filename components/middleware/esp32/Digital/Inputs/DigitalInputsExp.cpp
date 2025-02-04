/**
 * @file DigitalInputsExp.h
 * @brief Digital input expander
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "DigitalInputsExp.h"
#include <string.h>

static const char TAG[] = "DigitalInputsExp";

uint8_t DigitalInputsExp::_nb;
ioex_num_t* DigitalInputsExp::_ioex_nums;
IsrCallback_t* DigitalInputsExp::_callbacks;
void** DigitalInputsExp::_args;
ioex_device_t** DigitalInputsExp::_device;
QueueHandle_t DigitalInputsExp::_event;

int DigitalInputsExp::init(ioex_device_t **device, const ioex_num_t *num, int nb)
{
    int err = 0;
    _nb = nb;
    _device = device;
    
    /* Init memory and copy gpio numbers in _gpio_nums table */
    _ioex_nums = (ioex_num_t*) calloc(nb, sizeof(ioex_num_t));
    memcpy(_ioex_nums, num, nb * sizeof(ioex_num_t));

    _callbacks = (IsrCallback_t*) calloc(nb, sizeof(IsrCallback_t));
    _args = (void**) calloc(nb, sizeof(void*));

    /* Init DIN */
    ESP_LOGI(TAG, "Init DIN");
    ioex_config_t dinConf = {
        .pin_bit_mask = 0,
        .mode = IOEX_INPUT,
        .pull_mode = IOEX_FLOATING,
        .interrupt_type = IOEX_INTERRUPT_DISABLE,
    };
    for (uint8_t i = 0; i < _nb; i++) {
        dinConf.pin_bit_mask |= (1ULL <<_ioex_nums[i]);
        // /!\ Set level before setting to output
        err |= ioex_set_level(*_device, _ioex_nums[i], IOEX_LOW);
    }
    err |= ioex_config(*_device, &dinConf);

    _event = xQueueCreate(1, sizeof(uint32_t));
    ESP_LOGI(TAG, "Create interrupt task");
    xTaskCreate(_task, "DIN intr task", 4096, NULL, 10, NULL);

    return err;
}

int DigitalInputsExp::digitalRead(DIn_Num_t num)
{
    if (num < _nb) {
        return ioex_get_level(*_device, _ioex_nums[num]);
    } else {
        ESP_LOGE(TAG, "Invalid DIN_%d", num+1);
        return -1;
    }
}

void DigitalInputsExp::attachInterrupt(DIn_Num_t num, IsrCallback_t callback, InterruptMode_t mode, void* arg)
{
    if (num < _nb) {
        /* Remove previous callback if it exists */
        if (_callbacks[num] != NULL) {
            detachInterrupt(num);
        }
        _callbacks[num] = callback;
        _args[num] = arg;
        ioex_isr_handler_add(*_device, _ioex_nums[num], (ioex_isr_t)callback, (void *)num, 1);
        ioex_set_interrupt_type(*_device, _ioex_nums[num], (ioex_interrupt_type_t)(mode));
        ioex_interrupt_enable(*_device, _ioex_nums[num]);
    } else {
        ESP_LOGE(TAG, "Invalid DIN_%d", num+1);
    }
}

void DigitalInputsExp::detachInterrupt(DIn_Num_t num)
{
    if (num < _nb) {
        _callbacks[num] = NULL;
        _args[num] = NULL;
        ioex_interrupt_disable(*_device, _ioex_nums[num]);
        ioex_isr_handler_remove(*_device, _ioex_nums[num]);
    } else {
        ESP_LOGE(TAG, "Invalid DIN_%d", num+1);
    }
}

void IRAM_ATTR DigitalInputsExp::_isr(void* pvParameters)
{
    uint32_t din = (uint32_t)pvParameters;
    xQueueSendFromISR(_event, &din, NULL);
}

void DigitalInputsExp::_task(void* pvParameters)
{
    DIn_Num_t din;

    while(1) {
        if(xQueueReceive(_event, &din, portMAX_DELAY)) {
            ioex_interrupt_disable(*_device, _ioex_nums[din]); // Disable interrupt
            _callbacks[din](_args[din]); // Call user function
            ioex_interrupt_enable(*_device,_ioex_nums[din]); // Re-enable interrupt
            xQueueReset(_event); // Empty queue to avoid overflow
        }
    }
}