/**
 * @file AnalogInputsHV.h
 * @brief Analog Input
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include <string.h>
#include "esp_adc_cal.h"
#include "esp_efuse.h"

#include "AnalogInputs.h"

#define ESP_ADC_NO_OF_SAMPLES        128U
#define ESP_ADC_DEFAULT_COEFF_A      11.6965f
#define ESP_ADC_DEFAULT_COEFF_B      0.0f

typedef struct {
    float ain_coeff_a;
    float ain_coeff_b;
} AnalogInput_eFuse_Coeff_t;

class AnalogInputEsp32s3
{
public:

    AnalogInputEsp32s3(uint8_t num, AdcNumChannel_t channel);
    void init(void);
    int read(void);
    float read(AnalogInput_Unit_t unit);
    void getCoeffs();
    float applyCoeffs(float voltage);

private:

    int _num;
    AdcNumChannel_t _channel;
    esp_adc_cal_characteristics_t _adc_characteristic;
    AnalogInput_eFuse_Coeff_t _coeff;
};

class AnalogInputsHV
{
public:

    static int init(const AdcNumChannel_t* channel, uint8_t nb);

    /**
     * @brief Read the value of AIN.
     * The function return an integer that correspond to the internal voltage of the Analog Input in mV.
     * This value is not calibrated and not converted to the real Analog Input voltage.
     * It is strongly recommended to use analogReadVolts or analogReadMillivolts instead.
     * 
     * @param ain ANA input number.
     * @return Value of the AIN input.
     */
    static int analogRead(AnalogInput_Num_t num);
    
    /**
     * @brief Read the value of AIN.
     * The function return a float that correspond to the voltage of the ANA (from 0 to 30V).
     *
     * @param ain ANA input number.
     * @return Value of the AIN input.
     */
    static float analogReadVolt(AnalogInput_Num_t num);
    
    /**
     * @brief Read the value of AIN.
     * The function return a float that correspond to the voltage of the ANA (from 0 to 30000mV).
     *
     * @param ain ANA input number.
     * @return Value of the AIN input.
     */
    static float analogReadMilliVolt(AnalogInput_Num_t num);
    
    static int setAnalogCoeffs(float* a, float* b);

private:

    static uint8_t _nb;
    static AnalogInputEsp32s3** _ains;

    static float read(AnalogInput_Num_t num, AnalogInput_Unit_t unit);
};