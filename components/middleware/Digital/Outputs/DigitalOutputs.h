/**
 * @file DigitalOutputs.h
 * @brief Digital outputs
 * @author
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "DigitalOutputsTypes.h"
#include "esp_adc_cal.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "adc.h"

class DigitalOutputs
{
protected:
    static int init(const gpio_num_t *gpio, const adc_num_t *adc, int num);

public:
    /**
     * @brief Set an output at high or low level.
     *
     * @param num DOUT to drive.
     * @param level DOUT level, HIGH or LOW.
     */
    static void digitalWrite(DOut_Num_t num, bool level);

    template <typename T1, typename T2> 
    static inline void digitalWrite(T1 num, T2 level) {
        digitalWrite((DOut_Num_t)num, (bool)level);
    }

    /**
     * @brief Toggle a digital output
     *
     * @param num DOUT to toggle
     **/
    static void toggleOutput(DOut_Num_t num);

    /**
     * @brief Configure output mode
     *
     * @param num ouput number
     * @param mode ouput mode [DIGITAL, PWM]
     */
    static void outputMode(DOut_Num_t num, DOut_Mode_t mode);

    /**
     * @brief Set the frequency value of PWM for a digital output
     *
     * @param num DOUT to set
     * @param freq PWM frequency [50 - 1000 Hz]
     **/
    static void setPWMFrequency(DOut_Num_t num, uint32_t freq);

    /**
     * @brief Set the duty cycle value of PWM for a digital output
     *
     * @param num DOUT to set
     * @param duty Duty cycle in percentage [0-100] 
     **/
    static void setPWMDutyCycle(DOut_Num_t num, float duty);

    /**
     * @brief Get the current of a digital output
     *
     * @param num DOUT to get
     * @return current in Ampere
     **/
    static float getOutputCurrent(DOut_Num_t num);

    /**
     * @brief Get the overcurrent status of a digital output
     *
     * @param num DOUT to get
     * @return 1 if overcurrent, 0 if not
     **/
    static int outputIsOvercurrent(DOut_Num_t num);

    /**
     * @brief Set the Overcurrent Threshold
     * 
     * @param threshold 
     * @param totalThreshold 
     */
    static void setOvercurrentThreshold(float threshold, float thresholdSum = 8.0f) {
        _overcurrentThreshold = threshold;
        _overcurrentThresholdSum = thresholdSum;
    }

    /**
     * @brief Attach a callback function to be called when an overcurrent is detected
     * 
     * @param callback Function pointer to the callback function
     */
    static void attachOvercurrentCallback(void (*callback)(void)) {
        _overcurrentCallback = callback;
    }

    /**
     * @brief Detach the overcurrent callback function
     */
    static void detachOvercurrentCallback(void) {
        _overcurrentCallback = NULL;
    }

private:
    /* Mode of each output (digital or PWM) */
    static DOut_Mode_t *_mode;

    /* Number of DOUT */
    static uint8_t _nb;

    /* GPIO num for DOUT */
    static gpio_num_t *_gpio_num;

    /* ADC Channel for ouput current */
    static adc_num_t *_adc_current;
    static esp_adc_cal_characteristics_t _adc1Characteristics;
    static esp_adc_cal_characteristics_t _adc2Characteristics;

    static bool *_level;
    static SemaphoreHandle_t _mutex;

    /* Overcurrent threshold */
    static float _overcurrentThreshold;
    static float _overcurrentThresholdSum;
    static void (*_overcurrentCallback)(void);

    static void _controlTask(void *pvParameters);

    static int _registerCLI(void);
};