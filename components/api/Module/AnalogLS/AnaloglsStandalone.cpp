#include "ModuleStandalone.h"
#include "AnaloglsStandalone.h"
#include "AnaloglsPinout.h"

#if defined(OI_ANALOG_LS)

typedef struct{
    float seebeck_coef;
    float current;
    float pt;
}rtd_param_t;

typedef struct{
    float seebeck_coef;
    float Tref;
    float Voffset;
}thermocouple_param_t;

static const char ANALOG_TAG[] = "analogls"; 

float adc_idacmag[16] = {0, 10e-6, 50e-6, 100e-6, 250e-6, 500e-6, 750e-6, 1000e-6, 1500e-6, 2000e-6, 0, 0, 0, 0, 0, 0};
float adc_datarate[14] = {2.5, 5, 10, 16, 20, 50, 60, 100, 200, 400, 800, 1000, 2000, 4000};
float res_tolerance = 1e-3;
float qmin = (2 * VREF / ADS1X4S08_GAIN_128*ADS1X4S08_GAIN_128)/ (float)pow(2, ADC_RESOLUTION);

bool channel_sel[4] = {false, false, false, false};
bool idacsel[4] = {false, false, false, false};
bool bool_channel_cal[4] = {false, false, false, false};
int channel_cal[4] = {0, 0, 0, 0};
sensor_type_t channel_sensor[4];
ads1x4s08_map_register_t channel_mapreg[4];
QueueHandle_t channelQueue[4] = {NULL, NULL, NULL, NULL};
int queue_sz = 256;
uint8_t channel_inpmux[4] = {ADS1X4S08_ADC_CHANNEL_A, ADS1X4S08_ADC_CHANNEL_B, ADS1X4S08_ADC_CHANNEL_C, ADS1X4S08_ADC_CHANNEL_D};
char channel_name[4] = {'A', 'B', 'C', 'D'};
ads1x4s08_data_t channel_data[4];


rtd_param_t rtd_param[4];
thermocouple_param_t thermocouple_param[4];

/**
 * @brief Infinite loop
 * @retval None
*/
// void AnaloglsStandalone::loop(rtd_type_t rtd){
//     ADS1x4S08_start_conversion();
//     while (1)
//         AnaloglsStandalone::get_channel_data(rtd);
// }

/**
 * @brief Initialisation of the driver with the GPIOs and peripherals
 * @retval None
*/
void AnaloglsStandalone::init(void)
{
    ESP_LOGI(ANALOG_TAG, "Init");
    ModuleStandalone::init();
    ADS1x4S08_init(ADC_START, ADC_DRDY, ADC_RESET, MOSI, MISO, SCLK, CS);
    STDS75_init(SDA, SCL, OS_INT);
    CD74HC4052_init(SEL0, SEL1);
 
    // Add the options STATUS and CRC to the 16-bit conversion data
    ADS1x4S08_set_data_format(ADS1X4S08_ENABLE, ADS1X4S08_ENABLE);

    // Set the datarate of the ADC
    ADS1x4S08_set_datarate(ADS1X4S08_DR_50_SPS);

    ADS1x4S08_start_conversion();
}

/**
 * @brief Enables or disables one or two excitation currents
 * @param[out] ads1x4s08_mapreg
 * @param[in] set_exc1 ENABLE or DISABLE (IDAC1)
 * @param[in] set_exc2 ENABLE or DISABLE (IDAC2)
 * @param[in] idac_magnitude See header ads114s08.h from line 74 to line 83
 * @param[in] update_register Use or not the set_register function for IDACMUX register
 * @retval None
*/
void AnaloglsStandalone::IdacConfig(ads1x4s08_map_register_t *ads1x4s08_mapreg, ads1x4s08_enable_t idac1, ads1x4s08_enable_t idac2, ads1x4s08_idac_mag_t idac_mag, bool update_register){
    if(idac1 == ADS1X4S08_ENABLE)
        ads1x4s08_mapreg->ads1x4s08_idacmux.ads1x4s08_i1mux = ADS1X4S08_AIN8;   // IDAC1 connected to AIN8
    else if(idac1 == ADS1X4S08_DISABLE)
        ads1x4s08_mapreg->ads1x4s08_idacmux.ads1x4s08_i1mux = ADS1X4S08_DISCONNECTED;
    else
        ESP_LOGE(ANALOG_TAG, "Please enter either ENABLE or DISABLE");
    
    if(idac2 == ADS1X4S08_ENABLE)
        ads1x4s08_mapreg->ads1x4s08_idacmux.ads1x4s08_i2mux = ADS1X4S08_AIN10;   // IDAC2 connected to AIN10
    else if(idac2 == ADS1X4S08_DISABLE)
        ads1x4s08_mapreg->ads1x4s08_idacmux.ads1x4s08_i2mux = ADS1X4S08_DISCONNECTED;
    else
        ESP_LOGE(ANALOG_TAG, "Please enter either ENABLE or DISABLE");

    ads1x4s08_mapreg->ads1x4s08_idacmag.ads1x4s08_imag = idac_mag;
    
    
    if(update_register){
        ADS1x4S08_set_register(ADS1X4S08_REG_IDACMAG  , (uint8_t*)&ads1x4s08_mapreg->ads1x4s08_idacmag);
        ADS1x4S08_set_register(ADS1X4S08_REG_IDACMUX, (uint8_t*)&ads1x4s08_mapreg->ads1x4s08_idacmux);
    }
}

/**
 * @brief Measures the current of IDAC1 and IDAC2
 * @note To measure both currents, set the multiplexer on channel A and connect 
 * the two inputs to the ground to guarentee the current return
 * @param[in] idac_mag Value of desired current
 * @param[in] resistor Value of the resistor where the current goes through
 * @retval None
*/
void AnaloglsStandalone::TestIdac(ads1x4s08_idac_mag_t idac_mag, float resistor){
    float v_idac;
    float idac;
    float q = 2 * VREF / pow(2, ADC_RESOLUTION);
    float adc_tolerance, idac_tolerance;

    CD74HC4052_output_selection(GPIO_NUM_48,GPIO_NUM_26,CD74HC4052_MUX_OUTPUT_CHANA);

    ADS1x4S08_set_gain(ADS1X4S08_GAIN_2);

    ads1x4s08_mapreg.ads1x4s08_idacmag.ads1x4s08_imag = idac_mag;

    ads1x4s08_mapreg.ads1x4s08_idacmux.ads1x4s08_i1mux = ADS1X4S08_AIN8;
    ads1x4s08_mapreg.ads1x4s08_idacmux.ads1x4s08_i2mux = ADS1X4S08_AIN10;

    ads1x4s08_mapreg.ads1x4s08_inpmux.ads1x4s08_muxp = ADS1X4S08_AIN8;
    ads1x4s08_mapreg.ads1x4s08_inpmux.ads1x4s08_muxn = ADS1X4S08_AIN9;
    
    ADS1x4S08_set_register(ADS1X4S08_REG_INPMUX, (uint8_t*)&ads1x4s08_mapreg.ads1x4s08_inpmux);
    ADS1x4S08_set_register(ADS1X4S08_REG_IDACMUX, (uint8_t*)&ads1x4s08_mapreg.ads1x4s08_idacmux);
    ADS1x4S08_set_register(ADS1X4S08_REG_IDACMAG  , (uint8_t*)&ads1x4s08_mapreg.ads1x4s08_idacmag);
    
    ADS1x4S08_start_conversion();
    ADS1x4S08_get_conversion_data();
    ADS1x4S08_stop_conversion();
    v_idac = ADS1x4S08_get_analog_voltage();
    ADS1x4S08_get_register(ADS1X4S08_REG_PGA, (uint8_t*)&ads1x4s08_mapreg.ads1x4s08_pga);
    adc_tolerance = q / (pow(2, ads1x4s08_mapreg.ads1x4s08_pga.ads1x4s08_gain) * 2 * sqrtf(3));
    idac_tolerance = adc_tolerance / v_idac + res_tolerance;

    idac = v_idac / resistor;
    ESP_LOGI(ANALOG_TAG, "IDAC1 = %f µA", idac * 1e6);
    float eps1 = fabs(idac - adc_idacmag[ads1x4s08_mapreg.ads1x4s08_idacmag.ads1x4s08_imag]) / adc_idacmag[ads1x4s08_mapreg.ads1x4s08_idacmag.ads1x4s08_imag];

    if(eps1 > idac_tolerance)
        ESP_LOGI(ANALOG_TAG, "Warning ! The measured current of IDAC1 is too much different with the desired current");

    ads1x4s08_mapreg.ads1x4s08_inpmux.ads1x4s08_muxp = ADS1X4S08_AIN10;
    ads1x4s08_mapreg.ads1x4s08_inpmux.ads1x4s08_muxn = ADS1X4S08_AIN11;
    ADS1x4S08_set_register(ADS1X4S08_REG_INPMUX, (uint8_t*)&ads1x4s08_mapreg.ads1x4s08_inpmux);

    ADS1x4S08_start_conversion();
    ADS1x4S08_gain_calibration();
    ADS1x4S08_get_conversion_data();
    ADS1x4S08_stop_conversion();
    v_idac = ADS1x4S08_get_analog_voltage();
    ADS1x4S08_get_register(ADS1X4S08_REG_PGA, (uint8_t*)&ads1x4s08_mapreg.ads1x4s08_pga);
    adc_tolerance = q / (pow(2, ads1x4s08_mapreg.ads1x4s08_pga.ads1x4s08_gain) * 2 * sqrtf(3));
    idac_tolerance = adc_tolerance / v_idac + res_tolerance;

    idac = v_idac / resistor;
    ESP_LOGI(ANALOG_TAG, "IDAC2 = %f µA", idac * 1e6);
    float eps2 = fabs(idac - adc_idacmag[ads1x4s08_mapreg.ads1x4s08_idacmag.ads1x4s08_imag]) / adc_idacmag[ads1x4s08_mapreg.ads1x4s08_idacmag.ads1x4s08_imag];

    if(eps2 > idac_tolerance)
        ESP_LOGI(ANALOG_TAG, "Warning ! The measured current of IDAC2 is too much different with the desired current");
}

/**
 * @brief Measures the reference temperature and determine the junction temperature
 * of the thermocouple
 * @param[in] adc_voltage Output voltage from ADC
 * @param[in] alpha Thermocouple coefficient
 * @param[in] Tref Reference temperature
 * @param[in] Voffset Voltage of thermocouple at Tref
 * @retval Temprature of thermocouple
*/
float AnaloglsStandalone::GetThermocoupleTemperature(float adc_voltage, float alpha, float Tref, float Voffset){
    return (adc_voltage - Voffset) / alpha + Tref;
}

/**
 * @brief Determine the temperature of the RTD
 * @param[in] adc_voltage Output voltage from ADC
 * @param[in] alpha RTD coefficient
 * @param[in] pt Type of RTD : PT100 or PT1000
 * @retval Temperature of RTD
*/
float AnaloglsStandalone::GetRtdTemperature(float adc_voltage, float current, float alpha, rtd_type_t pt){
        return alpha * (adc_voltage / current - (float)pt*100);
}

/**
 * @brief Determine the state of the level detector
 * @param[in] adc_voltage Output voltage from ADC
*/
float AnaloglsStandalone::GetLevelDetector(float adc_voltage){
    float level = 0;
    float Vth = 0.073;        // Threshold voltage (V) calculated in the worst case
        if(adc_voltage < Vth)
            {
            ESP_LOGV(ANALOG_TAG, "The level of the liquid is low");
            level = 0;
            }
        else
            {
            ESP_LOGV(ANALOG_TAG, "The level of the liquid is high");
            level = 1;
            }
return level;

}

/**
 * @brief ADC for voltage measurement
 * @param[in] channel Channel of the sensor
 * @param[in] range Voltage range of the sensor
 * @retval None
*/
void AnaloglsStandalone::AddVoltmeter(analog_channel_t channel, ads1x4s08_voltage_range_t range){
    channel_mapreg[channel] = ads1x4s08_mapreg;

    channel_sel[channel] = true;

    channel_mapreg[channel].ads1x4s08_inpmux.ads1x4s08_muxp = channel_inpmux[channel] >> 4;
    channel_mapreg[channel].ads1x4s08_inpmux.ads1x4s08_muxn = channel_inpmux[channel] & 0xf;

    idacsel[channel] = false;

    if(range == ADS1X4S08_AUTO_CALIBRATION)
    {
        bool_channel_cal[channel] = true;
        channel_mapreg[channel].ads1x4s08_pga.ads1x4s08_gain = ADS1X4S08_GAIN_128;
        channel_mapreg[channel].ads1x4s08_pga.ads1x4s08_pga_en = ADS1X4S08_ENABLE;
    }
    else
    {
        bool_channel_cal[channel] = false;
        if(range != ADS1X4S08_FULLSCALE_5V){
            channel_mapreg[channel].ads1x4s08_pga.ads1x4s08_pga_en = ADS1X4S08_ENABLE;
            channel_mapreg[channel].ads1x4s08_pga.ads1x4s08_gain = range;
        }
        else{            
            channel_mapreg[channel].ads1x4s08_pga.ads1x4s08_pga_en = ADS1X4S08_DISABLE;
            channel_mapreg[channel].ads1x4s08_pga.ads1x4s08_gain = ADS1X4S08_GAIN_1;
        }
    }

    switch (channel)
    {
        case ANALOG_CHANNEL_A:
        {
            ads1x4s08_mapreg.ads1x4s08_vbias.ads1x4s08_vb_ain1 = ADS1X4S08_ENABLE;
            break;
        }
        case ANALOG_CHANNEL_B:
        {
            ads1x4s08_mapreg.ads1x4s08_vbias.ads1x4s08_vb_ain3 = ADS1X4S08_ENABLE;
            break;
        }
        case ANALOG_CHANNEL_C:
        {
            ads1x4s08_mapreg.ads1x4s08_vbias.ads1x4s08_vb_ain2 = ADS1X4S08_ENABLE;
            break;
        }
        case ANALOG_CHANNEL_D:
        {
            ads1x4s08_mapreg.ads1x4s08_vbias.ads1x4s08_vb_ain0 = ADS1X4S08_ENABLE;
            break;
        }
        default:
            break;
    }
    ads1x4s08_mapreg.ads1x4s08_vbias.ads1x4s08_vb_level = ADS1X4S08_VBIAS_DIV2;
    ADS1x4S08_set_register(ADS1X4S08_REG_VBIAS, (uint8_t*)&ads1x4s08_mapreg.ads1x4s08_vbias);

    channel_sensor[channel] = VOLTMETER;

    //ESP_LOGV(ANALOG_TAG, "The Voltmeter is configured on channel %c", channel_name[channel]);
}

/**
 * @brief Configure ADC for RTD sensor
 * @param[in] channel Channel of the sensor
 * @param[in] type Type of RTD (PT100/PT1000)
 * @param[in] nb_wire Number of wires on sensor (2/3 wires)
 * @param[in] range Voltage range of the sensor
 * @param[in] current Magnitude of the current from excitation sources
 * @param[in] coef Temperature coefficient of RTD
 * @retval None
*/
void AnaloglsStandalone::AddRTD(analog_channel_t channel, rtd_type_t type, wire_t nb_wire, ads1x4s08_voltage_range_t range, ads1x4s08_idac_mag_t current, float coef){

    channel_mapreg[channel] = ads1x4s08_mapreg;
    
    channel_sel[channel] = true;

    rtd_param[channel].current = adc_idacmag[current];
    rtd_param[channel].pt = (float)type;
    rtd_param[channel].seebeck_coef = coef;

    channel_mapreg[channel].ads1x4s08_inpmux.ads1x4s08_muxp = channel_inpmux[channel] >> 4;
    channel_mapreg[channel].ads1x4s08_inpmux.ads1x4s08_muxn = channel_inpmux[channel] & 0xf;

    if(nb_wire == RTD_2WIRE)
        AnaloglsStandalone::IdacConfig(&channel_mapreg[channel], ADS1X4S08_ENABLE, ADS1X4S08_DISABLE, current, false);
    else if(nb_wire == RTD_3WIRE)
        AnaloglsStandalone::IdacConfig(&channel_mapreg[channel], ADS1X4S08_ENABLE, ADS1X4S08_ENABLE, current, false);
    idacsel[channel] = true;
    
    if(range == ADS1X4S08_AUTO_CALIBRATION)
    {
        bool_channel_cal[channel] = true;
        channel_mapreg[channel].ads1x4s08_pga.ads1x4s08_gain = ADS1X4S08_GAIN_128;
        channel_mapreg[channel].ads1x4s08_pga.ads1x4s08_pga_en = ADS1X4S08_ENABLE;
    }
    else
    {
        bool_channel_cal[channel] = false;
        if(range != ADS1X4S08_FULLSCALE_5V){
            channel_mapreg[channel].ads1x4s08_pga.ads1x4s08_pga_en = ADS1X4S08_ENABLE;
            channel_mapreg[channel].ads1x4s08_pga.ads1x4s08_gain = range;
        }
        else{            
            channel_mapreg[channel].ads1x4s08_pga.ads1x4s08_pga_en = ADS1X4S08_DISABLE;
            channel_mapreg[channel].ads1x4s08_pga.ads1x4s08_gain = ADS1X4S08_GAIN_1;
        }
    }

    channel_sensor[channel] = RTD;

    //ESP_LOGV(ANALOG_TAG, "The RTD is configured on channel %c", channel_name[channel]);
}

/**
 * @brief ADC for thermocouple
 * @param[in] channel Channel of the sensor
 * @param[in] range Voltage range of the sensor
 * @param[in] coef Temperature coefficient of thermocouple
 * @retval None
*/
void AnaloglsStandalone::AddThermocouple(analog_channel_t channel, ads1x4s08_voltage_range_t range, float coef){

    channel_mapreg[channel] = ads1x4s08_mapreg;
    
    channel_sel[channel] = true;

    thermocouple_param[channel].seebeck_coef = coef;
    thermocouple_param[channel].Tref = STDS75_get_temperature();
    thermocouple_param[channel].Voffset = 0;

    channel_mapreg[channel].ads1x4s08_inpmux.ads1x4s08_muxp = channel_inpmux[channel] >> 4;
    channel_mapreg[channel].ads1x4s08_inpmux.ads1x4s08_muxn = channel_inpmux[channel] & 0xf;

    AnaloglsStandalone::IdacConfig(&channel_mapreg[channel], ADS1X4S08_DISABLE, ADS1X4S08_DISABLE, ADS1X4S08_MAG_1000uA, false);
    idacsel[channel] = false;

    if(range == ADS1X4S08_AUTO_CALIBRATION)
    {
        bool_channel_cal[channel] = true;
        channel_mapreg[channel].ads1x4s08_pga.ads1x4s08_gain = ADS1X4S08_GAIN_128;
        channel_mapreg[channel].ads1x4s08_pga.ads1x4s08_pga_en = ADS1X4S08_ENABLE;
    }
    else
    {
        bool_channel_cal[channel] = false;
        if(range != ADS1X4S08_FULLSCALE_5V){
            channel_mapreg[channel].ads1x4s08_pga.ads1x4s08_pga_en = ADS1X4S08_ENABLE;
            channel_mapreg[channel].ads1x4s08_pga.ads1x4s08_gain = range;
        }
        else{            
            channel_mapreg[channel].ads1x4s08_pga.ads1x4s08_pga_en = ADS1X4S08_DISABLE;
            channel_mapreg[channel].ads1x4s08_pga.ads1x4s08_gain = ADS1X4S08_GAIN_1;
        }
    }

    switch (channel)
    {
        case ANALOG_CHANNEL_A:
        {
            ads1x4s08_mapreg.ads1x4s08_vbias.ads1x4s08_vb_ain1 = ADS1X4S08_ENABLE;
            break;
        }
        case ANALOG_CHANNEL_B:
        {
            ads1x4s08_mapreg.ads1x4s08_vbias.ads1x4s08_vb_ain3 = ADS1X4S08_ENABLE;
            break;
        }
        case ANALOG_CHANNEL_C:
        {
            ads1x4s08_mapreg.ads1x4s08_vbias.ads1x4s08_vb_ain2 = ADS1X4S08_ENABLE;
            break;
        }
        case ANALOG_CHANNEL_D:
        {
            ads1x4s08_mapreg.ads1x4s08_vbias.ads1x4s08_vb_ain0 = ADS1X4S08_ENABLE;
            break;
        }
        default:
            break;
    }
    ads1x4s08_mapreg.ads1x4s08_vbias.ads1x4s08_vb_level = ADS1X4S08_VBIAS_DIV2;
    ADS1x4S08_set_register(ADS1X4S08_REG_VBIAS, (uint8_t*)&ads1x4s08_mapreg.ads1x4s08_vbias);

    channel_sensor[channel] = THERMOCOUPLE;

    //ESP_LOGV(ANALOG_TAG, "The thermocouple is configured on channel %c", channel_name[channel]);
}

/**
 * @brief Configure ADC for level detector
 * @param[in] channel Channel of the sensor
 * @retval None
*/
void AnaloglsStandalone::AddLevelDetector(analog_channel_t channel){

    channel_mapreg[channel] = ads1x4s08_mapreg;
    
    channel_sel[channel] = true;

    AnaloglsStandalone::IdacConfig(&channel_mapreg[channel], ADS1X4S08_DISABLE, ADS1X4S08_DISABLE, ADS1X4S08_MAG_1000uA, false);
    idacsel[channel] = false;
    
    channel_mapreg[channel].ads1x4s08_inpmux.ads1x4s08_muxn = ADS1X4S08_AINCOM;
    channel_mapreg[channel].ads1x4s08_inpmux.ads1x4s08_muxp = channel_inpmux[channel] & 0xf;

    switch (channel)
    {
        case ANALOG_CHANNEL_A:
        {
            ads1x4s08_mapreg.ads1x4s08_vbias.ads1x4s08_vb_ain1 = ADS1X4S08_ENABLE;
            break;
        }
        case ANALOG_CHANNEL_B:
        {
            ads1x4s08_mapreg.ads1x4s08_vbias.ads1x4s08_vb_ain3 = ADS1X4S08_ENABLE;
            break;
        }
        case ANALOG_CHANNEL_C:
        {
            ads1x4s08_mapreg.ads1x4s08_vbias.ads1x4s08_vb_ain2 = ADS1X4S08_ENABLE;
            break;
        }
        case ANALOG_CHANNEL_D:
        {
            ads1x4s08_mapreg.ads1x4s08_vbias.ads1x4s08_vb_ain0 = ADS1X4S08_ENABLE;
            break;
        }
        default:
            break;
    }
    ads1x4s08_mapreg.ads1x4s08_vbias.ads1x4s08_vb_level = ADS1X4S08_VBIAS_DIV2;
    ADS1x4S08_set_register(ADS1X4S08_REG_VBIAS, (uint8_t*)&ads1x4s08_mapreg.ads1x4s08_vbias);

    bool_channel_cal[channel] = false;
    channel_mapreg[channel].ads1x4s08_pga.ads1x4s08_pga_en = ADS1X4S08_DISABLE;
    channel_mapreg[channel].ads1x4s08_pga.ads1x4s08_gain = ADS1X4S08_GAIN_1;

    channel_sensor[channel] = LEVEL_DETECTOR;

    ESP_LOGV(ANALOG_TAG, "The level detector is configured on channel %c", channel_name[channel]);
}

/**
 * @brief Configuration of ADC for Wheatstone bridge
 * @param[in] channel Channel of the ensor
 * @retval None
*/
//void AnaloglsStandalone::add_Wheatstone_Bridge(analog_channel_t channel){}

/**
 * @brief Do not make voltage measurement on selected channel
 * @param[in] channel Channel of the sensor
 * @retval None
*/
//void AnaloglsStandalone::remove_sensor_from_channel(analog_channel_t channel){
//    channel_sel[channel] = false;
//    ESP_LOGI(ANALOG_TAG, "The sensor has been removed from channel %c", channel_name[channel]);
//}

/**
 * @brief Get the sensor value reguarding the type of sensor and send it to the serial monitor
 * @param[in] channel Channel of the sensor
 * @param[in] sensor Type of the sensor
 *  @param[in] rtd Type of rtd PT100 or PT1000 
*/
float AnaloglsStandalone::GetSensorValue(analog_channel_t channel, sensor_type_t sensor, rtd_type_t rtd){

    float Tsensor = 1;
    float adc_voltage = ADS1x4S08_get_analog_voltage();
    switch (sensor)
    {
        case VOLTMETER:
        {
            ESP_LOGV(ANALOG_TAG,"Voltage = %f V", adc_voltage);
            Tsensor = adc_voltage;
            // printf("Voltmeter = %f \n", Tsensor);
            break;
        }
        case RTD:
        {
            
            Tsensor = AnaloglsStandalone::GetRtdTemperature(adc_voltage, rtd_param[channel].current, rtd_param[channel].seebeck_coef, rtd);
            // printf("RTD Temp = %f \n", Tsensor);
            ESP_LOGV(ANALOG_TAG, "RTD temperature = %f °C", Tsensor);
            break;
        }
        case THERMOCOUPLE:
        {
            
            Tsensor = AnaloglsStandalone::GetThermocoupleTemperature(adc_voltage, thermocouple_param[channel].seebeck_coef, thermocouple_param[channel].Tref, thermocouple_param[channel].Voffset);
            // printf("TC = %f \n", Tsensor);
            ESP_LOGV(ANALOG_TAG, "Thermocouple temperature = %f °C", Tsensor);
            break;
        }
        case LEVEL_DETECTOR:
        {
            Tsensor = AnaloglsStandalone::GetLevelDetector(adc_voltage);
        }
        /*
        case 'other sensor':
        {
            break;
        }
        */
        default: 
        break;
    }
 return Tsensor;
}

/**
 * @brief Get data on each used channel
 * @retval None
*/
float AnaloglsStandalone::GetData(rtd_type_t rtd){
    
    float value = 0;
    for (int channel = ANALOG_CHANNEL_A; channel <= ANALOG_CHANNEL_D; channel++)
    {

        if (channel_sel[channel])
        {            
            ESP_LOGV(ANALOG_TAG, "Channel %c :", channel_name[channel]);
            if(idacsel[channel]){
                ADS1x4S08_set_register(ADS1X4S08_REG_IDACMAG  , (uint8_t*)&channel_mapreg[channel].ads1x4s08_idacmag);
                ADS1x4S08_set_register(ADS1X4S08_REG_IDACMUX, (uint8_t*)&channel_mapreg[channel].ads1x4s08_idacmux);
                CD74HC4052_output_selection(GPIO_NUM_48,GPIO_NUM_26, (cd74hc4052_mux_channel_t)channel);
            }

            ADS1x4S08_set_register(ADS1X4S08_REG_INPMUX, (uint8_t*)&channel_mapreg[channel].ads1x4s08_inpmux);
            
            if(channel_cal[channel] & (channel_cal[channel] == 0))
                {
                    ads1x4s08_mapreg.ads1x4s08_pga.ads1x4s08_gain = channel_mapreg[channel].ads1x4s08_pga.ads1x4s08_gain;
                    ADS1x4S08_gain_calibration();
                    channel_mapreg[channel].ads1x4s08_pga.ads1x4s08_gain = ads1x4s08_mapreg.ads1x4s08_pga.ads1x4s08_gain;
                    channel_mapreg[channel].ads1x4s08_pga.ads1x4s08_pga_en = ads1x4s08_mapreg.ads1x4s08_pga.ads1x4s08_pga_en;
                    channel_cal[channel] = 1;
                }
            else{
                ADS1x4S08_set_register(ADS1X4S08_REG_PGA, (uint8_t*)&channel_mapreg[channel].ads1x4s08_pga);
                ADS1x4S08_pga_saturation();
            }
            ADS1x4S08_get_conversion_data();
            if(ADS1x4S08_pga_saturation())
            {
                channel_cal[channel] = 0;
            }
            else
                {
                value = AnaloglsStandalone::GetSensorValue((analog_channel_t)channel, channel_sensor[channel], rtd);
                }
        }

    }

return value;
}

float AnaloglsStandalone::GetChannelData(analog_channel_t channel,rtd_type_t rtd)
{
    float value = 0;
if (channel_sel[channel])
        {            
    ESP_LOGV(ANALOG_TAG, "Channel %c :", channel_name[channel]);
            if(idacsel[channel]){
                ADS1x4S08_set_register(ADS1X4S08_REG_IDACMAG  , (uint8_t*)&channel_mapreg[channel].ads1x4s08_idacmag);
                ADS1x4S08_set_register(ADS1X4S08_REG_IDACMUX, (uint8_t*)&channel_mapreg[channel].ads1x4s08_idacmux);
                CD74HC4052_output_selection(GPIO_NUM_48,GPIO_NUM_26, (cd74hc4052_mux_channel_t)channel);
            }

            ADS1x4S08_set_register(ADS1X4S08_REG_INPMUX, (uint8_t*)&channel_mapreg[channel].ads1x4s08_inpmux);
            
            if(channel_cal[channel] & (channel_cal[channel] == 0))
                {
                    ads1x4s08_mapreg.ads1x4s08_pga.ads1x4s08_gain = channel_mapreg[channel].ads1x4s08_pga.ads1x4s08_gain;
                    ADS1x4S08_gain_calibration();
                    channel_mapreg[channel].ads1x4s08_pga.ads1x4s08_gain = ads1x4s08_mapreg.ads1x4s08_pga.ads1x4s08_gain;
                    channel_mapreg[channel].ads1x4s08_pga.ads1x4s08_pga_en = ads1x4s08_mapreg.ads1x4s08_pga.ads1x4s08_pga_en;
                    channel_cal[channel] = 1;
                }
            else{
                ADS1x4S08_set_register(ADS1X4S08_REG_PGA, (uint8_t*)&channel_mapreg[channel].ads1x4s08_pga);
                ADS1x4S08_pga_saturation();
            }

            ADS1x4S08_get_conversion_data();
            if(ADS1x4S08_pga_saturation())
                channel_cal[channel] = 0;
            else
                value = AnaloglsStandalone::GetSensorValue(channel, channel_sensor[channel],rtd);
        }
        
return value;
}
#endif