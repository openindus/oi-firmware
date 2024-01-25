#pragma once

#include "ads1x4s08.h"
#include "stds75.h"
#include "cd74hc4052.h"

typedef enum{
    VOLTMETER           = 1,
    RTD                 = 2,
    THERMOCOUPLE        = 3,
    LEVEL_DETECTOR      = 4,
    WHEATSTONE_BRIDGE   = 5
}sensor_type_t;

typedef struct
    {
        gpio_num_t start;
        gpio_num_t drdy;
        gpio_num_t reset;
        gpio_num_t miso;
        gpio_num_t mosi;
        gpio_num_t sclk;
        gpio_num_t cs;
    }pin_adc_t;

typedef struct
    {
        gpio_num_t sel0;
        gpio_num_t sel1;
    }pin_mux_t;

typedef struct
    {
        gpio_num_t sda;
        gpio_num_t scl;
        gpio_num_t os_int;
    }pin_temp_sensor_t;

typedef struct
    {
        pin_adc_t pin_adc;
        pin_mux_t pin_mux;
        pin_temp_sensor_t pin_temp_sensor;
    }pin_esp_t;

typedef float level_detector_param_t;

typedef enum{
        ANALOG_CHANNEL_A = 0,
        ANALOG_CHANNEL_B = 1,
        ANALOG_CHANNEL_C = 2,
        ANALOG_CHANNEL_D = 3,
    }analog_channel_t;

typedef enum{
        PT100 = 1,
        PT1000 = 10
    }rtd_type_t;

typedef enum{
        RTD_2WIRE = 0,
        RTD_3WIRE = 1
    }wire_t;