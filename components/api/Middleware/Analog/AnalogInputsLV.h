/**
 * @file AnalogInputs.h
 * @brief Analog Input
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include <string.h>
#include "ads866x.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "esp_efuse.h"

#define AIN_CURRENT_MODE_RES_VALUE   100.0f

#define ESP_ADC_NO_OF_SAMPLES        128U
#define ESP_ADC_DEFAULT_COEFF_A      11.6965f
#define ESP_ADC_DEFAULT_COEFF_B      0.0f

typedef enum {
    AIN_1 = 0,
    AIN_2 = 1,
    AIN_3 = 2,
    AIN_4 = 3,
    AIN_MAX
} AnalogInput_Num_t;

typedef enum {
    AIN_MODE_VOLTAGE = 0,
    AIN_MODE_CURRENT = 1,
    AIN_MODE_UNDEFINED
} AnalogInput_Mode_t;

typedef enum {
    AIN_VOLTAGE_RANGE_0_10V24 = 5,
    AIN_VOLTAGE_RANGE_0_5V12 = 6,
    AIN_VOLTAGE_RANGE_0_2V56 = 7,
    AIN_VOLTAGE_RANGE_0_1V28 = 8,
    AIN_VOLTAGE_RANGE_UNDEFINED
} AnalogInput_VoltageRange_t;

typedef enum {
    AIN_UNIT_RAW = 0,
    AIN_UNIT_MILLIVOLT = 1,
    AIN_UNIT_MILLIAMP = 2,
    AIN_UNIT_VOLT = 3,
    AIN_UNIT_AMP = 4,
    AIN_UNIT_UNDEFINED
} AnalogInput_Unit_t;

typedef enum {
    ANALOG_INPUT_ADS866X = 0,
    ANALOG_INPUT_ESP32S3,
    ANALOG_INPUT_MAX
} AnalogInputType_t;

typedef struct {
    float ain_coeff_a;
    float ain_coeff_b;
} AnalogInput_eFuse_Coeff_t;

class AnalogInputAds866x 
{
public:

    AnalogInputAds866x(uint8_t num, gpio_num_t cmdGpio);

    void init(AnalogInput_VoltageRange_t range, AnalogInput_Mode_t mode);

    int read(void);
    float read(AnalogInput_Unit_t unit);
    void setMode(AnalogInput_Mode_t mode);
    uint8_t getMode(void);
    void setVoltageRange(AnalogInput_VoltageRange_t range);
    uint8_t getVoltageRange(void);
    gpio_num_t getModePin();

private:

    int _num;
    gpio_num_t _modePin;
    AnalogInput_Mode_t _mode;
    AnalogInput_VoltageRange_t _voltage_range;
};

class AnalogInputEsp32s3
{
public:

    AnalogInputEsp32s3(uint8_t num, adc_channel_t channel, adc_unit_t adc_unit);

    void init(void);
    int read(void);
    float read(AnalogInput_Unit_t unit);
    
    void getCoeffs();
    float applyCoeffs(float voltage);

private:

    int _num;
    adc_channel_t _channel;
    adc_unit_t _adc_unit;
    esp_adc_cal_characteristics_t _adc_characteristic;
    AnalogInput_eFuse_Coeff_t _coeff;
};

class AnalogInputs
{
public:

    AnalogInputs(const gpio_num_t* cmdGpio, uint8_t nb);

    static int init(ads866x_config_t *ads866xConfig, AnalogInput_VoltageRange_t range, AnalogInput_Mode_t mode);
    static int init(const adc_channel_t* channel, adc_unit_t adc_unit, uint8_t nb);

    /**
     * @brief Read the value of AIN.
     * The function return an integer that correspond to the raw value of the ANA.
     * On OICore and OIDiscrete, the returned value correspond to the raw internal voltage and not raw value. 
     * It is better to use analogReadVolts or analogReadMillivolts.
     * @param ain ANA input.
     * @return int Value of the AIN input.
     */
    static int analogRead(AnalogInput_Num_t num);
    
    /**
     * @brief Read the value of AIN.
     * The function return a float that correspond to the voltage of the ANA (from 0 to 30V).
     *
     * @param ain ANA input.
     * @return float Value of the AIN input.
     */
    static float analogReadVolt(AnalogInput_Num_t num);
    
    /**
     * @brief Read the value of AIN.
     * The function return a float that correspond to the voltage of the ANA (from 0 to 30000mV).
     *
     * @param ain ANA input.
     * @return float Value of the AIN input.
     */
    static float analogReadMilliVolt(AnalogInput_Num_t num);
    
    static int setAnalogCoeffs(float* a, float* b);

    // To delete
    int read(AnalogInput_Num_t num);
    void setMode(AnalogInput_Num_t num, AnalogInput_Mode_t mode);
    uint8_t getMode(AnalogInput_Num_t num);
    void setVoltageRange(AnalogInput_Num_t num, AnalogInput_VoltageRange_t range);
    uint8_t getVoltageRange(AnalogInput_Num_t num);
    
     // keep this one in private
    static float read(AnalogInput_Num_t num, AnalogInput_Unit_t unit);

private:

    static uint8_t _nb;
    static AnalogInputType_t _type;

    static AnalogInputAds866x* _ains[AIN_MAX];
    static AnalogInputEsp32s3* _ains_esp[AIN_MAX];
};