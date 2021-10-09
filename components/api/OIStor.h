/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file OIStor.h
 * @brief 
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include <stdint.h>
#include "esp_adc_cal.h"
#include "driver/ledc.h"
#include "driver/gpio.h"

#include "OIType.h"

struct OIStorDescr_s
{
    gpio_num_t gpio;
    adc_channel_t adcSenseChannel;
};

template<uint8_t N>
struct OIStor
{
    uint8_t nb() { return N; }
    OIStorDescr_s* conf;
    StorMode_t mode[N];
    ledc_channel_config_t pwmConf[N];
    bool currentSenseEnable;
    esp_adc_cal_characteristics_t adcChars;
    adc_bits_width_t bitsWidth;
    static constexpr const char* OI_STOR_TAG = "OISTOR";

    OIStor()
    {
        conf = NULL;
        currentSenseEnable = true;

    #if !CONFIG_IDF_TARGET_ESP32
        bitsWidth = ADC_WIDTH_BIT_13; // Not supported on ESP32
    #else
        bitsWidth = ADC_WIDTH_BIT_12;
    #endif

    }

    void setConf(OIStorDescr_s* _conf)
    {
        conf = _conf;
    }

    void init(void)
    {
        uint8_t i = 0;

        gpio_config_t io_conf;
        io_conf.intr_type = GPIO_INTR_DISABLE;
        io_conf.mode = GPIO_MODE_INPUT_OUTPUT;
        io_conf.pin_bit_mask = 0;

        for(i = 0; i < nb(); i++)
        {
            io_conf.pin_bit_mask |= (1ULL<<conf[i].gpio);
        }
        io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
        io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
        ESP_ERROR_CHECK(gpio_config(&io_conf));

        for(i= 0; i < nb(); i++)
        {
            mode[i] = STOR;
            pwmConf[i] = (ledc_channel_config_t){.gpio_num = conf[i].gpio, .speed_mode = LEDC_LOW_SPEED_MODE, .channel = (ledc_channel_t)(LEDC_CHANNEL_0 + i), .intr_type = LEDC_INTR_DISABLE, .timer_sel = LEDC_TIMER_1, .duty = 0, .hpoint = 0};
            gpio_set_level(conf[i].gpio, LOW_LEVEL);
        }

        if (currentSenseEnable)
        {
            esp_adc_cal_characterize(ADC_UNIT_2, ADC_ATTEN_DB_11, bitsWidth, ADC_DEFAULT_VREF, &adcChars);

            for(i = 0; i < nb(); i++)
            {
                adc2_config_channel_atten((adc2_channel_t)conf[i].adcSenseChannel, ADC_ATTEN_DB_11);
            }
        }
    } 

    void digitalWrite(Stor_t stor, uint8_t level) 
    {
        if (stor < nb())
        {
            if(mode[stor] != STOR)
            {
                gpio_config_t io_conf;
                const ledc_channel_config_t * stor_pwm_conf = (const ledc_channel_config_t *) &(pwmConf[stor]);
                ledc_stop(stor_pwm_conf->speed_mode, stor_pwm_conf->channel, 0);

                io_conf.intr_type = GPIO_INTR_DISABLE;
                io_conf.mode = GPIO_MODE_INPUT_OUTPUT;
                io_conf.pin_bit_mask = (1ULL<<conf[stor].gpio);
                io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
                io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
                ESP_ERROR_CHECK(gpio_config(&io_conf));

                mode[stor] = STOR;
            }
            gpio_set_level(conf[stor].gpio, level);
        }
        else
        {
            ESP_LOGE(OI_STOR_TAG, "Invalid STOR%d", stor);
        }
    }

    void toggleOutput(Stor_t stor)
    {
        uint32_t level;
        gpio_config_t io_conf;

        if (stor < nb())
        {
            if(mode[stor] != STOR)
            {
                
                const ledc_channel_config_t * stor_pwm_conf = (const ledc_channel_config_t *) &(pwmConf[stor]);
                ledc_stop(stor_pwm_conf->speed_mode, stor_pwm_conf->channel, 0);
                io_conf.intr_type = GPIO_INTR_DISABLE;
                io_conf.mode = GPIO_MODE_INPUT_OUTPUT;
                io_conf.pin_bit_mask = (1ULL<<conf[stor].gpio);
                io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
                io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
                ESP_ERROR_CHECK(gpio_config(&io_conf));

                mode[stor] = STOR;
            }
        
            level = (gpio_get_level(conf[stor].gpio) == 1 ? 0 : 1);
            gpio_set_level(conf[stor].gpio, level);
        }
        else
        {
            ESP_LOGE(OI_STOR_TAG, "Invalid STOR%d", stor);
        }
    }

    float getStorCurrent(Stor_t stor) 
    {
        float retVal = 0.0;

        if (stor < nb())
        {
            uint32_t adc_reading = 0;
            for (int i = 0; i < ADC_NB_SAMPLE; i++) 
            {
                int raw;
                adc2_get_raw((adc2_channel_t)conf[stor].adcSenseChannel, bitsWidth, &raw);
                adc_reading += raw;
            }
            adc_reading /= ADC_NB_SAMPLE;
            
            float getValue = esp_adc_cal_raw_to_voltage(adc_reading, &adcChars);
            retVal = (getValue*STOR_SENSOR_COEFF_K)/(VSENSE_RESISTOR);
        }
        else
        {
            ESP_LOGE(OI_STOR_TAG, "Invalid STOR%d", stor);
        }
        return retVal;
    }

    void analogWrite(Stor_t stor, uint8_t duty) 
    {
        if (stor < nb())
        {
            const ledc_channel_config_t * stor_pwm_conf = (const ledc_channel_config_t *) &(pwmConf[stor]);

            if(mode[stor] != PWM)
            {
                ledc_channel_config(stor_pwm_conf);
                mode[stor] = PWM;
            }

            ledc_set_duty(stor_pwm_conf->speed_mode, stor_pwm_conf->channel, duty);
            ledc_update_duty(stor_pwm_conf->speed_mode, stor_pwm_conf->channel);
        }
        else
        {
            ESP_LOGE(OI_STOR_TAG, "Invalid STOR%d", stor);   
        }
    }
};