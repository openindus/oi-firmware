/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file OIMixed.cpp
 * @brief Functions for OIMixed module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "OIMixed.h"

#if defined CONFIG_OI_MIXED

static const char OI_MIXED_TAG[] = "OIMixed";
xQueueHandle OIMixed::_mixed_evt_queue;

esp_adc_cal_characteristics_t * OIMixed::_adc_chars_ana;

Dac8760_DeviceConfig_t OIMixed::_dac8760DeviceConfig;
Ads866x_DeviceConfig_t OIMixed::_ads866xDeviceConfig;

OIStorDescr_s OIMixed::_storConf[4] = {{OIMIXED_PIN_STOR1, OIMIXED_CHANNEL_STOR_SENSOR1},
                                       {OIMIXED_PIN_STOR2, OIMIXED_CHANNEL_STOR_SENSOR2},
                                       {OIMIXED_PIN_STOR3, OIMIXED_CHANNEL_STOR_SENSOR3},
                                       {OIMIXED_PIN_STOR4, OIMIXED_CHANNEL_STOR_SENSOR4}
                                      };

OIEtorDescr_s OIMixed::_etorConf[4] =  {{OIMIXED_PIN_ETOR1},
                                        {OIMIXED_PIN_ETOR2},
                                        {OIMIXED_PIN_ETOR3},
                                        {OIMIXED_PIN_ETOR4}
                                        };

static uint8_t AIN[4] = {2, 3, 1, 0}; // change the order of the analog inputs
                                        

OIMixed::OIMixed()
{
    OIModuleMaster();
}

void OIMixed::init()
{
    _type = OI_MIXED;

    _mixed_evt_queue = xQueueCreate(1, sizeof(uint32_t));
    xTaskCreate(_mixed_isr_task, "_mixed_isr_task", 2048, this, 10, NULL);

    OIModuleMaster::init();

    /* Init of etor pins */
    ESP_LOGI(OI_MIXED_TAG, "initializing ETOR");
    _etor.setConf(_etorConf);
    _etor.init();

    /* Init of stor pins */
    ESP_LOGI(OI_MIXED_TAG, "initializing STOR");
    _stor.setConf(_storConf);
    _stor.init();

    /**
     * @brief SPI Init
     */
    ESP_LOGI(OI_MIXED_TAG, "Initializes the bus SPI%u", OIMIXED_SPI_HOST+1);
    ESP_LOGI(OI_MIXED_TAG, "MOSI:  GPIO_NUM_%u | MISO:  GPIO_NUM_%u | CLK:  GPIO_NUM_%u",
        OIMIXED_PIN_SPI_MOSI, OIMIXED_PIN_SPI_MISO, OIMIXED_PIN_SPI_SCK);

    spi_bus_config_t spi_config = {
        .mosi_io_num = OIMIXED_PIN_SPI_MOSI,
        .miso_io_num = OIMIXED_PIN_SPI_MISO,
        .sclk_io_num = OIMIXED_PIN_SPI_SCK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 0,
        .flags = 0,
        .intr_flags = 0
    };

    ESP_ERROR_CHECK(spi_bus_initialize(OIMIXED_SPI_HOST, &spi_config, 1));

    /* Init of Eana pins */
    ESP_LOGI(OI_MIXED_TAG, "initializing EANA");
    _initEAna();

    /* Init of Sana pins */
    ESP_LOGI(OI_MIXED_TAG, "initializing SANA");
    _initSAna();

    /* Initializes CMD Mosfet Alim */
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL<<OIMIXED_PIN_CMD_MOSFET);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    gpio_set_level(OIMIXED_PIN_CMD_MOSFET, LOW_LEVEL);

    /* Init pwm for analog write */
    ledc_timer_config_t ledc_timer; 
    ledc_timer.duty_resolution = LEDC_TIMER_8_BIT;  // resolution of PWM duty
    ledc_timer.freq_hz = 50;                      // frequency of PWM signal
    ledc_timer.speed_mode = LEDC_LOW_SPEED_MODE;           // timer mode
    ledc_timer.timer_num = LEDC_TIMER_1;           // timer index
    ledc_timer.clk_cfg = LEDC_AUTO_CLK;             // Auto select the source clock
    // Set configuration of timer0 for high speed channels
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL3);
}

void OIMixed::digitalWrite(Stor_t stor, uint8_t level) 
{
    _stor.digitalWrite(stor, level);
}

void OIMixed::analogWrite(Stor_t stor, uint8_t duty) 
{
    _stor.analogWrite(stor, duty);
}

int OIMixed::digitalRead(Etor_t etor)
{
    return _etor.digitalRead(etor);
}

float OIMixed::getStorCurrent(Stor_t stor) 
{
    return _stor.getStorCurrent(stor);
}

uint32_t OIMixed::analogRead(Eana_t ana)
{
    return Ads866x_AnalogRead(AIN[ana]);
}

float OIMixed::analogRead(Eana_t ana, Units_t units)
{
    return Ads866x_AnalogReadUnits(AIN[ana], (Ads866x_Units_t)units);
}

void OIMixed::analogReadMode(Eana_t ana, AdcMode_t mode)
{
    Ads866x_setAdcMode(AIN[ana], (Ads866x_AdcMode_t) mode);
}

void OIMixed::analogReadResolution(AdcRes_t res)
{
    Ads866x_setAdcResolution((Ads866x_AdcResolutionBits_t) res);
}

float OIMixed::getAnalogReference(Units_t units)
{
    float analogRef = 0.0;

    if (units == UNITS_MILLIVOLTS)
    {
        analogRef = Ads866x_getAnalogReference();
    }
    else
    {
        ESP_LOGE(OI_MIXED_TAG, "Invalid units specified !");
    }
    return analogRef;
}

void OIMixed::setAnalogMode(Sana_t sana, DACAnalogMode_t mode)
{
    Dac8760_setVoltageMode(sana, (Dac8760_VoltageMode_t)mode);
}

DACAnalogMode_t OIMixed::getAnalogMode(Sana_t sana)
{
    return (DACAnalogMode_t)Dac8760_getVoltageMode(sana);
}

void OIMixed::voltageWrite(Sana_t sana, uint32_t value)
{
    Dac8760_VoltageWrite(sana, (float)value, DAC8760_UNITS_RAW);
}

void OIMixed::voltageWrite(Sana_t sana, float value, Units_t units)
{
    Dac8760_VoltageWrite(sana, value, (Dac8760_Units_t)units);
}

void OIMixed::setCurrentMode(Sana_t sana, DACCurrentMode_t mode)
{
    Dac8760_setCurrentMode(sana, (Dac8760_CurrentMode_t) mode);
}

DACCurrentMode_t OIMixed::getCurrentMode(Sana_t sana)
{
    return (DACCurrentMode_t)Dac8760_getCurrentMode(sana);
}   

void OIMixed::currentWrite(Sana_t sana, uint32_t value)
{
    Dac8760_CurrentWrite(sana, (float)value, DAC8760_UNITS_RAW);
}

void OIMixed::currentWrite(Sana_t sana, float value, Units_t units)
{
    Dac8760_CurrentWrite(sana, value, (Dac8760_Units_t)units);
}

void OIMixed::attachInterrupt(Etor_t etor, voidFuncPtrArg handler, InterruptMode_t mode) 
{
    _etor.attachInterrupt(etor, handler, mode);
}


void OIMixed::detachInterrupt(Etor_t etor, InterruptMode_t mode)
{
    _etor.detachInterrupt(etor, mode);
}

/************************************************************************************************************/
/*----------------------------------------- PRIVATE FUCNTIONS ----------------------------------------------*/
/************************************************************************************************************/

void OIMixed::_initEAna(void)
{
    _ads866xDeviceConfig = Ads866x_DeviceConfig_t {
        .spi_host = OIMIXED_SPI_HOST,
        .spi_freq = OIMIXED_SPI_FREQ,
        .spi_pin_cs = OIMIXED_ADC_PIN_CS,
        .pin_rst = OIMIXED_ADC_PIN_RST,
        .pin_mode = {OIMIXED_ADC_PIN_MODE_EANA1, 
                     OIMIXED_ADC_PIN_MODE_EANA2,
                     OIMIXED_ADC_PIN_MODE_EANA3,
                     OIMIXED_ADC_PIN_MODE_EANA4},
        .adc_analogs_nb = OIMIXED_ADC_NB,
        .adc_res = OIMIXED_ADC_DEFAULT_RES_BITS,
        .adc_mode = {ADS866X_VOLTAGE_MODE,
                     ADS866X_VOLTAGE_MODE,
                     ADS866X_VOLTAGE_MODE,
                     ADS866X_VOLTAGE_MODE}
    };

    Ads866x_DeviceConfig(&_ads866xDeviceConfig);
    Ads866x_Init();
}

void OIMixed::_initSAna(void)
{
    _dac8760DeviceConfig = Dac8760_DeviceConfig_t{
        .spi_host = OIMIXED_SPI_HOST,        
        .spi_freq = OIMIXED_SPI_FREQ,
        .spi_pin_cs = {OIMIXED_DAC_PIN_CS1, OIMIXED_DAC_PIN_CS2},        
        .nb_devices = OIMIXED_DAC_NB,
        .conf = {}
    };
    Dac8760_DeviceConfig(&_dac8760DeviceConfig);

    Dac8760_Init();
}

void IRAM_ATTR OIMixed::handleInterrupt(void* arg)
{
    uint32_t etor = (uint32_t) arg;
    xQueueSendFromISR(_mixed_evt_queue, &etor, NULL);
}

void OIMixed::_mixed_isr_task(void* arg)
{
    Etor_t etor;
    OIMixed * mixed = (OIMixed*) arg;
    InterruptMode_t mode; 
    
    for(;;) 
    {
        if(xQueueReceive(_mixed_evt_queue, &etor, portMAX_DELAY)) 
        {
                gpio_intr_disable(mixed->getGpioFromEtor(etor));
                ESP_LOGV(OI_MIXED_TAG, "An ISR occured\n");
                mode = (mixed->digitalRead(etor) == LOW_LEVEL ? FALLING_MODE : RISING_MODE );
                BusTWAI.sendMessage(OIMessage(CMD_ATTACH_INTERRUPT, mixed->getId(), etor, mode), BROADCAST_ID);
                gpio_intr_enable(mixed->getGpioFromEtor(etor));
        }
    }
}

#endif /* CONFIG_OI_MIXED */