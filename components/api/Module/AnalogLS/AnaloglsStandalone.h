#pragma once

#include "ModuleStandalone.h"
#include "AnaloglsType.h"

#if defined(CONFIG_ANALOG_LS) 

// Set ADC_RESOLUTION between 16 and 24 bits
#define ADC_RESOLUTION  16
#define DATA_FORMAT     (ADC_RESOLUTION - 16)


class AnaloglsStandalone : public ModuleStandalone
{
public:
/* Custom OpenIndus functions */

    static void setup(void);

    static void loop(rtd_type_t rtd);

    static void init();

    static void TestIdac(ads1x4s08_idac_mag_t idac_mag, float resistor);

    static void IdacConfig(ads1x4s08_map_register_t *ads1x4s08_mapreg, ads1x4s08_enable_t idac1, ads1x4s08_enable_t idac2, ads1x4s08_idac_mag_t idac_mag, bool update_register);

    static float GetThermocoupleTemperature(float adc_voltage, float alpha, float Tref, float Voffset);

    static float GetRtdTemperature(float adc_voltage, float current, float alpha, rtd_type_t pt);

    static float GetLevelDetector(float adc_voltage);

    static void AddVoltmeter(analog_channel_t channel, ads1x4s08_voltage_range_t range);

    static void AddRTD(analog_channel_t channel, rtd_type_t type, wire_t nb_wire, ads1x4s08_voltage_range_t range, ads1x4s08_idac_mag_t current, float coef);

    static void AddThermocouple(analog_channel_t channel, ads1x4s08_voltage_range_t range, float coef);

    static void AddLevelDetector(analog_channel_t channel);

    static float GetSensorValue(analog_channel_t channel, sensor_type_t sensor, rtd_type_t rtd);

    /* Function not coded yet
    void AnalogLS_add_Wheatstone_Bridge(analog_channel_t channel);
    */

    static float GetData(rtd_type_t rtd);

    static float GetChannelData(analog_channel_t channel,rtd_type_t rtd);
private:
    
};

#endif





