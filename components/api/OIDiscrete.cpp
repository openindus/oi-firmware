/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file OIDiscrete.cpp
 * @brief Functions for discrete module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "OIDiscrete.h"


#if defined CONFIG_OI_DISCRETE || CONFIG_OI_DISCRETE_VERTICAL

static const char OI_DISCRETE_TAG[] = "OIDiscrete";
xQueueHandle OIDiscrete::_discrete_evt_queue;
esp_adc_cal_characteristics_t * OIDiscrete::_adc_chars_ana;
esp_adc_cal_characteristics_t * OIDiscrete::_adc_chars_stor;

OIStorDescr_s OIDiscrete::_storConf[8] =    {
                                            {OIDISCRETE_PIN_STOR1, OIDISCRETE_CHANNEL_STOR_SENSOR1},
                                            {OIDISCRETE_PIN_STOR2, OIDISCRETE_CHANNEL_STOR_SENSOR2},
                                            {OIDISCRETE_PIN_STOR3, OIDISCRETE_CHANNEL_STOR_SENSOR3},
                                            {OIDISCRETE_PIN_STOR4, OIDISCRETE_CHANNEL_STOR_SENSOR4},
                                            {OIDISCRETE_PIN_STOR5, OIDISCRETE_CHANNEL_STOR_SENSOR5},
                                            {OIDISCRETE_PIN_STOR6, OIDISCRETE_CHANNEL_STOR_SENSOR6},
                                            {OIDISCRETE_PIN_STOR7, OIDISCRETE_CHANNEL_STOR_SENSOR7},
                                            {OIDISCRETE_PIN_STOR8, OIDISCRETE_CHANNEL_STOR_SENSOR8}
                                            };

OIEtorDescr_s OIDiscrete::_etorConf[10] =    {
                                            {OIDISCRETE_PIN_ETOR1},
                                            {OIDISCRETE_PIN_ETOR2},
                                            {OIDISCRETE_PIN_ETOR3},
                                            {OIDISCRETE_PIN_ETOR4},
                                            {OIDISCRETE_PIN_ETOR5},
                                            {OIDISCRETE_PIN_ETOR6},
                                            {OIDISCRETE_PIN_ETOR7},
                                            {OIDISCRETE_PIN_ETOR8},
                                            {OIDISCRETE_PIN_ETOR9},
                                            {OIDISCRETE_PIN_ETOR10}
                                            };


void OIDiscrete::init()
{
    #ifdef CONFIG_OI_DISCRETE
    _type = OI_DISCRETE;
    #else
    _type = OI_DISCRETEVE;
    #endif

    OIModuleMaster::init();
    
    _discrete_evt_queue = xQueueCreate(1, sizeof(uint32_t));
    xTaskCreate(_discrete_isr_task, "_discrete_isr_task", 2048, this, 10, NULL);

    /* Init of etor pins */
    ESP_LOGI(OI_DISCRETE_TAG, "initializing ETOR");
    _etor.setConf(_etorConf); 
    _etor.init();

    /* Init of stor pins */
    ESP_LOGI(OI_DISCRETE_TAG, "initializing STOR");
    _stor.setConf(_storConf); 
    _stor.init();

    /* Init of ana pins */
    ESP_LOGI(OI_DISCRETE_TAG, "initializing ANA");
    _anaToAdcChannel[EANA1] = (adc_channel_t)OIDISCRETE_CHANNEL_EANA1;
    adc1_config_width(ADC_WIDTH_BIT_13);
    adc1_config_channel_atten((adc1_channel_t)_anaToAdcChannel.at(EANA1), ADC_ATTEN_DB_11);
    _adc_chars_ana = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_13, ADC_DEFAULT_VREF, _adc_chars_ana);
    ESP_LOGV(OI_DISCRETE_TAG, "ADC for ANA is calibrated"); 

    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL<<OIDISCRETE_PIN_CMD_MOSFET);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    gpio_set_level(OIDISCRETE_PIN_CMD_MOSFET, LOW_LEVEL);

    /* Init pwm for analog write */
    ledc_timer_config_t ledc_timer; 
    ledc_timer.duty_resolution = LEDC_TIMER_8_BIT;  // resolution of PWM duty
    ledc_timer.freq_hz = 50;                      // frequency of PWM signal
    ledc_timer.speed_mode = LEDC_LOW_SPEED_MODE;           // timer mode
    ledc_timer.timer_num = LEDC_TIMER_1;           // timer index
    ledc_timer.clk_cfg = LEDC_AUTO_CLK;             // Auto select the source clock
    // Set configuration of timer0 for high speed channels
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
    gpio_install_isr_service(0);
}

void OIDiscrete::digitalWrite(Stor_t stor, uint8_t level) 
{
    _stor.digitalWrite(stor, level);
}

void OIDiscrete::analogWrite(Stor_t stor, uint8_t duty) 
{
    _stor.analogWrite(stor, duty);
}

int OIDiscrete::digitalRead(Etor_t etor)
{
    return _etor.digitalRead(etor);
}

uint32_t OIDiscrete::analogRead(Eana_t ana) 
{
    uint32_t adc_reading = 0;
    for (int i = 0; i < ADC_NB_SAMPLE; i++) 
    {
        adc_reading += adc1_get_raw((adc1_channel_t)_anaToAdcChannel.at(ana));
    }
    adc_reading /= ADC_NB_SAMPLE;
    
    return adc_reading;
}

float OIDiscrete::analogRead(Eana_t ana, Units_t units) 
{
    uint32_t adc_reading = analogRead(ana);
    if(units == UNITS_RAW)
    {
        return adc_reading;
    }

    else if(units == UNITS_MILLIVOLTS)
    {
        return esp_adc_cal_raw_to_voltage(adc_reading, _adc_chars_ana);
    }

    else
    {
        ESP_LOGE(OI_DISCRETE_TAG, "Milliamps unit is not supported on this board, please use raw or millivolts units");
        return -1;
    }
    
}

float OIDiscrete::getStorCurrent(Stor_t stor) 
{
    return _stor.getStorCurrent(stor);
}

void OIDiscrete::attachInterrupt(Etor_t etor, voidFuncPtrArg handler, InterruptMode_t mode) 
{
    _etor.attachInterrupt(etor, handler, mode);
}


void OIDiscrete::detachInterrupt(Etor_t etor, InterruptMode_t mode)
{
    _etor.detachInterrupt(etor, mode);
}

void IRAM_ATTR OIDiscrete::handleInterrupt(void* arg)
{
    uint32_t etor = (uint32_t) arg;
    xQueueSendFromISR(_discrete_evt_queue, &etor, NULL);
}

void OIDiscrete::_discrete_isr_task(void* arg)
{
    Etor_t etor;
    OIDiscrete * discrete = (OIDiscrete*) arg;
    InterruptMode_t mode; 
    
    for(;;) 
    {
        if(xQueueReceive(_discrete_evt_queue, &etor, portMAX_DELAY)) 
        {
                gpio_intr_disable(discrete->getGpioFromEtor(etor));
                ESP_LOGV(OI_DISCRETE_TAG, "An ISR occured\n");
                mode = (discrete->digitalRead(etor) == LOW_LEVEL ? FALLING_MODE : RISING_MODE );
                BusTWAI.sendMessage(OIMessage(CMD_ETOR_INTERRUPT, discrete->getId(), etor, mode), BROADCAST_ID);
                gpio_intr_enable(discrete->getGpioFromEtor(etor));
                
        }
    }
}

#endif /* CONFIG_OI_DISCRETE || CONFIG_OI_DISCRETE_VERTICAL */