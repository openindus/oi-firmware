/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file OIEtor.h
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

typedef void (*voidFuncPtr)(void);
typedef void (*voidFuncPtrArg)(void*);

struct OIEtorDescr_s
{
    gpio_num_t gpio;
};

template<uint8_t N>
struct OIEtor
{
    uint8_t nb() { return N; }
    OIEtorDescr_s* conf;
    InterruptMode_t intr_mode[N];

    static constexpr const char* OI_ETOR_TAG = "OIETOR";

    OIEtor()
    {
        conf = NULL;
    }

    void setConf(OIEtorDescr_s* _conf)
    {
        conf = _conf;
    }

    void init(void)
    {
        uint8_t i = 0;

        gpio_config_t io_conf;
        io_conf.intr_type = GPIO_INTR_DISABLE;
        io_conf.mode = GPIO_MODE_INPUT;
        io_conf.pin_bit_mask = 0;

        for(i = 0; i < nb(); i++)
        {
            io_conf.pin_bit_mask |= (1ULL<<conf[i].gpio);
        }
        io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
        io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
        ESP_ERROR_CHECK(gpio_config(&io_conf));

        for(i= 0; i < nb(); i++)
        {
            intr_mode[i] = NONE_MODE;
        }
    } 

    int digitalRead(Etor_t etor) 
    {
        if (etor < nb())
        {
            return gpio_get_level(conf[etor].gpio);
        }

        else
        {
            ESP_LOGE(OI_ETOR_TAG, "Invalid ETOR%d", etor);
            return -1;
        }
    }

    void attachInterrupt(Etor_t etor, voidFuncPtrArg handler, InterruptMode_t mode) 
    {
        if (etor < nb())
        {
            InterruptMode_t newMode = (InterruptMode_t)(intr_mode[etor] | (int)mode);

            if(newMode == mode)
            {
                gpio_set_intr_type(conf[etor].gpio, (gpio_int_type_t)(mode));
            }
            
            else if(newMode == CHANGE_MODE)
            {
                gpio_set_intr_type(conf[etor].gpio, GPIO_INTR_ANYEDGE);
            }

            intr_mode[etor] = newMode;
            gpio_isr_handler_add(conf[etor].gpio, (gpio_isr_t) handler, (void *) etor);
            gpio_intr_enable(conf[etor].gpio);
        }

        else
        {
            ESP_LOGE(OI_ETOR_TAG, "Invalid ETOR%d", etor);
        }
    }


    void detachInterrupt(Etor_t etor, InterruptMode_t mode)
    {
    
        if (etor < nb())
        {
            if(intr_mode[etor] == CHANGE_MODE)
            {
                if(mode == FALLING_MODE)
                {
                    gpio_set_intr_type(conf[etor].gpio, GPIO_INTR_POSEDGE);
                    intr_mode[etor] = RISING_MODE;
                }
                else if(mode == RISING_MODE)
                {
                    gpio_set_intr_type(conf[etor].gpio, GPIO_INTR_NEGEDGE);
                    intr_mode[etor] = FALLING_MODE;
                }
            }

            else if(intr_mode[etor] == mode)
            {
                gpio_isr_handler_remove(conf[etor].gpio);
                gpio_set_intr_type(conf[etor].gpio, GPIO_INTR_DISABLE);
                intr_mode[etor] = NONE_MODE;
            }
        }

        else
        {
            ESP_LOGE(OI_ETOR_TAG, "Invalid ETOR%d", etor);
        }
    }

    gpio_num_t getGpioFromEtor(Etor_t etor)
    {
        if (etor < nb())
        {
            return conf[etor].gpio;
        }

        else
        {
            ESP_LOGE(OI_ETOR_TAG, "Invalid ETOR%d", etor);
            return GPIO_NUM_NC;
        }
    }
};