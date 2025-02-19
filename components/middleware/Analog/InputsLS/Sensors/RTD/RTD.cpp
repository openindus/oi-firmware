/**
 * @file RTD.cpp
 * @brief RTD (PT100 / PT1000)
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "RTD.h"
#include <math.h>

static const char TAG[] = "RTD";

// Value of temperature for pt100 by decade
// First value is the value for 10ohms and last one is the value for 400Ohms
// A zero is added at the beginning to match index and resistor value
// Value are taken from online fluke pt100 calculator
static const float pt100_table[] = {-242.021, -219.539, -196.572, -173.158, -149.335, -125.146, -100.631, -75.828, -50.771, -25.488,
                                    0.000, 25.684, 51.566, 77.651, 103.943, 130.447, 157.169, 184.115, 211.289, 238.698, 
                                    266.348, 294.246, 322.397, 350.810, 379.492, 408.450, 437.693, 467.229, 497.067, 527.217, 
                                    557.688, 588.491, 619.638, 651.140, 683.009, 715.259, 747.903, 780.957, 814.436, 848.357, 
                                    882.737};
    

float RTD::_calculateRTD(int16_t adcCode)
{
    /* Calculate RTD resistor values */
    if (_type == RTD_PT100)
        return (float)(2 * RTD_R_REF * adcCode) / (float)(RTD_PT100_GAIN * ADS114S0X_MAX_ADC_CODE);
    else if (_type == RTD_PT1000) {
        return (float)(2 * RTD_R_REF * adcCode) / (float)(RTD_PT1000_GAIN * ADS114S0X_MAX_ADC_CODE);
    }
    return 0.0f;
}

/**
 * @brief Read RTD resistor (ohm)
 * 
 * @return float R_RTD value
 */
float RTD::readResistor(bool print_result)
{
    float rRTD = 0.0;
    float rRTD0 = 0.0;
    float rRTD1 = 0.0;

    /* MEASURE WITH TWO INPUTS */

    _mux_config.hs_index = 0;
    /* set the output pin to index 1 if 2 wire config and index 2 if 3 wire config */
    _mux_config.ls_index = _ainPins[2] == -1 ? 1 : 2;
    int16_t adcCode = raw_read(0, 1);
    rRTD0 = _calculateRTD(adcCode);

    /* RTD 2 Wires */
    if (_ainPins[2] == -1) {
        rRTD = rRTD0;
        if (print_result) {
            // result is a resistance
            print_float(rRTD, "Ω");
        }
        return rRTD;
    }

    /* MEASURE WITH THE OTHER INPUT */

    /* Set internal mux */
    adcCode = raw_read(1, 2);
    rRTD1 = _calculateRTD(adcCode);

    /* Subtract cable resistance to RTD resistance */
    rRTD = std::abs(rRTD0 - rRTD1);
    if (print_result) {
        // result is a resistance
        print_float(rRTD, "Ω");
    }
    return rRTD;
}

/**
 * @brief Read temperature (°C)
 * 
 * @return float Temperature value
 */
float RTD::readTemperature(bool print_result)
{
    // Read resistor value
    float rRtd = readResistor();
    float temperature = NAN;

    if ((_type == RTD_PT100 && rRtd >= 10 && rRtd <= 390) || (_type == RTD_PT1000 && rRtd >= 100 && rRtd <= 2900))
    {
        // Second order interpolation
        if (_type == RTD_PT100) rRtd = rRtd / 10.0;
        if (_type == RTD_PT1000) rRtd = rRtd / 100.0;
        int integer = floor(rRtd);
        float decimal = rRtd - integer;

        float a = pt100_table[integer];
        float b = pt100_table[integer + 1] / 2;
        float c = pt100_table[integer - 1] / 2;
        temperature = a + decimal * (b - c + decimal * (c + b - a));
    }

    if (print_result) {
        // result is a temperature
        print_float(temperature, "°c");
    }
    return temperature;
}