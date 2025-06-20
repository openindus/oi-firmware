/**
 * @file DigitalInputs.cpp
 * @brief Digital Inputs
 * @author Kevin Lefeuvre (kevinLefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "DigitalInputs.h"

static const char TAG[] = "DigitalInputs";

uint8_t DigitalInputs::_nb = DIN_MAX;
IsrCallback_t* DigitalInputs::_callbacks;
void** DigitalInputs::_args;
QueueHandle_t DigitalInputs::_event;
#if defined(OI_CORE)
ioex_device_t** DigitalInputs::_device;
ioex_num_t* DigitalInputs::_ioex_nums;
#else
gpio_num_t* DigitalInputs::_gpio_nums;
#endif

#if defined(OI_CORE)
int DigitalInputs::init(ioex_device_t **device, const ioex_num_t *num, int nb)
#else
int DigitalInputs::init(const gpio_num_t *gpio, int nb)
#endif
{
    int err = 0;

#if defined(OI_CORE)
    _device = device;
    _ioex_nums = (ioex_num_t*) calloc(nb, sizeof(ioex_num_t));
    memcpy(_ioex_nums, num, nb * sizeof(ioex_num_t));
#else
    _gpio_nums = (gpio_num_t*) calloc(nb, sizeof(gpio_num_t));
    memcpy(_gpio_nums, gpio, nb * sizeof(gpio_num_t));
#endif

    _callbacks = (IsrCallback_t*) calloc(nb, sizeof(IsrCallback_t));
    _args = (void**) calloc(nb, sizeof(void*));

    ESP_LOGI(TAG, "Init Digital Inputs");
    
#if defined(OI_CORE)
    ioex_config_t dinConf = {
        .pin_bit_mask = 0,
        .mode = IOEX_INPUT,
        .pull_mode = IOEX_FLOATING,
        .interrupt_type = IOEX_INTERRUPT_DISABLE,
    };
    for (uint8_t i = 0; i < nb; i++) {
        dinConf.pin_bit_mask |= (1ULL <<_ioex_nums[i]);
        // /!\ Set level before setting to output
        err |= ioex_set_level(*_device, _ioex_nums[i], IOEX_LOW);
    }
    err |= ioex_config(*_device, &dinConf);
#else
    gpio_config_t dinConf = {
        .pin_bit_mask = 0,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    for (uint8_t i = 0; i < nb; i++) {
        dinConf.pin_bit_mask |= (1ULL <<_gpio_nums[i]);
    }
    err |= gpio_config(&dinConf);
#endif

    _event = xQueueCreate(1, sizeof(uint32_t));
    ESP_LOGI(TAG, "Create interrupt task");
    xTaskCreate(_task, "DIN intr task", 4096, NULL, 10, NULL);

    return err;
}

int DigitalInputs::digitalRead(DIn_Num_t num)
{
    int ret = 0;
    if (num < _nb) {
#if defined(OI_CORE)
        ret = ioex_get_level(*_device, _ioex_nums[num]);
#else
        ret = gpio_get_level(_gpio_nums[num]);
#endif
    } else {
        ESP_LOGE(TAG, "Invalid DIN_%d", num+1);
        ret = -1;
    }
    return ret;
}

void DigitalInputs::attachInterrupt(DIn_Num_t num, IsrCallback_t callback, InterruptMode_t mode, void* arg)
{
    if (num < _nb) {
        if (_callbacks[num] != NULL) {
            detachInterrupt(num); // Detach previous interrupt
        }

        _callbacks[num] = callback;
        _args[num] = arg;

#if defined(OI_CORE)
        ioex_isr_handler_add(*_device, _ioex_nums[num], (ioex_isr_t)callback, (void *)num, 1);
        ioex_set_interrupt_type(*_device, _ioex_nums[num], (ioex_interrupt_type_t)(mode));
        ioex_interrupt_enable(*_device, _ioex_nums[num]);
#else
        gpio_isr_handler_add(_gpio_nums[num], _isr, (void *)num);
        gpio_set_intr_type(_gpio_nums[num], (gpio_int_type_t)mode);
        gpio_intr_enable(_gpio_nums[num]);
#endif
    } else {
        ESP_LOGE(TAG, "Invalid DIN_%d", num+1);
    }
}

void DigitalInputs::detachInterrupt(DIn_Num_t num)
{
    if (num < _nb) {
        _callbacks[num] = NULL;
        _args[num] = NULL;
#if defined(OI_CORE)
        ioex_interrupt_disable(*_device, _ioex_nums[num]);
        ioex_isr_handler_remove(*_device, _ioex_nums[num]);
#else
        gpio_intr_disable(_gpio_nums[num]);
        gpio_isr_handler_remove(_gpio_nums[num]);
#endif
    } else {
        ESP_LOGE(TAG, "Invalid DIN_%d", num+1);
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
            // Disable interrupt
#if defined(OI_CORE)
            ioex_interrupt_disable(*_device, _ioex_nums[din]);
#else
            gpio_intr_disable(_gpio_nums[din]);
#endif
            _callbacks[din](_args[din]); // Call user function

            // Re-enable interrupt
#if defined(OI_CORE)
            ioex_interrupt_enable(*_device,_ioex_nums[din]);
#else
            gpio_intr_enable(_gpio_nums[din]);
#endif
            xQueueReset(_event); // Empty queue to avoid overflow
        }
    }
}