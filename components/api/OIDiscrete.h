/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file OIDiscrete.h
 * @brief Functions for discrete module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

/* General includes */
#include <map>
#include <stdint.h>

/* ESP-IDF includes */
#include "driver/adc.h"
#include "driver/ledc.h"
#include "esp_adc_cal.h"
#include "soc/adc_channel.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

/* Component includes */
#include "OIBus.h"
#include "OIModule.h"
#include "OIType.h"
#include "OIStor.h"
#include "OIEtor.h"

#define OIDISCRETE_PIN_CMD_MOSFET      (GPIO_NUM_2)


/***************************************************************************************************/
/*----------------------------------------- DISCRETE ----------------------------------------------*/
/***************************************************************************************************/

#if defined CONFIG_OI_DISCRETE

/* ETOR */
#define OIDISCRETE_PIN_ETOR1           (GPIO_NUM_41)
#define OIDISCRETE_PIN_ETOR2           (GPIO_NUM_46)
#define OIDISCRETE_PIN_ETOR3           (GPIO_NUM_36)
#define OIDISCRETE_PIN_ETOR4           (GPIO_NUM_26)
#define OIDISCRETE_PIN_ETOR5           (GPIO_NUM_33)
#define OIDISCRETE_PIN_ETOR6           (GPIO_NUM_45)
#define OIDISCRETE_PIN_ETOR7           (GPIO_NUM_35)
#define OIDISCRETE_PIN_ETOR8           (GPIO_NUM_38)
#define OIDISCRETE_PIN_ETOR9           (GPIO_NUM_34)
#define OIDISCRETE_PIN_ETOR10          (GPIO_NUM_37)

/* STOR_Cmd_B */
#define OIDISCRETE_PIN_STOR1           (GPIO_NUM_40)
#define OIDISCRETE_PIN_STOR2           (GPIO_NUM_39)
#define OIDISCRETE_PIN_STOR3           (GPIO_NUM_42)
#define OIDISCRETE_PIN_STOR4           (GPIO_NUM_21)

/* STOR_Cmd_A */
#define OIDISCRETE_PIN_STOR5           (GPIO_NUM_6)
#define OIDISCRETE_PIN_STOR6           (GPIO_NUM_7)
#define OIDISCRETE_PIN_STOR7           (GPIO_NUM_8)
#define OIDISCRETE_PIN_STOR8           (GPIO_NUM_9)

/* STOR_Sensors_B*/
#define OIDISCRETE_CHANNEL_STOR_SENSOR1     (ADC_CHANNEL_0)
#define OIDISCRETE_CHANNEL_STOR_SENSOR2     (ADC_CHANNEL_2)
#define OIDISCRETE_CHANNEL_STOR_SENSOR3     (ADC_CHANNEL_4)
#define OIDISCRETE_CHANNEL_STOR_SENSOR4     (ADC_CHANNEL_8)

/* STOR_Sensors_A*/
#define OIDISCRETE_CHANNEL_STOR_SENSOR5     (ADC_CHANNEL_9)
#define OIDISCRETE_CHANNEL_STOR_SENSOR6     (ADC_CHANNEL_5)
#define OIDISCRETE_CHANNEL_STOR_SENSOR7     (ADC_CHANNEL_3)
#define OIDISCRETE_CHANNEL_STOR_SENSOR8     (ADC_CHANNEL_1)

/* ANA */
#define OIDISCRETE_PIN_EANA1            (GPIO_NUM_10) 
#define OIDISCRETE_CHANNEL_EANA1        (ADC1_CHANNEL_9)

/***************************************************************************************************/
/*-------------------------------------- DISCRETE VERTICALE----------------------------------------*/
/***************************************************************************************************/

#else

/* ETOR */
#ifdef CONFIG_IDF_TARGET_ESP32S2
#define OIDISCRETE_PIN_ETOR1           (GPIO_NUM_46)
#else
#define OIDISCRETE_PIN_ETOR1           (GPIO_NUM_0)
#endif
#define OIDISCRETE_PIN_ETOR2           (GPIO_NUM_12)
#define OIDISCRETE_PIN_ETOR3           (GPIO_NUM_6)
#define OIDISCRETE_PIN_ETOR4           (GPIO_NUM_7)
#define OIDISCRETE_PIN_ETOR5           (GPIO_NUM_9)
#define OIDISCRETE_PIN_ETOR6           (GPIO_NUM_8)
#define OIDISCRETE_PIN_ETOR7           (GPIO_NUM_10)
#define OIDISCRETE_PIN_ETOR8           (GPIO_NUM_14)
#ifdef CONFIG_IDF_TARGET_ESP32S2
#define OIDISCRETE_PIN_ETOR9           (GPIO_NUM_45)
#else
#define OIDISCRETE_PIN_ETOR9           (GPIO_NUM_0)
#endif
#define OIDISCRETE_PIN_ETOR10          (GPIO_NUM_11)

/* STOR_Cmd_B */
#define OIDISCRETE_PIN_STOR1           (GPIO_NUM_15)
#ifdef CONFIG_IDF_TARGET_ESP32S2
#define OIDISCRETE_PIN_STOR2           (GPIO_NUM_41)
#else
#define OIDISCRETE_PIN_STOR2           (GPIO_NUM_0)
#endif
#define OIDISCRETE_PIN_STOR3           (GPIO_NUM_39)
#define OIDISCRETE_PIN_STOR4           (GPIO_NUM_26)

/* STOR_Cmd_A */
#define OIDISCRETE_PIN_STOR5           (GPIO_NUM_34)
#define OIDISCRETE_PIN_STOR6           (GPIO_NUM_36)
#define OIDISCRETE_PIN_STOR7           (GPIO_NUM_38)
#define OIDISCRETE_PIN_STOR8           (GPIO_NUM_37)

/* STOR_Sensors_B*/
#define OIDISCRETE_CHANNEL_STOR_SENSOR1     (ADC_CHANNEL_2)
#define OIDISCRETE_CHANNEL_STOR_SENSOR2     (ADC_CHANNEL_2)
#define OIDISCRETE_CHANNEL_STOR_SENSOR3     (ADC_CHANNEL_2)
#define OIDISCRETE_CHANNEL_STOR_SENSOR4     (ADC_CHANNEL_2)

/* STOR_Sensors_A*/
#define OIDISCRETE_CHANNEL_STOR_SENSOR5     (ADC_CHANNEL_2)
#define OIDISCRETE_CHANNEL_STOR_SENSOR6     (ADC_CHANNEL_2)
#define OIDISCRETE_CHANNEL_STOR_SENSOR7     (ADC_CHANNEL_2)
#define OIDISCRETE_CHANNEL_STOR_SENSOR8     (ADC_CHANNEL_2)

/* ANA */
#define OIDISCRETE_PIN_EANA1            (GPIO_NUM_16) 
#define OIDISCRETE_CHANNEL_EANA1         (ADC_CHANNEL_1)

#endif


  /*********************************************************************/
  /*                    MAPPING OIDISCRETE CONNECTOR                   */
  /*********************************************************************/

  /*********************************************************************/
  /*| 9V-30V |       |       |       | STOR1 | STOR3  | STOR5 | STOR7 |*/
  /*|  VIN   | ETOR1 | ETOR3 | ETOR5 | ETOR7 | ETOR9  |       |       |*/
  /*|--------|-------|-------|-------|-------|--------|-------|-------|*/
/***|        |       |       |       | STOR2 | STOR4  | STOR6 | STOR8 |***/
/* *|  GND   | ETOR2 | ETOR4 | ETOR6 | ETOR8 | ETOR10 |       |       |* */
/*************************************************************************/


#if defined CONFIG_OI_DISCRETE || CONFIG_OI_DISCRETE_VERTICAL

class OIDiscrete : public OIModuleMaster
{

public:

    OIDiscrete() : OIModuleMaster() {}

    virtual void init(void);

    /**
     * @brief Set an output at high or low level.
     * First argument is the STOR to drive and second argument is the level to set.
     * 
     * @param stor STOR to drive.
     * @param level STOR level, HIGH or LOW.
     */
    void digitalWrite(Stor_t stor, uint8_t level);

    /**
     * @brief Set a PWM output at a given duty cycle. 
     * First argument is the STOR to drive and second argument is the duty cycle to set (from 0 to 255).
     * 
     * @param stor STOR to drive.
     * @param duty Value of the duty cycle (from 0 to 255).
     */
    void analogWrite(Stor_t stor, uint8_t duty);

    /**
     * @brief Read an input current level. Argument is the ETOR to read.
     * The function return an integer that correspond to the value of the ETOR.
     * 
     * @param etor ETOR to monitor.
     * @return Value of the ETOR input (1 or 0). 
     */
    int digitalRead(Etor_t etor);
    
    /**
     * @brief Read the value of EANA. 
     * The function return a float that correspond to the voltage of the ANA (from 0 to 2.6V).
     * 
     * @param eana ANA input to monitor.
     * @return uint32_t : Adc raw value
     */
    uint32_t analogRead(Eana_t ana);

      /**
     * @brief Read a voltage measure on analog pins
     * @param[in] ana : Analog input
     * @param[in] units : Format of the returned value
     * @return float : Measure in specified units
     */
    float analogRead(Eana_t ana, Units_t units);

    /**
     * @brief Read the value of STOR sensor. 
     * The function return a float that correspond to the current of the STOR.
     * 
     * @param stor STOR to monitor.
     * @return float Value of the STOR output current.
     */
    float getStorCurrent(Stor_t stor);

    /**
     * @brief Attach a user callback to the ETOR interrupts. 
     * First argument is the ETOR to attach interrupt, the second argument is the interrupt handler, third argument is the interrupt mode.
     * 
     * @param etor ETOR to attach interrupt.
     * @param handler Handler to execute when an interrupt occurs.
     * @param mode Interrupt mode (RISING, FALLING or CHANGE)
     */
    void attachInterrupt(Etor_t etor, voidFuncPtrArg handler, InterruptMode_t mode = RISING_MODE);

    /**
     * @brief Detach interrupt of a given ETOR. 
     * First argument is the ETOR to detach interrupt, the second argument is the interrupt mode, third argument is the id of the module that attached the interrupt.
     * 
     * @param etor ETOR to detach interrupt.
     * @param mode Mode of the interrupt to detach (RISING, FALLING or CHANGE).
     */
    void detachInterrupt(Etor_t etor,  InterruptMode_t mode);

    /**
     * @brief handle interrupt
     * 
     * @param arg 
     */
    static void IRAM_ATTR handleInterrupt(void* arg);

private:

    OIStor<8> _stor;
    static OIStorDescr_s _storConf[8];
    OIEtor<10> _etor;
    static OIEtorDescr_s _etorConf[10];

    std::map<Eana_t, adc_channel_t> _anaToAdcChannel;

    gpio_num_t getGpioFromEtor(Etor_t etor) { return _etor.getGpioFromEtor(etor);};

    static xQueueHandle _discrete_evt_queue;
    static esp_adc_cal_characteristics_t *_adc_chars_ana;
    static esp_adc_cal_characteristics_t *_adc_chars_stor;

    static void _discrete_isr_task(void* arg);

};

#else

class OIDiscrete : public OIModuleSlave
{
public:

    OIDiscrete(uint8_t id) : OIModuleSlave(id) {}

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
     * @brief Set a PWM output at a given duty cycle. 
     * First argument is the STOR to drive and second argument is the duty cycle to set (from 0 to 255).
     * 
     * @param stor STOR to drive.
     * @param duty Value of the duty cycle (from 0 to 255).
     */
    inline void analogWrite(Stor_t stor, uint8_t duty) {
        setMessage(OIMessage(CMD_ANALOG_WRITE, _senderId, (uint16_t) stor, (uint32_t) duty));
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
        float value;
        uint32_t valueReturn = getMessage(OIMessage(CMD_ANALOG_READ, _senderId, (uint16_t) ana));
        memcpy(&value, &valueReturn, sizeof(float));
        return value;
    }

    /**
     * @brief Read the value of STOR sensor. 
     * The function return a float that correspond to the current of the STOR.
     * 
     * @param stor_sensor STOR sensor to monitor.
     * @return float Value of the STOR output current.
     */
    inline float getStorCurrent(Stor_t stor) const {
        float value;
        uint32_t valueReturn = getMessage(OIMessage(CMD_STOR_SENSOR_READ, _senderId, (uint16_t) stor));
        memcpy(&value, &valueReturn, sizeof(float));
        return value;
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
        callbackTable[etor] = callback;
        Fct.add(OIMessage(CMD_ETOR_INTERRUPT, _destId), [this](OIMessage msg) -> uint32_t {
            this->runCallbackTable(((Etor_t)msg.getConf()));
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
    void (*callbackTable[ETOR10])(void);

    inline void runCallbackTable(Etor_t etor) { callbackTable[etor](); }
};

#endif /* CONFIG_OI_DISCRETE || CONFIG_OI_DISCRETE_VERTICAL */