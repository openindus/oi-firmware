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
pcnt_unit_handle_t MotorBLDC::_PcntUnit1 = nullptr;
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

    /* Create PCNT unit */
    pcnt_unit_config_t unit_config = {
        .low_limit = nbStepCommutationN,
        .high_limit = nbStepCommutationP,
        .intr_priority = 0,
        .flags = {
            .accum_count = 0,
        }
    };
    err |= pcnt_new_unit(&unit_config, &_PcntUnit1);

    /* Configure channel */
    pcnt_chan_config_t chan_config = {
        .edge_gpio_num = _fgout,
        .level_gpio_num = GPIO_NUM_0,
        .flags = {
            .invert_edge_input = 0,
            .invert_level_input = 0,
            .virt_edge_io_level = 0,
            .virt_level_io_level = 0,
            .io_loop_back = 0
        },
    };
    pcnt_channel_handle_t pcnt_channel = NULL;
    err |= pcnt_new_channel(_PcntUnit1, &chan_config, &pcnt_channel);

    /* Set edge and level actions for the channel */
    err |= pcnt_channel_set_edge_action(pcnt_channel, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD);
    err |= pcnt_channel_set_level_action(pcnt_channel, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_KEEP);

    /* Add watch points and register callbacks */
    err |= pcnt_unit_add_watch_point(_PcntUnit1, nbStepCommutationP);
    err |= pcnt_unit_add_watch_point(_PcntUnit1, nbStepCommutationN);

    /* Register event callbacks */
    pcnt_event_callbacks_t cbs = {
        .on_reach = _pcntIntrHandler1,
    };
    err |= pcnt_unit_register_event_callbacks(_PcntUnit1, &cbs, nullptr);

    /* Configure and enable the input filter */
    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = 100000,  // 100us
    };
    err |= pcnt_unit_set_glitch_filter(_PcntUnit1, &filter_config);

    /* Clear counter and enable unit */
    err |= pcnt_unit_clear_count(_PcntUnit1);
    err |= pcnt_unit_enable(_PcntUnit1);
    err |= pcnt_unit_start(_PcntUnit1);

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

bool IRAM_ATTR MotorBLDC::_pcntIntrHandler1(pcnt_unit_handle_t unit, const pcnt_watch_event_data_t *edata, void *user_ctx)
{
    evt1.unit = 1; // Unit number for identification
    evt1.watch_point_value = edata->watch_point_value;
    BaseType_t high_task_wakeup;
    xQueueSendFromISR(_PcntEvtQueue1, &evt1, &high_task_wakeup);
    return (high_task_wakeup == pdTRUE);
}

void MotorBLDC::_vTaskFunctionSpeed(void *pvParameters)
{
    portBASE_TYPE res;
    int64_t Time = 0;
    int64_t TimeMs = 0;
    int64_t PreviousTime = 0;
    int revolution = 0;
    int count = 0;
    int8_t nbStepCommutationP = 6;
    while(1) 
    {
        res = xQueueReceive(_PcntEvtQueue1, &evt1, 10/portTICK_PERIOD_MS);
        if (res == pdTRUE){
            pcnt_unit_get_count(_PcntUnit1, &count);
            vTaskDelay(1 / portTICK_PERIOD_MS);
            if (evt1.watch_point_value == nbStepCommutationP){
                revolution ++;
            } 
            else if (evt1.watch_point_value == -nbStepCommutationP){
                revolution ++;
            }
            else{};
        } 
        else{
            pcnt_unit_get_count(_PcntUnit1, &count);
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

void MotorBLDC::_ledcInit(void)
{
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES,
        .timer_num        = LEDC_TIMER,
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 45000 kHz
        .clk_cfg          = LEDC_USE_APB_CLK,
        .deconfigure      = false // Do not deconfigure the timer
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
        .sleep_mode     = LEDC_SLEEP_MODE_NO_ALIVE_NO_PD,
        .flags = {
            .output_invert = 0
    } 
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    vTaskDelay(10 / portTICK_PERIOD_MS);
}
