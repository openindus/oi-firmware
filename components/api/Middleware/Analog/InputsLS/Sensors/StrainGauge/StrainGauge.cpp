/**
 * @file StrainGauge.cpp
 * @brief Strain gauge (current or voltage excitation)
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "StrainGauge.h"

#define SG_GAIN                             128.0f
#define SG_GAIN_REGISTER                    ADS114S0X_PGA_GAIN_128
#define SG_ACQUISITION_REFERENCE_VOLTAGE    ADS114S0X_REF_REFP0_REFNO
#define SG_ACQUISITION_REFERENCE_CURRENT    ADS114S0X_REF_REFP1_REFN1

static const char TAG[] = "StrainGauge";

float StrainGauge::read(void)
{

    int16_t adcCode = 0;

    if ((_highSideMux == NULL) || (_lowSideMux == NULL) || (_adc == NULL)) {
        ESP_LOGE(TAG, "%s() error", __func__);
        return 0.0f;
    }

    switch (_excitation)
    {
    case EXCITATION_VOLTAGE_3V3:
    case EXCITATION_VOLTAGE_5V:
    case EXCITATION_VOLTAGE_USER:
    {
        /* Select input voltage */
        Mux_IO_t voltage_input = INPUT_SENSOR_3V3;
        if (_excitation == EXCITATION_VOLTAGE_3V3)  voltage_input = INPUT_SENSOR_3V3;
        else if (_excitation == EXCITATION_VOLTAGE_5V) voltage_input = INPUT_SENSOR_5V;
        else if (_excitation == EXCITATION_VOLTAGE_USER) voltage_input = INPUT_VISUP_DIV;

        /* MUX Configuration:
        * Input excitation is voltage
        * Output should go to ground through the bias resistor for voltage offset */
        _highSideMux->route(voltage_input, _pins.hs);
        _lowSideMux->route(_pins.ls, OUTPUT_RBIAS_GAUGE);
        
        /* Set ADC Acquisition reference */
        _adc->setReference(SG_ACQUISITION_REFERENCE_VOLTAGE);
    
        /* Set PGA Gain */
        _adc->setPGAGain(SG_GAIN_REGISTER);

        /* Reset Internal MUX */
        _adc->setInternalMux(static_cast<ads114s0x_adc_input_e>(_pins.sP), static_cast<ads114s0x_adc_input_e>(_pins.sN));

        /* Wait for stabilization if needed */
        _adc->waitStabilization();
        
        /* Read adc value */
        adcCode = _adc->read();

        /* Reset Internal MUX */
        _adc->setInternalMux(ADS114S0X_NOT_CONNECTED, ADS114S0X_NOT_CONNECTED);

        /* MUX Configuration:
        * Disconnect input
        * Disconnect output */
        _highSideMux->route(INPUT_OPEN_HS, _pins.hs);
        _lowSideMux->route(_pins.ls, OUTPUT_OPEN_LS);

        break;
    }

    case EXCITATION_CURRENT_10UA:
    case EXCITATION_CURRENT_100UA:
    case EXCITATION_CURRENT_500UA:
    case EXCITATION_CURRENT_1000UA:
    case EXCITATION_CURRENT_2000UA:
    {
         /* MUX Configuration:
        * Input excitation is IDAC1
        * Output should go to ground through the bias resistor for voltage offset */
        _highSideMux->route(INPUT_IDAC1, _pins.hs);
        _lowSideMux->route(_pins.ls, OUTPUT_RBIAS_GAUGE);
        
        /* Set ADC Acquisition reference */
        _adc->setReference(SG_ACQUISITION_REFERENCE_CURRENT);
    
        /* Set PGA Gain */
        _adc->setPGAGain(SG_GAIN_REGISTER);

        /* Set excitation */
        ads114s0x_idac_magnitude_e excitation_current = ADS114S0X_IDAC_10_UA;
        if (_excitation == EXCITATION_CURRENT_10UA) excitation_current = ADS114S0X_IDAC_10_UA;
        else if (_excitation == EXCITATION_CURRENT_100UA) excitation_current = ADS114S0X_IDAC_100_UA;
        else if (_excitation == EXCITATION_CURRENT_500UA) excitation_current = ADS114S0X_IDAC_500_UA;
        else if (_excitation == EXCITATION_CURRENT_1000UA) excitation_current = ADS114S0X_IDAC_1000_UA;
        else if (_excitation == EXCITATION_CURRENT_2000UA) excitation_current = ADS114S0X_IDAC_2000_UA;
        _adc->setExcitation(excitation_current);

        /* Reset Internal MUX */
        _adc->setInternalMux(static_cast<ads114s0x_adc_input_e>(_pins.sP), static_cast<ads114s0x_adc_input_e>(_pins.sN));

        /* Wait for stabilization if needed */
        _adc->waitStabilization();

        /* Read adc value */
        adcCode = _adc->read();

        /* Stop exitation */
        _adc->setExcitation(ADS114S0X_IDAC_OFF);

        /* Reset Internal MUX */
        _adc->setInternalMux(ADS114S0X_NOT_CONNECTED, ADS114S0X_NOT_CONNECTED);

        /* MUX Configuration:
        * Disconnect input
        * Disconnect output */
        _highSideMux->route(INPUT_OPEN_HS, _pins.hs);
        _lowSideMux->route(_pins.ls, OUTPUT_OPEN_LS);
        break;
    }

    default:
        break;
    }

    return adcCode/SG_GAIN;
}