/**
 * @file global_sensor.cpp
 * @brief Global sensor
 * @author Mani Gillier (mani.gillier@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "global_sensor.hpp"

static const char TAG[] = "Sensor";

void Sensor::route(void)
{
    AIn_Num_t hs_ain = _ainPins[_mux_config.hs_index];
    AIn_Num_t ls_ain = _ainPins[_mux_config.ls_index];

    /* Positive AIn is congruent with 0 modulo 2 */
    if (hs_ain % 2 != 0) {
        ESP_LOGE(TAG, "The sensor AIn (%d) set at index %d is not a positive AIn. No MUX ROUTE may occur.\n", hs_ain, _mux_config.hs_index);
        return;
    }
    /* Positive AIn is congruent with 1 modulo 2 */
    if (ls_ain % 2 != 1) {
        ESP_LOGE(TAG, "The sensor AIn (%d) set at index %d is not a negative AIn. No MUX ROUTE may occur.\n", ls_ain, _mux_config.ls_index);
        return;
    }
    /* Divided by 2 because only half of the AIns are in each MUX */
    _highSideMux->route(_mux_config.input, hs_ain / 2);
    _lowSideMux->route(ls_ain / 2, _mux_config.output);
}

/**
 * @brief Initialise the read parameters and connections
 */
void Sensor::init_read(void)
{
    /* Route multiplexer */
    route();

    /* Set bias on negative input if activated */
    if (_bias_active) {
        _adc->setBias(static_cast<ads114s0x_adc_input_e>(_adcPins[1]));
    } else {
        _adc->setBias(ADS114S0X_NOT_CONNECTED);
    }

    /* Set Internal reference to selected ref */
    _adc->setReference((ads114s0x_ref_selection_e)_reference);

    /* Set Excitation mode to selected mode */
    _adc->setExcitation((ads114s0x_idac_magnitude_e)_excitation);

    /* Set PGA Gain */
    _adc->setPGAGain((ads114s0x_pga_gain_e)_gain);

    /* Wait for stabilization */
    _adc->waitStabilization();
}

void Sensor::reset_read(void)
{
    /* Reset multiplexer route */
    _highSideMux->route(INPUT_OPEN_HS, 0);
    _lowSideMux->route(0, OUTPUT_OPEN_LS);

    /* Reset bias */
    _adc->setBias(ADS114S0X_NOT_CONNECTED);

    /* Reset ref */
    _adc->setReference(ADS114S0X_REF_INTERNAL_2_5V);

    /* Reset excitation */
    _adc->setExcitation(ADS114S0X_IDAC_OFF);

    /* Reset gain */
    _adc->setPGAGain(ADS114S0X_PGA_GAIN_1);

    /* Reset Internal MUX */
    _adc->setInternalMux(ADS114S0X_NOT_CONNECTED, ADS114S0X_NOT_CONNECTED);
}

/**
 * @brief Read value default function
 *
 * @return adcCode converted to float
 */
// - [X] TASK make read commands accept a defaulted to false parameter that describes if the return value should be printed
float Sensor::read(bool print_result)
{
    int16_t adc_code = raw_read(0, 1, print_result); // read the raw value and print it if asked.

    return (float) adc_code;
}

/**
 * @brief Read raw value
 * 
 * @return int16_t adcCode
 */
// - [X] TASK make read commands accept a defaulted to false parameter that describes if the return value should be printed
int16_t Sensor::raw_read(uint8_t positive_index, uint8_t negative_index, bool print_result)
{
    if (_adc == NULL) {
        ESP_LOGE(TAG, "%s() error", __func__);
        return 0;
    }

    init_read();

    /* Set Internal mux */
    _adc->setInternalMux(static_cast<ads114s0x_adc_input_e>(_adcPins[positive_index]), static_cast<ads114s0x_adc_input_e>(_adcPins[negative_index]));

    /* ADC Read */
    int16_t adcCode = _adc->read();

    reset_read();

    if (print_result) {
        // adcCode is a number with maximum at 32767 (MAX_SHORT_INT)
        print_int16(adcCode, "/ 32767 (raw value)");
    }
    return adcCode;
}

/**
 * @brief prints an int16 (for example adcCode) with suffixed unit_str
 */
void Sensor::print_int16(const int16_t value, const char *unit_str)
{
    printf("[%d] - %hd %s\n", _index, value, unit_str);
}

/**
 * @brief prints a float (for example temperature value) with suffixed unit_str
 */
void Sensor::print_float(const float value, const char *unit_str)
{
    printf("[%d] - %.3f %s\n", _index, value, unit_str);
}
