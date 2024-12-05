/**
 * @file global_sensor.h
 * @brief Global sensor
 * @author Mani Gillier (mani.gillier@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "_ADS114S0X.h"
#include "Multiplexer.h"
#include "Sensor.h"

struct Dual_Mux_Config_s {
    Multiplexer_Input_e input;
    Multiplexer_Output_e output;
    uint8_t hs_index;
    uint8_t ls_index;
};

#define SENSOR_TAG "Sensor"
#define SENSOR_FUNCTIONNALITY_NOT_FOUND_MESSAGE "The sensor you requested doesn't have this functionnality (%s)."
#define SENSOR_FUNCTIONNALITY_NOT_FOUND ESP_LOGE(SENSOR_TAG, SENSOR_FUNCTIONNALITY_NOT_FOUND_MESSAGE, __PRETTY_FUNCTION__);
#define SENSOR_FUNCTIONNALITY_NOT_FOUND_RETURN(value) ESP_LOGE(SENSOR_TAG, SENSOR_FUNCTIONNALITY_NOT_FOUND_MESSAGE, __PRETTY_FUNCTION__); return value;

class Sensor
{
    public:
    Sensor(ADS114S0X* adc, Multiplexer* highSideMux, Multiplexer* lowSideMux, const Sensor_Pinout_s& pins, Sensor_Type_e type, uint32_t index) :
        _adc(adc),
        _highSideMux(highSideMux),
        _lowSideMux(lowSideMux),
        _ainPins(pins.ainPins),
        _adcPins(pins.adcPins),
        _gain(GAIN_1),
        _reference(REFERENCE_INTERNAL_2_5V),
        _bias_active(true),
        _excitation(EXCITATION_OFF),
        _mux_config({
            .input = INPUT_OPEN_HS,
            .output = OUTPUT_OPEN_LS,
            .hs_index = 0,
            .ls_index = 1,
        }),
        _type(type),
        _index(index) {}

    /* Found on all sensors */
    virtual float read(bool print_result = false);
    virtual int16_t raw_read(uint8_t positive_index = 0, uint8_t negative_index = 1, bool print_result = false);

    /* Sets a parameter */
    virtual inline void setParameter(Sensor_Parameter_e parameter, Sensor_Parameter_Value_u value) { SENSOR_FUNCTIONNALITY_NOT_FOUND };
    /* Read the value converted in millivolts */
    virtual inline float readMillivolts(bool print_result = false) { SENSOR_FUNCTIONNALITY_NOT_FOUND_RETURN(0.0) };
    /* Read the resistance of an RTD */
    virtual inline float readResistor(bool print_result = false) { SENSOR_FUNCTIONNALITY_NOT_FOUND_RETURN(0.0) };
    /* Read the temperature */
    virtual inline float readTemperature(bool print_result = false) { SENSOR_FUNCTIONNALITY_NOT_FOUND_RETURN(0.0) };
    /* Set the excitation mode for the strain gauge */
    virtual inline void setSGExcitationMode(StrainGauge_Excitation_e excitation) { SENSOR_FUNCTIONNALITY_NOT_FOUND };

    inline uint32_t get_index(void) { return _index; }
    inline enum Sensor_Type_e get_type(void) { return _type; }
    inline std::array<AIn_Num_t, 4> get_ain_pins(void) { return _ainPins; }

    protected:
    /* Found on all sensors */
    /* Used for the read method */
    void route(void);
    void init_read(void);
    void reset_read(void);
    void print_int16(const int16_t value, const char *unit_str);
    void print_float(const float value, const char *unit_str);
    /* Used for the setParameter method */
    virtual inline void setGain(Sensor_Gain_e gain) { SENSOR_FUNCTIONNALITY_NOT_FOUND };
    virtual inline void setReference(Sensor_Ref_e reference) { SENSOR_FUNCTIONNALITY_NOT_FOUND };
    virtual inline void setBiasActive(bool active) { SENSOR_FUNCTIONNALITY_NOT_FOUND };
    virtual inline void setExcitation(Sensor_Excitation_Current_e excitation) { SENSOR_FUNCTIONNALITY_NOT_FOUND };

    ADS114S0X* _adc;
    Multiplexer* _highSideMux;
    Multiplexer* _lowSideMux;
    std::array<AIn_Num_t, 4> _ainPins;
    std::array<ADC_Input_t, 4> _adcPins;

    Sensor_Gain_e _gain;
    Sensor_Ref_e _reference;
    bool _bias_active;
    Sensor_Excitation_Current_e _excitation;
    Dual_Mux_Config_s _mux_config;

    enum Sensor_Type_e _type;
    uint32_t _index;
};
