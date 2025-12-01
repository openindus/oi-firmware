/**
 * @file AnalogInputsHV.h
 * @brief Analog Inputs High Voltage
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include <string.h>
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_efuse.h"
#include "AnalogInputs.h"

#define ESP_ADC_NO_OF_SAMPLES 10U
#define ESP_ADC_DEFAULT_COEFF_A 11.6965f
#define ESP_ADC_DEFAULT_COEFF_B 0.0f

class AnalogInputsHV
{
public:
    static int init(const adc_unit_t adc_unit,
        const adc_channel_t* adc_channels,
        uint8_t nb_channels,
        adc_oneshot_unit_handle_t adc_handle);

    /**
     * @brief Read the value of AIN.
     * The function return an integer that correspond to the internal voltage of the Analog Input in
     * mV. This value is not calibrated and not converted to the real Analog Input voltage. It is
     * strongly recommended to use analogReadVolts or analogReadMillivolts instead.
     *
     * @param num ANA input number.
     * @return Value of the AIN input.
     */
    static float analogRead(AnalogInput_Num_t num);

    /**
     * @brief Read the value of AIN.
     * The function return a float that correspond to the voltage of the ANA (from 0 to 30V).
     *
     * @param num ANA input number.
     * @return Value of the AIN input.
     */
    static float analogReadVolt(AnalogInput_Num_t num);

    /**
     * @brief Read the value of AIN.
     * The function return a float that correspond to the voltage of the ANA (from 0 to 30000mV).
     *
     * @param num ANA input number.
     * @return Value of the AIN input.
     */
    static float analogReadMilliVolt(AnalogInput_Num_t num);

    /**
     * @brief Set the Analog Coeffs object
     *
     * @param a slope coefficient
     * @param b y-intercept
     * @return 0 if success, -1 if error
     **/
    static int setAnalogCoeffs(float *a, float *b);

    /**
     * @brief Get the Analog Coeffs object
     *
     * @param as slope coefficient of each Analog Inputs
     * @param bs y-intercept of each Analog Inputs
     **/
    static void getAnalogCoeffs(float *as, float *bs);

protected:
    static float read(AnalogInput_Num_t num, AnalogInput_Unit_t unit);

private:
    static adc_unit_t _adc_unit;
    static adc_channel_t *_adc_channels;
    static uint8_t _nb_channels;
    static adc_oneshot_unit_handle_t *_adc_handles;
    static adc_cali_handle_t *_adc_cali_handles;
    static float *_coeff_a;
    static float *_coeff_b;

    static int _registerCLI(void);
};