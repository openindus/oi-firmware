/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file MotorBLDC.c
 * @brief 
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "MotorBLDC.h"

static const char* TAG = "MotorBLDC";

SemaphoreHandle_t MotorBLDC::_xSemaphore = NULL;
static const TickType_t xDelay100ms = pdMS_TO_TICKS(100);
float MotorBLDC::_speed;
static gpio_num_t _brake, _direction, _enable_chip, _speed_ctrl, _nfault, _fgout;
QueueHandle_t MotorBLDC::_PcntEvtQueue1;
pcnt_unit_t MotorBLDC::_PcntUnit1 = PCNT_UNIT_1 ;
MotorBLDC::pcnt_evt_t MotorBLDC::evt1;

int MotorBLDC::init(gpio_num_t brake, gpio_num_t direction, gpio_num_t enable_chip, gpio_num_t speed_ctrl, gpio_num_t nfault, gpio_num_t fgout)
{
    int err = 0;

    int8_t nbStepCommutationP = 6;
    int8_t nbStepCommutationN = -6;
    _brake = brake;
    _direction = direction;
    _enable_chip = enable_chip;
    _speed_ctrl = speed_ctrl;
    _nfault = nfault;
    _fgout = fgout;

    err |= gpio_set_direction(_brake, GPIO_MODE_OUTPUT);   
    err |= gpio_set_direction(_direction, GPIO_MODE_OUTPUT);
    err |= gpio_set_direction(_enable_chip, GPIO_MODE_OUTPUT);
    err |= gpio_set_direction(_speed_ctrl, GPIO_MODE_OUTPUT);
    err |= gpio_set_direction(_nfault, GPIO_MODE_INPUT);
    err |= gpio_set_direction(_fgout, GPIO_MODE_INPUT);

    _ledcInit();
    vTaskDelay(10 / portTICK_PERIOD_MS);

    /* Prepare configuration for the PCNT unit */
    pcnt_config_t pcnt_config1 = {
        .pulse_gpio_num = _fgout,
        .ctrl_gpio_num = GPIO_NUM_0,
        .lctrl_mode = PCNT_MODE_DISABLE, 
        .hctrl_mode = PCNT_MODE_KEEP,    
        .pos_mode = PCNT_COUNT_INC,   
        .neg_mode = PCNT_COUNT_DIS,   
        .counter_h_lim = nbStepCommutationP,
        .counter_l_lim = nbStepCommutationN,
        .unit = _PcntUnit1,
        .channel = PCNT_CHANNEL_1,
    };

    /* Initialize PCNT unit */
    err |= pcnt_unit_config(&pcnt_config1);

    /* Configure and enable the input filter */
    err |= pcnt_set_filter_value(_PcntUnit1, 100);
    err |= pcnt_filter_enable(_PcntUnit1);

    /* Enable events on zero, maximum and minimum limit values */
    err |= pcnt_event_enable(_PcntUnit1, PCNT_EVT_H_LIM);
    err |= pcnt_event_enable(_PcntUnit1, PCNT_EVT_L_LIM);

    /* Initialize PCNT's counter */
    err |= pcnt_counter_pause(_PcntUnit1);
    err |= pcnt_counter_clear(_PcntUnit1);
    
    /* Install interrupt service and add isr callback handler */
    err |= pcnt_isr_service_install(0);
    err |= pcnt_isr_handler_add(_PcntUnit1, _pcntIntrHandler1, (void *)_PcntUnit1);

    /* Everything is set up, now go to counting */
    err |= pcnt_counter_resume(_PcntUnit1);

    _PcntEvtQueue1 = xQueueCreate(10, sizeof(pcnt_evt_t));

    //Set-up tache pour calcul de la vitesse
    xTaskCreate(&_vTaskFunctionSpeed, "vTaskFunctionSpeed", 4096, NULL , 2, NULL);
    vTaskDelay(10 / portTICK_PERIOD_MS);

    err |= gpio_set_level(_enable_chip, 1);
    uint32_t fault = gpio_get_level(_nfault);
    if (fault == 1){
        ESP_LOGE(TAG, "Error during initialisation");
    }
    else{
        ESP_LOGI(TAG, "Set-up done, Chip enable");
    }
    
    return err;
}

void MotorBLDC::setSpeed(uint32_t duty_cycle)
{
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL,duty_cycle)); // the duty cycle need to be set between 0 (0%) and 510(100%) (because of PWM declaration)
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
}

float MotorBLDC::getSpeed()
{
    printf("Speed : %f \n", _speed);
    return _speed;
}

void MotorBLDC::setBrake(bool brake)
{
    if (brake == true) {
        gpio_set_level(_brake, 0); //frein activé quand bool = 1
    } 
    else {    
        gpio_set_level(_brake, 1); //frein désactivé quand bool = 0
    }
}

void MotorBLDC::setDirection(bool direction)
{
    if (direction == 1){
        gpio_set_level(_direction, 1); //sens horaire = 1
    } 
    else {
        gpio_set_level(_direction, 0); //sens anti-horaire = 0
    }
}

void IRAM_ATTR MotorBLDC::_pcntIntrHandler1(void *arg)
{
    evt1.unit = _PcntUnit1;
    pcnt_get_event_status(_PcntUnit1, &evt1.status);
    xQueueSendFromISR(_PcntEvtQueue1, &evt1, NULL);
}

void MotorBLDC::_vTaskFunctionSpeed(void *pvParameters)
{
    portBASE_TYPE res;
    int64_t Time = 0;
    int64_t TimeMs = 0;
    int64_t PreviousTime = 0;
    int revolution = 0;
    int16_t count = 0;
    int8_t nbStepCommutationP = 6;
    while(1) 
    {
        res = xQueueReceive(_PcntEvtQueue1, &evt1, 10/portTICK_PERIOD_MS);
        if (res == pdTRUE){
            pcnt_get_counter_value(_PcntUnit1, &count);
            vTaskDelay(1 / portTICK_PERIOD_MS);
            if (evt1.status & PCNT_EVT_H_LIM){
                revolution ++;
            } 
            else if (evt1.status & PCNT_EVT_L_LIM){
                revolution ++;
            }
            else{};
        } 
        else{
            ESP_ERROR_CHECK(pcnt_get_counter_value(_PcntUnit1, &count));
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }
        printf("Speed : %f \n", _speed);
        printf("Revolution : %d \n", revolution);
        printf("Count : %d \n", count);
        Time = esp_timer_get_time();
        TimeMs = Time/1000; 
        if (TimeMs - PreviousTime > 1000){
            PreviousTime = TimeMs;
            _speed = (revolution + (count / nbStepCommutationP)) * 60;
            revolution = 0;
            count = 0;
        }
        else{
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }
    }
} 

void MotorBLDC::_ledcInit()
{
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES,
        .timer_num        = LEDC_TIMER,
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 45000 kHz
        .clk_cfg          = LEDC_USE_APB_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .gpio_num       = _speed_ctrl,
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .intr_type      = LEDC_INTR_DISABLE,
        .timer_sel      = LEDC_TIMER,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0,
        .flags = {.output_invert = 0} 
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    vTaskDelay(10 / portTICK_PERIOD_MS);
}
