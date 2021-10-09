/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file OICore.cpp
 * @brief Functions for core module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "OICore.h"

static const char OI_CORE_TAG[] = "OICore";

const ioex_num_t OICore::_stor[4] = { 
    OICORE_IOEX_PIN_STOR1,
    OICORE_IOEX_PIN_STOR2,
    OICORE_IOEX_PIN_STOR3,
    OICORE_IOEX_PIN_STOR4,
};

const ioex_num_t OICore::_storSensor[4] = { 
    OICORE_IOEX_PIN_STOR_SENSOR1,
    OICORE_IOEX_PIN_STOR_SENSOR2,
    OICORE_IOEX_PIN_STOR_SENSOR3,
    OICORE_IOEX_PIN_STOR_SENSOR4,
};

const ioex_num_t OICore::_etor[4] = { 
    OICORE_IOEX_PIN_ETOR1,
    OICORE_IOEX_PIN_ETOR2,
    OICORE_IOEX_PIN_ETOR3,
    OICORE_IOEX_PIN_ETOR4,
};

const adc1_channel_t OICore::_eana[2] = {
    OICORE_CHANNEL_EANA1,
    OICORE_CHANNEL_EANA2
};

ioex_device_t* OICore::_ioex;

void OICore::init()
{
    OIModuleMaster::init();

    _type = OI_CORE;

    /* Init GPIO service */
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

    /**
     * @brief I2C init
     * 
     */
    ESP_LOGI(OI_CORE_TAG, "Initializes the bus I2C (I2C_NUM_%u)", OICORE_I2C_PORT_NUM);
    ESP_LOGI(OI_CORE_TAG, "SDA: GPIO_NUM_%u | SCL: GPIO_NUM_%u",
        OICORE_PIN_I2C_SDA, OICORE_PIN_I2C_SCL);

    i2c_config_t i2c_config;
    i2c_config.mode = I2C_MODE_MASTER;
    i2c_config.sda_io_num = OICORE_PIN_I2C_SDA;
    i2c_config.sda_pullup_en = GPIO_PULLUP_ENABLE;
    i2c_config.scl_io_num = OICORE_PIN_I2C_SCL;
    i2c_config.scl_pullup_en = GPIO_PULLUP_ENABLE;
    i2c_config.master.clk_speed = OICORE_DEFAULT_I2C_SPEED;
    i2c_config.clk_flags = 0;

    ESP_ERROR_CHECK(i2c_param_config(OICORE_I2C_PORT_NUM, &i2c_config));
    ESP_ERROR_CHECK(i2c_driver_install(OICORE_I2C_PORT_NUM, i2c_config.mode, 0, 0, 0));

    /**
     * @brief SPI Init
     * 
     */
    ESP_LOGI(OI_CORE_TAG, "Initializes the bus SPI%u", OICORE_SPI_HOST+1);
    ESP_LOGI(OI_CORE_TAG, "MOSI:  GPIO_NUM_%u | MISO:  GPIO_NUM_%u | CLK:  GPIO_NUM_%u",
        OICORE_PIN_SPI_MOSI, OICORE_PIN_SPI_MISO, OICORE_PIN_SPI_SCK);

    spi_bus_config_t spi_config = {
        .mosi_io_num = OICORE_PIN_SPI_MOSI,
        .miso_io_num = OICORE_PIN_SPI_MISO,
        .sclk_io_num = OICORE_PIN_SPI_SCK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 0,
        .flags = 0,
        .intr_flags = 0
    };

    ESP_ERROR_CHECK(spi_bus_initialize(OICORE_SPI_HOST, &spi_config, 1));

    /**
     * @brief IO Expander init
     * 
     */
    _ioex = ioex_create(OICORE_I2C_PORT_NUM, OICORE_I2C_IOEXPANDER_ADDRESS, true, OICORE_PIN_ETOR_INTERRUPT);

    ESP_ERROR_CHECK(ioex_set_level(_ioex, OICORE_IOEX_PIN_CMD_MOSFET, IOEX_LOW));
    ESP_ERROR_CHECK(ioex_set_level(_ioex, OICORE_IOEX_PIN_ALIM_EXT, IOEX_HIGH));

    ioex_config_t config;
    config.mode = IOEX_OUTPUT;
    config.pull_mode = IOEX_FLOATING;
    config.interrupt_type = IOEX_INTERRUPT_DISABLE;
    config.pin_bit_mask = (1ULL<<OICORE_IOEX_PIN_CMD_MOSFET) | (1ULL<<OICORE_IOEX_PIN_ALIM_EXT);
    ESP_ERROR_CHECK(ioex_config(_ioex, &config));

    /**
     * @brief ETOR Init
     * 
     */
    ioex_config_t etor_config;
    etor_config.mode = IOEX_INPUT;
    etor_config.pull_mode = IOEX_PULLDOWN;
    etor_config.interrupt_type = IOEX_INTERRUPT_DISABLE;
    etor_config.pin_bit_mask = (1ULL<<_etor[0]) | 
                               (1ULL<<_etor[1]) | 
                               (1ULL<<_etor[2]) | 
                               (1ULL<<_etor[3]);
    
    ESP_ERROR_CHECK(ioex_config(_ioex, &etor_config));

    _etorCurrentMode[ETOR1] = NONE_MODE;
    _etorCurrentMode[ETOR2] = NONE_MODE;
    _etorCurrentMode[ETOR3] = NONE_MODE;
    _etorCurrentMode[ETOR4] = NONE_MODE;

    /**
     * @brief STOR Init
     * 
     */
    // /!\ Set level before setting to output
    ESP_ERROR_CHECK(ioex_set_level(_ioex, _stor[0], IOEX_LOW));
    ESP_ERROR_CHECK(ioex_set_level(_ioex, _stor[1], IOEX_LOW));
    ESP_ERROR_CHECK(ioex_set_level(_ioex, _stor[2], IOEX_LOW));
    ESP_ERROR_CHECK(ioex_set_level(_ioex, _stor[3], IOEX_LOW));
    
    ioex_config_t stor_config;
    stor_config.mode = IOEX_OUTPUT;
    stor_config.pull_mode = IOEX_FLOATING;
    stor_config.interrupt_type = IOEX_INTERRUPT_DISABLE;
    stor_config.pin_bit_mask = (1ULL<<_stor[0]) | 
                               (1ULL<<_stor[1]) | 
                               (1ULL<<_stor[2]) | 
                               (1ULL<<_stor[3]);
    
    ESP_ERROR_CHECK(ioex_config(_ioex, &stor_config));

    /**
     * @brief STOR Sensor init
     * 
     */
    ioex_config_t stor_sensor_config;        
    stor_sensor_config.mode = IOEX_INPUT;
    stor_sensor_config.pull_mode = IOEX_PULLDOWN;
    stor_sensor_config.interrupt_type = IOEX_INTERRUPT_POSEDGE;
    stor_sensor_config.pin_bit_mask = (1ULL<<_storSensor[0]) | 
                                      (1ULL<<_storSensor[0]) | 
                                      (1ULL<<_storSensor[0]) | 
                                      (1ULL<<_storSensor[0]);

    ESP_ERROR_CHECK(ioex_config(_ioex, &stor_sensor_config));

    /**
     * @brief EANA Init
     * 
     */
    #if CONFIG_IDF_TARGET_ESP32
    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_12));
    ESP_ERROR_CHECK(adc1_config_channel_atten(_eana[0], ADC_ATTEN_11db));
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_11db, ADC_WIDTH_BIT_12, 1100, &_adc1Characteristics);
    #endif

    /**
     * @brief CAN EXT init
     * 
     */
    ioex_config_t io_can_conf;

    io_can_conf.mode = IOEX_OUTPUT;
    io_can_conf.pull_mode = IOEX_FLOATING;
    io_can_conf.interrupt_type = IOEX_INTERRUPT_DISABLE;
    io_can_conf.pin_bit_mask = (1ULL<<OICORE_IOEX_PIN_CAN_RESET);
    ioex_config(_ioex, &io_can_conf);
    ioex_set_level(_ioex, OICORE_IOEX_PIN_CAN_RESET, IOEX_HIGH);
    vTaskDelay(10);
    ioex_set_level(_ioex, OICORE_IOEX_PIN_CAN_RESET, IOEX_LOW);
    vTaskDelay(10);
    ioex_set_level(_ioex, OICORE_IOEX_PIN_CAN_RESET, IOEX_HIGH);

    /**
     * @brief RTC Init
     * Configure IOExpander RTC Interrupt pin
     */
    ioex_config_t io_rtc_conf;        
    io_rtc_conf.mode = IOEX_INPUT;
    io_rtc_conf.pull_mode = IOEX_PULLUP;
    io_rtc_conf.interrupt_type = IOEX_INTERRUPT_NEGEDGE;
    io_rtc_conf.pin_bit_mask = (1ULL<<OICORE_IOEX_PIN_RTC_INT);

    ESP_ERROR_CHECK(ioex_config(_ioex, &io_rtc_conf));
}

void OICore::digitalWrite(Stor_t stor, uint8_t level) const
{
    ioex_set_level(_ioex, _stor[stor], static_cast<ioex_level_t>(level));
}

int OICore::digitalRead(Etor_t etor)
{
    return ioex_get_level(_ioex, _etor[etor]);
}

float OICore::analogRead(Eana_t ana) const
{
    int adc_reading = 0;

    for (int i = 0; i < OICORE_ADC_NO_OF_SAMPLES; i++)
    {
        adc_reading += adc1_get_raw(_eana[ana]);
    }

    adc_reading /= OICORE_ADC_NO_OF_SAMPLES;

    // Convert adc_reading to voltage in mV
    uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, &_adc1Characteristics);

    return static_cast<float>(voltage * OICORE_ADC_REDUCTION_FACTOR);
}

void OICore::attachInterrupt(Etor_t etor, void (*callback)(void *), void * args, InterruptMode_t mode)
{
    InterruptMode_t newMode = (InterruptMode_t)((int)_etorCurrentMode.at(etor) | (int)mode);

    if(newMode == mode)
    {
        ioex_set_interrupt_type(_ioex, _etor[etor], (ioex_interrupt_type_t)(mode));
    }
    
    else if(newMode == CHANGE_MODE)
    {
        ioex_set_interrupt_type(_ioex, _etor[etor], IOEX_INTERRUPT_ANYEDGE);
    }

     _etorCurrentMode[etor] = newMode;
    ioex_isr_handler_add(_ioex, _etor[etor], (ioex_isr_t)callback, args, OICORE_ETOR_INTERRUPT_PRIORITY);
    ioex_interrupt_enable(_ioex, _etor[etor]);
}

void OICore::detachInterrupt(Etor_t etor, InterruptMode_t mode)
{
    
    if(_etorCurrentMode.at(etor) == CHANGE_MODE)
    {
        if(mode == FALLING_MODE)
        {
            ioex_set_interrupt_type(_ioex, _etor[etor], IOEX_INTERRUPT_POSEDGE);
            _etorCurrentMode[etor] = RISING_MODE;
        }
        else if(mode == RISING_MODE)
        {
            ioex_set_interrupt_type(_ioex, _etor[etor], IOEX_INTERRUPT_NEGEDGE);
            _etorCurrentMode[etor] = FALLING_MODE;
        }
    }

    else if(_etorCurrentMode.at(etor) == mode)
    {
        ioex_set_interrupt_type(_ioex, _etor[etor], IOEX_INTERRUPT_DISABLE);
        ioex_isr_handler_remove(_ioex, _etor[etor]);
        _etorCurrentMode[etor] = NONE_MODE;
    }
}

time_t OICore::time(void)
{
    DateTime seconds = now();
    return seconds.unixtime();
}

DateTime OICore::now(void)
{
    M41T62_Time_st time;
    M41T62_Date_st date;
    M41T62_Getting_Time(&time);
    M41T62_Getting_Date(&date);
    DateTime now(date.Year, date.Month, date.DayMonth, time.Hours, time.Minutes, time.Seconds);

    return now;
}

void OICore::setTime(time_t time)
{
    setTime(DateTime(time));
}

void OICore::setTime(DateTime datetime)
{
    M41T62_Time_st time;
    M41T62_Date_st date;
    
    date.Century = 0; // 2000
    date.Year = datetime.year() - 2000;
    date.Month = datetime.month();
    date.DayMonth = datetime.day();
    date.DayWeek = datetime.dayOfWeek();
    time.Hours = datetime.hour();
    time.Minutes = datetime.minute();
    time.Seconds = datetime.second();
    time.Seconds_10Ths = 0;

    M41T62_Setting_Time(time);
    M41T62_Setting_Date(date);
}

void OICore::enableRTCAlarm(void)
{
    M41T62_Set_AFE_Bit(M41T62_AFE_HIGH);
    ioex_interrupt_enable(_ioex, OICORE_IOEX_PIN_RTC_INT);
}

void OICore::disableRTCAlarm(void)
{
    ioex_interrupt_disable(_ioex, OICORE_IOEX_PIN_RTC_INT);
    M41T62_Set_AFE_Bit(M41T62_AFE_LOW);
}

void OICore::setRTCAlarm(time_t alarm)
{
    // Set Alarm
    setRTCAlarm(DateTime(alarm));
}

void OICore::setRTCAlarm(DateTime alarm)
{
    M41T62_Alarm_st alarm_set;

    alarm_set.AlMonth = alarm.month();
    alarm_set.AlDate = alarm.day();
    alarm_set.AlHour = alarm.hour();
    alarm_set.AlMinutes = alarm.minute();
    alarm_set.AlSeconds = alarm.second();

    M41T62_Alarm_Setting(alarm_set);
}

void OICore::attachRTCAlarm(void (*callback)(void), void * args) const
{
    rtc_user_handler = (rtc_isr_t)callback;
    ioex_isr_handler_add(_ioex, OICORE_IOEX_PIN_RTC_INT, (ioex_isr_t) rtc_isr_handler, args, OICORE_RTC_INTERRUPT_PRIORITY);
}

void OICore::detachRTCAlarm(void) const
{
    ioex_isr_handler_remove(_ioex, OICORE_IOEX_PIN_RTC_INT);
}

void OICore::handleInterrupt(void* arg) 
{
    Etor_t* etor = (Etor_t*) arg;
    InterruptMode_t mode = (digitalRead(*etor) == LOW_LEVEL ? FALLING_MODE : RISING_MODE );

    BusTWAI.sendMessage(OIMessage(CMD_ETOR_INTERRUPT, OIModule::getId(), *etor, mode), BROADCAST_ID);
}