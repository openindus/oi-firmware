/**
 * @file DigitalInputs.h
 * @brief Digital Inputs
 * @author Kevin Lefeuvre (kevinLefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Common.h"
#include "DigitalInputsInterface.h"

#if defined(CONFIG_OI_CORE)
#include "pcal6524.h"
#else
#include "driver/gpio.h"
#endif

class DigitalInputs : public DigitalInputsInterface
{
public:
    DigitalInputs() {}
    ~DigitalInputs() {}

    int digitalRead(DIn_Num_t num) override;
    void attachInterrupt(DIn_Num_t num, IsrCallback_t callback, InterruptMode_t mode, void *arg = NULL) override;
    void detachInterrupt(DIn_Num_t num) override;

protected:
#if defined(CONFIG_OI_CORE)
    static int init(ioex_device_t **device, const ioex_num_t *num, int nb);
#else
    static int init(const gpio_num_t *gpio, int nb);
#endif

private:
    static uint8_t _nb; // Number of digital inputs
    static IsrCallback_t *_callbacks;
    static void **_args;
    static QueueHandle_t _event;
    static void IRAM_ATTR _isr(void *pvParameters);
    static void _task(void *pvParameters);

#if defined(CONFIG_OI_CORE)
    static ioex_device_t** _device; // Pointer to IOEX device
    static ioex_num_t *_ioex_nums; // IOEX numbers for digital inputs
#else
    static gpio_num_t *_gpios; // GPIO numbers for digital inputs
#endif
};
