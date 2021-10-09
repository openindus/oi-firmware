/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file OICore.h
 * @brief Functions for core module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

/* General includes */
#include <map>
#include <stdint.h>

/* ESP-IDF includes */
#include "driver/i2c.h"
#include "driver/spi_master.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "soc/adc_channel.h"

#include "esp_vfs_fat.h"
#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#include "sdmmc_cmd.h"

#ifdef CONFIG_IDF_TARGET_ESP32
#include "driver/sdmmc_host.h"
#endif

/* Drivers includes */
#include "pcal6524/pcal6524.h"
#include "m41t62/m41t62.h"
#include "m41t62/STM32DriversComponents/M41T62.h"
#include "sc16is750/sc16is750.h"
#include "mcp25625/mcp25625.h"
#include "mcp25625/DriversComponents/MCP25625.h"

/* Component includes */
#include "OIBus.h"
#include "OIModule.h"
#include "OIType.h"

/* I2C pins */
#define OICORE_PIN_I2C_SDA              GPIO_NUM_32
#define OICORE_PIN_I2C_SCL              GPIO_NUM_33

/* SPI pins */
#define OICORE_PIN_SPI_MISO             GPIO_NUM_12
#define OICORE_PIN_SPI_MOSI             GPIO_NUM_13
#define OICORE_PIN_SPI_SCK              GPIO_NUM_14

/* RS EXT pins */
#define OICORE_PIN_RS_INTERRUPT         GPIO_NUM_34
#define OICORE_PIN_RS_SPI_CS            GPIO_NUM_19
#define OICORE_IOEX_PIN_RS_RESET        IOEX_NUM_21

/* CAN EXT pins */
#define OICORE_PIN_CAN_INTERRUPT        GPIO_NUM_23
#define OICORE_PIN_CAN_SPI_CS           GPIO_NUM_27
#define OICORE_IOEX_PIN_CAN_RESET       IOEX_NUM_20

/* USB Host pins */
#define OICORE_PIN_USB_HOST_SPI_CS      GPIO_NUM_18
#define OICORE_IOEX_PIN_USB_HOST_RESET  IOEX_NUM_23

/* Ethernet pins */
#define OICORE_PIN_ETHERNET_SPI_CS      GPIO_NUM_25
#define OICORE_IOEX_PIN_ETHERNET_RESET  IOEX_NUM_22

/* USB Host pins */
#define OICORE_PIN_SD_SPI_CS            GPIO_NUM_26

/* RTC pins */
#define OICORE_PIN_RTC_INTERRUPT        IOEX_NUM_25

/* Module ext pins */
#define OICORE_PIN_MODULE_EXT_IO15      GPIO_NUM_15
#define OICORE_PIN_MODULE_EXT_IO2       GPIO_NUM_12

/* ETOR Interrupt */
#if CONFIG_IDF_TARGET_ESP32
#define OICORE_PIN_ETOR_INTERRUPT       GPIO_NUM_22
#else // Dummy value if esp32s2
#define OICORE_PIN_ETOR_INTERRUPT       (gpio_num_t) 0
#endif

/* ANA */
#if CONFIG_IDF_TARGET_ESP32
#define OICORE_CHANNEL_EANA1            ADC1_GPIO36_CHANNEL
#define OICORE_CHANNEL_EANA2            ADC1_GPIO39_CHANNEL
#else // Dummy value if esp32s2
#define OICORE_CHANNEL_EANA1            (adc1_channel_t) 0
#define OICORE_CHANNEL_EANA2            (adc1_channel_t) 0
#endif

/* STOR */
#define OICORE_IOEX_PIN_STOR1           IOEX_NUM_0
#define OICORE_IOEX_PIN_STOR2           IOEX_NUM_1
#define OICORE_IOEX_PIN_STOR3           IOEX_NUM_2
#define OICORE_IOEX_PIN_STOR4           IOEX_NUM_3

/* STOR Sensor */
#define OICORE_IOEX_PIN_STOR_SENSOR1    IOEX_NUM_4
#define OICORE_IOEX_PIN_STOR_SENSOR2    IOEX_NUM_5
#define OICORE_IOEX_PIN_STOR_SENSOR3    IOEX_NUM_6
#define OICORE_IOEX_PIN_STOR_SENSOR4    IOEX_NUM_7

/* ETOR */
#define OICORE_IOEX_PIN_ETOR1           IOEX_NUM_10
#define OICORE_IOEX_PIN_ETOR2           IOEX_NUM_11
#define OICORE_IOEX_PIN_ETOR3           IOEX_NUM_12
#define OICORE_IOEX_PIN_ETOR4           IOEX_NUM_13

/* MODULE EXT */
#define OICORE_IOEX_PIN_MODULE_EXT_0    IOEX_NUM_14
#define OICORE_IOEX_PIN_MODULE_EXT_1    IOEX_NUM_15
#define OICORE_IOEX_PIN_MODULE_EXT_2    IOEX_NUM_16
#define OICORE_IOEX_PIN_MODULE_EXT_3    IOEX_NUM_17

/* RTC */
#define OICORE_IOEX_PIN_RTC_INT         IOEX_NUM_25
#define OICORE_RTC_INTERRUPT_PRIORITY   (10)

/* Command mosfet alim ext */
#define OICORE_IOEX_PIN_CMD_MOSFET      IOEX_NUM_24

/* Command mosfet alim ext */
#define OICORE_IOEX_PIN_ALIM_EXT        IOEX_NUM_27

/* Bus configuration */
#define OICORE_I2C_PORT_NUM             I2C_NUM_0
#define OICORE_SPI_HOST                 HSPI_HOST

#define OICORE_I2C_IOEXPANDER_ADDRESS   (0x23)
#define OICORE_I2C_RTC_ADDRESS          (0xD0)
#define OICORE_DEFAULT_I2C_SPEED        (400000)

#define OICORE_ETOR_INTERRUPT_PRIORITY  (20U)

#define OICORE_ADC_NO_OF_SAMPLES        (64U)
#define OICORE_ADC_REDUCTION_FACTOR     (9.432f)

#define ESP_INTR_FLAG_DEFAULT           (0)


/*******************************************************************/
/*                                                                 */
/*                  MAPPING OICORE CONNECTOR                       */
/*                                                                 */
/*******************************************************************/
/*| 9V-30V | STOR1 | STOR3 |  ANA  |       |  I2C  |  CAN  |  RS  |*/
/*|  VIN   | ETOR1 | ETOR3 |   1   |  +5V  |  SDA  |   H   |  TX  |*/
/*|--------|-------|-------|-------|-------|-------|-------|------|*/
/*|        | STOR2 | STOR4 |  ANA  |       |  I2C  |  CAN  |  RS  |*/
/*|  GND   | ETOR2 | ETOR4 |   2   |  GND  |  SCL  |   L   |  RX  |*/
/*******************************************************************/


class OICore : public OIModuleMaster
{
public:

    /**
     * @brief Hardware init
     * 
     */
    void init();

    /**
     * @brief Set an output at high or low level.
     * First argument is the STOR to drive and second argument is the level to set.
     * 
     * @param stor STOR to drive.
     * @param level STOR level, HIGH or LOW.
     */
    void digitalWrite(Stor_t stor, uint8_t level) const;

    /**
     * @brief Read an input current level. Argument is the ETOR to read.
     * The function return an integer that correspond to the value of the ETOR.
     * 
     * @param etor ETOR to monitor.
     * @return Value of the ETOR input (1 or 0). 
     */
    static int digitalRead(Etor_t etor);

    /**
     * @brief Read the value of EANA. 
     * The function return a float that correspond to the voltage of the ANA (from 0 to 2.6V).
     * 
     * @param eana ANA input to monitor.
     * @return float Value of the EANA input.
     */
    float analogRead(Eana_t eana) const;

    /**
     * @brief Attach a user callback to the ETOR interrupts.
     * 
     * @param etor ETOR to attach interrupt.
     * @param callback function to attach
     * @param args function arguments
     * @param mode mode of interruption on rising edge, 
     * falling edge or both (rising edge by default).
     */
    void attachInterrupt(Etor_t etor, void (*callback)(void *), void* args=NULL, InterruptMode_t mode=RISING_MODE);
    
    /**
     * @brief Detach an interrupt to a given ETOR.
     * 
     * @param etor ETOR to detach interrupt.
     */
    void detachInterrupt(Etor_t etor, InterruptMode_t mode);

   /**
     * @brief Get the current time compute by RTC Clock.
     * @return time_t : time since epoch in seconds.
     */
    time_t time(void);

   /**
     * @brief Get the current time compute by RTC Clock.
     * @return DateTime : current time.
     */
    DateTime now(void);

    /**
     * @brief Set the time of internal RTC Clock
     * @param[in] time Current time since epoch in seconds
     * @note Must be used with care, RTC Time is setted during factory calibration
     */    
    void setTime(time_t time);

    /**
     * @brief Set the time of internal RTC Clock
     * @param[in] datetime Current date in a DateTime object format
     * @note Must be used with care, RTC Time is setted during factory calibration
     */    
    void setTime(DateTime datetime);

    /**
     * @brief Enable RTC Alarm
     * @note A callback must be registedred before calling this function. \n See attachRTCAlarm function.
     */
    void enableRTCAlarm(void);

    /**
     * @brief Disable RTC Alarm
     */
    void disableRTCAlarm(void);

    /**
     * @brief Set the time for next RTC Alarm
     * @param[in] alarm Time since epoch in seconds
     * @note If no callback has been registered, nothing will happens.
     */  
    void setRTCAlarm(time_t alarm);

    /**
     * @brief Set the time for next RTC Alarm
     * @param[in] alarm Alarm in a DateTime object format
     * @note If no callback has been registered, nothing will happens.
     */  
    void setRTCAlarm(DateTime alarm);

    /**
     * @brief Attach a user callback to the RTC Alarm interrupt.
     * 
     * @param callback function to attach
     * @param args optional function arguments
     */
    void attachRTCAlarm(void (*callback)(void), void * args) const;

    /**
     * @brief Detach the RTC interrupt handler if exists
     */
    void detachRTCAlarm(void) const;

    /**
     * @brief 
     * 
     * @param arg 
     */
    static void handleInterrupt(void* arg);

private:

    static ioex_device_t *_ioex;

    esp_adc_cal_characteristics_t _adc1Characteristics;

    static const ioex_num_t _stor[4];
    static const ioex_num_t _storSensor[4];
    static const ioex_num_t _etor[4];
    static const adc1_channel_t _eana[2];
};

#if defined CONFIG_OI_CORE || CONFIG_OI_TEST
class OISubCore: public OIModuleSlave
{
public:

    OISubCore(uint8_t id) : OIModuleSlave(id) {}

     /**
     * @brief Set an output at high or low level.
     * First argument is the STOR to drive and second argument is the level to set.
     * 
     * @param stor STOR to drive.
     * @param level STOR level, HIGH or LOW.
     */
    inline void digitalWrite(Stor_t stor, uint8_t level) {
        setMessage(OIMessage(CMD_DIGITAL_WRITE, _senderId, (uint16_t) stor, (uint32_t) level));
    }

    /**
     * @brief Read an input current level. Argument is the ETOR to read.
     * The function return an integer that correspond to the value of the ETOR.
     * 
     * @param etor ETOR to monitor.
     * @return Value of the ETOR input (1 or 0). 
     */
    inline int digitalRead(Etor_t etor) const {
        return (int)getMessage(OIMessage(CMD_DIGITAL_READ, _senderId, (uint16_t) etor));
    }

    /**
     * @brief Read the value of EANA. 
     * The function return a float that correspond to the voltage of the ANA (from 0 to 2.6V).
     * 
     * @param eana ANA input to monitor.
     * @return float Value of the EANA input.
     */
    inline float analogRead(Eana_t ana) const {
        uint32_t value =getMessage(OIMessage(CMD_ANALOG_READ, _senderId, (uint16_t) ana));
        return reinterpret_cast<float &>(value);
    }

    /**
     * @brief Attach a user callback to the ETOR interrupts. 
     * First argument is the ETOR to attach interrupt, the second argument is the interrupt handler, third argument is the interrupt mode.
     * 
     * @param etor ETOR to attach interrupt.
     * @param handler Handler to execute when an interrupt occurs.
     * @param mode Interrupt mode (RISING, FALLING or CHANGE)
     */
    inline void attachInterrupt(Etor_t etor, void (*callback)(void), InterruptMode_t mode) {
        setMessage(OIMessage(CMD_ATTACH_INTERRUPT, _senderId, (uint16_t) etor, (uint16_t) mode));
        Fct.add(OIMessage(CMD_ETOR_INTERRUPT, _senderId), [callback](OIMessage msg) -> uint32_t {
            callback();
            return 0;
        });
    }

    /**
     * @brief Detach interrupt of a given ETOR. 
     * First argument is the ETOR to detach interrupt, the second argument is the interrupt mode, third argument is the id of the module that attached the interrupt.
     * 
     * @param etor ETOR to detach interrupt.
     * @param mode Mode of the interrupt to detach (RISING, FALLING or CHANGE).
     */
    inline void detachInterrupt(Etor_t etor, InterruptMode_t mode) {
        setMessage(OIMessage(CMD_DETACH_INTERRUPT, _senderId, (uint16_t) etor, (uint16_t) mode));
    }

private:

};

#endif