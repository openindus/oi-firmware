/**
 * @file AnalogInputsLS.cpp
 * @brief Analog inputs low signal
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "AnalogInputsLS.h"
#include "Sensor.h"
#include <algorithm>

static const char TAG[] = "AnalogInputsLS";

static const std::array<ADC_Input_t, 10> AIN_TO_ADC_INPUT = {
    5, 0, 8, 1, 9, 2, 10, 3, 11, 4
};

static const std::array<Mux_IO_t, 10> AIN_TO_MUX_IO = {
    0, 0, 1, 1, 2, 2, 3, 3, 4, 4
};

ADS114S0X* AnalogInputsLS::_adc = NULL;
Multiplexer* AnalogInputsLS::_highSideMux = NULL;
Multiplexer* AnalogInputsLS::_lowSideMux = NULL;

int AnalogInputsLS::_init(void)
{
    int ret = 0;

    /* ADC */
    if (_adc != NULL) {
        ret |= _adc->init();
    } else {
        ESP_LOGE(TAG, "Failed to initialize ADC device");
        ret |= -1;
    }

    /* Multiplexer (high side/ low side)*/
    if ((_highSideMux != NULL) && (_lowSideMux != NULL)) {
        ret |= _highSideMux->init();
        ret |= _lowSideMux->init();
    } else {
        ESP_LOGE(TAG, "Failed to initialize multiplexer");
        ret |= -1;
    }

    _registerCLI();

    return ret;
}

std::vector<Sensor *> AnalogInputsLS::sensors;

int AnalogInputsLS::addSensor(Sensor_Type_e type, std::vector<AIn_Num_t> ainPins)
{
    if (!std::all_of(ainPins.begin(), ainPins.end(), [](AIn_Num_t aIn) {
        return (aIn >= AIN_A_P && aIn < AIN_MAX) || aIn == AIN_NULL;
    })) {
        ESP_LOGE(TAG, "One or more AINs are out of range (0 to AIN_MAX - 1).");
        return -1;
    }

    uint8_t nbr_pin_set = 0;
    Sensor_Pinout_s pinout = {
        .ainPins = (std::array<AIn_Num_t, 4>) {AIN_NULL, AIN_NULL, AIN_NULL, AIN_NULL},
        .adcPins = (std::array<ADC_Input_t, 4>) {-1, -1, -1, -1}
    };
    for (int i = 0; i < ainPins.size(); i++) {
        pinout.ainPins[i] = ainPins[i];
        if (pinout.ainPins[i] == AIN_NULL) {
            break;
        }
        pinout.adcPins[i] = AIN_TO_ADC_INPUT[pinout.ainPins[i]];
        nbr_pin_set++;
    }
    Sensor *sensor_ptr = NULL;
    uint32_t index = sensors.size();

    switch (type) 
    {
        case RAW_SENSOR:
            if (nbr_pin_set < 2) {
                ESP_LOGE(TAG, "Raw sensor requires at least 2 AIns.");
                return -1;
            }
            sensor_ptr = new RawSensor(_adc, _highSideMux, _lowSideMux, pinout, index);
            sensors.emplace_back(sensor_ptr);
            return index;
        case RTD_PT100:
        case RTD_PT1000:
            if (nbr_pin_set < 2) {
                ESP_LOGE(TAG, "RTD require at least 2 AIns.");
                return -1;
            }
            sensor_ptr = new RTD(_adc, _highSideMux, _lowSideMux, pinout, type == RTD_PT100 ? PT100 : PT1000, index);
            sensors.emplace_back(sensor_ptr);
            return index;
        case THERMOCOUPLE_B:
        case THERMOCOUPLE_E:
        case THERMOCOUPLE_J:
        case THERMOCOUPLE_K:
        case THERMOCOUPLE_N:
        case THERMOCOUPLE_R:
        case THERMOCOUPLE_S:
        case THERMOCOUPLE_T:
            if (nbr_pin_set < 2) {
                ESP_LOGE(TAG, "THERMOCOUPLE requires 2 AINs.");
                return -1;
            }
            sensor_ptr = new Thermocouple(_adc, _highSideMux, _lowSideMux, pinout, type, index);
            sensors.emplace_back(sensor_ptr);
            return index;

        case STRAIN_GAUGE:
            if (nbr_pin_set < 4) {
                ESP_LOGE(TAG, "STRAIN_GAUGE requires 4 AINs.");
                return -1;
            }
            sensor_ptr = new StrainGauge(_adc, _highSideMux, _lowSideMux, pinout, index);
            sensors.emplace_back(sensor_ptr);
            return index;
        default:
            ESP_LOGE(TAG, "Unknown sensor type.");
            return -1;
    }
}

static void print_sensor_type(enum Sensor_Type_e type)
{
    const char *type_str = NULL;

    switch (type) {
    case RAW_SENSOR:
        type_str = "raw sensor";
        break;
    case RTD_PT100:
        type_str = "RTD PT100";
        break;
    case RTD_PT1000:
        type_str = "RTD PT1000";
        break;
    case THERMOCOUPLE_B:
        type_str = "thermocouple B";
        break;
    case THERMOCOUPLE_E:
        type_str = "thermocouple E";
        break;
    case THERMOCOUPLE_J:
        type_str = "thermocouple J";
        break;
    case THERMOCOUPLE_K:
        type_str = "thermocouple K";
        break;
    case THERMOCOUPLE_N:
        type_str = "thermocouple N";
        break;
    case THERMOCOUPLE_R:
        type_str = "thermocouple R";
        break;
    case THERMOCOUPLE_S:
        type_str = "thermocouple S";
        break;
    case THERMOCOUPLE_T:
        type_str = "thermocouple T";
        break;
    case STRAIN_GAUGE:
        type_str = "strain gauge";
        break;
    default:
        type_str = "unknown";
        break;
    }
    printf("%s", type_str);
}

static void print_sensor(Sensor *sensor)
{
    uint32_t index = sensor->get_index();
    enum Sensor_Type_e type = sensor->get_type();
    std::array<AIn_Num_t, 4> ains = sensor->get_ain_pins();
    printf("\t %lu - ", index);
    fflush(stdout);
    print_sensor_type(type);
    printf(", ains [");
    for (int i = 0; i < ains.size(); i++) {
        if (ains[i] == -1) {
            break;
        }
        if (i != 0) {
            printf(", ");
        }
        printf("%ld", ains[i]);
    }
    printf("]\n");
}

int AnalogInputsLS::listSensors(void)
{
    printf("There are %d sensors : \n", sensors.size());
    for (size_t i = 0; i < sensors.size(); i++) {
        print_sensor(sensors[i]);
    }
    return sensors.size();
}
