/**
 * @file Sensor.h
 * @brief Sensor
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

enum AIn_Num_e : int32_t {
    AIN_NULL = -1,
    AIN_A_P = 0,
    AIN_A_N,
    AIN_B_P,
    AIN_B_N,
    AIN_C_P,
    AIN_C_N,
    AIN_D_P,
    AIN_D_N,
    AIN_E_P,
    AIN_E_N,
    AIN_MAX
};
typedef enum AIn_Num_e AIn_Num_t;

enum Sensor_Type_e : int32_t {
    RAW_SENSOR, // Read differential input voltage
    RTD_PT100,
    RTD_PT1000,
    THERMOCOUPLE_B, // From 250°C to 1820°C
    THERMOCOUPLE_E, // From -200°C to 1000°C 
    THERMOCOUPLE_J, // From -210°C to 1200°C
    THERMOCOUPLE_K, // From -270°C to 1372°C
    THERMOCOUPLE_N, // From -200°C to 1200°C
    THERMOCOUPLE_R, // From -50°C to 1768°C
    THERMOCOUPLE_S, // From -50°C to 1768°C
    THERMOCOUPLE_T, // From -200°C to 400°C
    STRAIN_GAUGE,
};

enum Sensor_Gain_e : int32_t {
    GAIN_1 = 0,
    GAIN_2,
    GAIN_4,
    GAIN_8,
    GAIN_16,
    GAIN_32,
    GAIN_64,
    GAIN_128
};

enum Sensor_Ref_e : int32_t {
    REFERENCE_EXCITATION = ADS114S0X_REF_REFP0_REFNO,
    REFERENCE_IDAC_1 = ADS114S0X_REF_REFP1_REFN1,
    REFERENCE_INTERNAL_2_5V = ADS114S0X_REF_INTERNAL_2_5V
};

enum Sensor_Excitation_Current_e : int32_t {
    EXCITATION_OFF = ADS114S0X_IDAC_OFF,
    EXCITATION_10_UA = ADS114S0X_IDAC_10_UA,
    EXCITATION_50_UA = ADS114S0X_IDAC_50_UA,
    EXCITATION_100_UA = ADS114S0X_IDAC_100_UA,
    EXCITATION_250_UA = ADS114S0X_IDAC_250_UA,
    EXCITATION_500_UA = ADS114S0X_IDAC_500_UA,
    EXCITATION_750_UA = ADS114S0X_IDAC_750_UA,
    EXCITATION_1000_UA = ADS114S0X_IDAC_1000_UA,
    EXCITATION_1500_UA = ADS114S0X_IDAC_1500_UA,
    EXCITATION_2000_UA = ADS114S0X_IDAC_2000_UA
};

enum StrainGauge_Excitation_e : int32_t {
    EXCITATION_VOLTAGE_3V3 = 0,
    EXCITATION_VOLTAGE_5V,
    EXCITATION_VOLTAGE_USER,
    EXCITATION_CURRENT_10UA,
    EXCITATION_CURRENT_100UA,
    EXCITATION_CURRENT_500UA,
    EXCITATION_CURRENT_1000UA,
    EXCITATION_CURRENT_2000UA
};

enum Sensor_Parameter_e : int32_t {
    PARAMETER_GAIN = 0x00,
    PARAMETER_BIAS = 0x01,
    PARAMETER_REFERENCE = 0x02,
    PARAMETER_EXCITATION_CURRENT = 0x03,
    PARAMETER_MUX_HS_INDEX = 0x04,
    PARAMETER_MUX_HS_INPUT = 0x05,
    PARAMETER_MUX_LS_INDEX = 0x06,
    PARAMETER_MUX_LS_OUTPUT = 0x07,
    PARAMETER_SG_EXCITATION = 0x08,
    PARAMETER_ACQUISITION_TIME = 0x09,
    PARAMETER_STABILIZATION_TIME = 0x0A,
};

enum AcquisitionDuration_e : int32_t {
    SAMPLE_400_MS = 0,
    SAMPLE_200_MS,
    SAMPLE_100_MS,
    SAMPLE_60_MS,
    SAMPLE_50_MS, // Default value
    SAMPLE_20_MS,
    SAMPLE_16_6_MS,
    SAMPLE_10_MS,
    SAMPLE_5_MS,
    SAMPLE_2_5_MS,
    SAMPLE_1_25_MS,
    SAMPLE_1_MS
};

union Mux_Parameter_u {
    Multiplexer_Input_e input;
    Multiplexer_Output_e output;
    uint32_t hs_index;
    uint32_t ls_index;
};

union Sensor_Parameter_Value_u {
    Sensor_Gain_e gain;
    bool bias;
    Sensor_Ref_e reference;
    Sensor_Excitation_Current_e excitation_current;
    StrainGauge_Excitation_e sg_excitation;
    Mux_Parameter_u mux_parameter;
    AcquisitionDuration_e acquisition_time;
    int32_t stabilization_time;
    int32_t value;
};

struct Sensor_Pinout_s {
    std::array<AIn_Num_t, 4> ainPins;
    std::array<ADC_Input_t, 4> adcPins;
};
