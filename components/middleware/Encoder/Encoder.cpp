/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file Encoder.cpp
 * @brief 
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "Encoder.h"

static const float _RpmToRadians = 0.10471975512;
static const float _RadToDeg = 57.29578;

QueueHandle_t Encoder::_PcntEvtQueue;

int Encoder::_position = 0;
float Encoder::_pos = 0;
bool Encoder::_direction = 0;
float Encoder::_speed = 0;
float Encoder::_pulsePerRevolution = 0;
Encoder::pcnt_evt_t Encoder::evt;
pcnt_unit_t Encoder::_PcntUnit = PCNT_UNIT_0;

void Encoder::init(gpio_num_t coderA, gpio_num_t coderB, int16_t ValueMaxPulsePerRevolution, int16_t ValueMinPulsePerRevolution)
{
    gpio_set_direction(coderA, GPIO_MODE_INPUT);   
    gpio_set_direction(coderB, GPIO_MODE_INPUT);

    _pulsePerRevolution = ValueMaxPulsePerRevolution;
    /* Prepare configuration for the PCNT unit */
    pcnt_config_t pcnt_config = {
        .pulse_gpio_num = coderA,
        .ctrl_gpio_num = coderB,
        .lctrl_mode = PCNT_MODE_REVERSE, 
        .hctrl_mode = PCNT_MODE_KEEP,    
        .pos_mode = PCNT_COUNT_INC,   
        .neg_mode = PCNT_COUNT_DIS,   
        .counter_h_lim = ValueMaxPulsePerRevolution,
        .counter_l_lim = ValueMinPulsePerRevolution,
        .unit = _PcntUnit,
        .channel = PCNT_CHANNEL_0,
    };

    /* Initialize PCNT unit */
    pcnt_unit_config(&pcnt_config);

    /* Configure and enable the input filter */
    pcnt_set_filter_value(_PcntUnit, 100);
    pcnt_filter_enable(_PcntUnit);

    /* Enable events on zero, maximum and minimum limit values */
    pcnt_event_enable(_PcntUnit, PCNT_EVT_H_LIM);
    pcnt_event_enable(_PcntUnit, PCNT_EVT_L_LIM);

    /* Initialize PCNT's counter */
    pcnt_counter_pause(_PcntUnit);
    pcnt_counter_clear(_PcntUnit);

    /* Install interrupt service and add isr callback handler */
    //pcnt_isr_service_install(0);
    pcnt_isr_handler_add(_PcntUnit, _pcntIntrHandler, (void *)_PcntUnit);

    /* Everything is set up, now go to counting */
    pcnt_counter_resume(_PcntUnit);

    _PcntEvtQueue = xQueueCreate(10, sizeof(pcnt_evt_t));

    //Set-up getRevolution and getDirection task
    xTaskCreate(&_vTaskFunctionRevolution, "vTaskFunctionRevolution", 4096, NULL, 2, NULL);
}

bool Encoder::getDirection()
{
    // Clockwise revolution (=1) or anti-clockwise revolution (=0)
    return _direction;
}

float Encoder::getSpeed()
{
    // Motor speed in revolution/min
    return _speed;
}

float Encoder::getPosition()
{
    // motor position in radians
    return _pos;    
}

void IRAM_ATTR Encoder::_pcntIntrHandler(void *arg)
{
    pcnt_evt_t evt;
    evt.unit = _PcntUnit;
    pcnt_get_event_status(_PcntUnit, &evt.status);
    xQueueSendFromISR(_PcntEvtQueue, &evt, NULL);
}

void Encoder::_vTaskFunctionRevolution(void *pvParameters) 
{
    portBASE_TYPE res;
    int64_t Time = 0;
    int64_t TimeMs = 0;
    int64_t PreviousTime = 0;
    int revolution = 0;
    int16_t count = 0;
    float deg;
    float result;
    float abs_count = 0;

    while(1) 
    {
        res = xQueueReceive(_PcntEvtQueue, &evt, 10/portTICK_PERIOD_MS);
        if (res == pdTRUE){
            pcnt_get_counter_value(_PcntUnit, &count);
            vTaskDelay(1 / portTICK_PERIOD_MS);
            if (evt.status & PCNT_EVT_H_LIM){
                revolution ++;
                _position ++;
            } 
            else if (evt.status & PCNT_EVT_L_LIM){
                revolution ++;
                _position --;
            }
            else{};
        } 
        else{
            ESP_ERROR_CHECK(pcnt_get_counter_value(_PcntUnit, &count));
            vTaskDelay(1 / portTICK_PERIOD_MS);
            if (count > 0){
                _direction = 1;
                vTaskDelay(1 / portTICK_PERIOD_MS);
            }
            else if (count < 0){
                _direction = 0;
                vTaskDelay(1 / portTICK_PERIOD_MS);
            }
            else{};
        }
        deg = 0.36; //  360 / 1000 (360 degres divided by 1000 pulses)
        result = count * deg;
        _pos = (_position * 6.28319) + (result * 0.01745);
        Time = esp_timer_get_time();
        TimeMs = Time/1000; 
        if (TimeMs - PreviousTime > 1000){
            PreviousTime = TimeMs;
            abs_count = abs(count);
            _speed = (revolution + abs_count / _pulsePerRevolution) * 60;
            revolution = 0;
            abs_count = 0;
        }
        else{}; 
    } 
}