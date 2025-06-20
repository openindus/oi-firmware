/**
 * @file DigitalOutputs.h
 * @brief Digital outputs
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Common.h"
#include "DigitalOutputsInterface.h"

#if defined(OI_CORE)
#include "pcal6524.h"
#else
#include "adc.h"
#include "esp_adc_cal.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#endif

class DigitalOutputs : public DigitalOutputsInterface
{
public:
    void digitalWrite(DOut_Num_t num, bool level) override;
    void toggleOutput(DOut_Num_t num) override;
    void outputMode(DOut_Num_t num, DOut_Mode_t mode) override;
    void setPWMFrequency(DOut_Num_t num, uint32_t freq) override;
    void setPWMDutyCycle(DOut_Num_t num, float duty) override;
    float getOutputCurrent(DOut_Num_t num) override;
    int outputIsOvercurrent(DOut_Num_t num) override;

    void setOvercurrentThreshold(float threshold, float thresholdSum = 8.0f) override {
        _overcurrentThreshold = threshold;
        _overcurrentThresholdSum = thresholdSum;
    }

    void attachOvercurrentCallback(void (*callback)(void*), void *arg = NULL) override {
        _overcurrentCallback = callback;
    }

    void detachOvercurrentCallback(void) override {
        _overcurrentCallback = NULL;
    }
    
protected:
#if defined(OI_CORE)
    static int init(ioex_device_t **ioex, const ioex_num_t *ioex_num, const ioex_num_t *ioex_current, int num);
#else
    static int init(const gpio_num_t *gpio, const adc_num_t *adc, int num);
#endif

private:
    static uint8_t _nb; // Number of DOUT
    static DOut_Mode_t *_mode; // Mode of each output (digital or PWM)
    static bool *_level; // Level of each DOUT (HIGH or LOW)

#if defined(OI_CORE)
    static ioex_num_t *_ioex_num;
    static ioex_num_t *_ioex_current;
    static ioex_device_t **_ioex;
#else
    static gpio_num_t *_gpio_num; // GPIO number for each DOUT
    static adc_num_t *_adc_current; // ADC channel for each DOUT current measurement
    static esp_adc_cal_characteristics_t _adc1Characteristics;
    static esp_adc_cal_characteristics_t _adc2Characteristics;
    static float _adcReadCurrent(DOut_Num_t num);
#endif

    /* Overcurrent threshold */
    static float _overcurrentThreshold;
    static float _overcurrentThresholdSum;
    static void (*_overcurrentCallback)(void*);

    static SemaphoreHandle_t _mutex;
    static void _controlTask(void *pvParameters);
};