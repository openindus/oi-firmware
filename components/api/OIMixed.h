/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file OIMixed.h
 * @brief Functions for OIMixed module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

/* General includes */
#include <map>
#include <stdint.h>

/* ESP-IDF includes */
#include "esp_adc_cal.h"
#include "driver/ledc.h"

/* Component includes */
#include "OIBus.h"
#include "OIModule.h"
#include "OIType.h"
#include "OIStor.h"
#include "OIEtor.h"
#include "ads866x/ads866x.h"
#include "dac8760/dac8760.h"

/* Generic Pinout */
#define OIMIXED_PIN_CAN_RX          (GPIO_NUM_4)
#define OIMIXED_PIN_CAN_TX          (GPIO_NUM_5)
#define OIMIXED_PIN_RS_UART_RX      (GPIO_NUM_18)
#define OIMIXED_PIN_RS_UART_TX      (GPIO_NUM_17)
#define OIMIXED_PIN_CMD_MOSFET      (GPIO_NUM_2)

/* SPI pins */
#define OIMIXED_SPI_HOST            FSPI_HOST
#define OIMIXED_SPI_FREQ            SPI_MASTER_FREQ_8M
#define OIMIXED_PIN_SPI_MISO        (GPIO_NUM_37)
#define OIMIXED_PIN_SPI_MOSI        (GPIO_NUM_35)
#define OIMIXED_PIN_SPI_SCK         (GPIO_NUM_36)


  /*********************************************************************/
  /*                    MAPPING OIMIXED CONNECTOR                      */
  /*********************************************************************/

  /*********************************************************************/
  /*| 9V-30V |       |       |       |       |        | STOR1 | STOR3 |*/
  /*|  VIN   | EANA1 | EANA2 | EANA3 | EANA4 | SANA1  | ETOR1 | ETOR3 |*/
  /*|--------|-------|-------|-------|-------|--------|-------|-------|*/
  /*|        |       |       |       |       |        | STOR2 | STOR4 |*/
  /*|  GND   |  GND  |  GND  |  GND  |  GND  | SANA2  | ETOR2 | ETOR4 |*/
  /*********************************************************************/

#if defined CONFIG_OI_MIXED

/* ETOR */
#define OIMIXED_PIN_ETOR1           (GPIO_NUM_20)
#define OIMIXED_PIN_ETOR2           (GPIO_NUM_19)
#define OIMIXED_PIN_ETOR3           (GPIO_NUM_46)
#define OIMIXED_PIN_ETOR4           (GPIO_NUM_26)

/* STOR */
#define OIMIXED_PIN_STOR1           (GPIO_NUM_45)
#define OIMIXED_PIN_STOR2           (GPIO_NUM_42)
#define OIMIXED_PIN_STOR3           (GPIO_NUM_38)
#define OIMIXED_PIN_STOR4           (GPIO_NUM_33)

/* STOR_Sensors */
#define OIMIXED_CHANNEL_STOR_SENSOR1     (ADC_CHANNEL_3)
#define OIMIXED_CHANNEL_STOR_SENSOR2     (ADC_CHANNEL_2)
#define OIMIXED_CHANNEL_STOR_SENSOR3     (ADC_CHANNEL_1)
#define OIMIXED_CHANNEL_STOR_SENSOR4     (ADC_CHANNEL_0)

/* ANA */
#define OIMIXED_PIN_EANA1                (GPIO_NUM_6) 
#define OIMIXED_CHANNEL_EANA1            (ADC1_CHANNEL_5)
#define OIMIXED_PIN_EANA2                (GPIO_NUM_7) 
#define OIMIXED_CHANNEL_EANA2            (ADC1_CHANNEL_6)

/* ADS8664 Pin */
#define OIMIXED_ADC_NB                  (uint8_t)4
#define OIMIXED_ADC_DEFAULT_RES_BITS    (uint8_t)12
#define OIMIXED_ADC_PIN_RST             (GPIO_NUM_8)
#define OIMIXED_ADC_PIN_CS              (GPIO_NUM_34)
#define OIMIXED_ADC_PIN_MODE_EANA1      (GPIO_NUM_21)
#define OIMIXED_ADC_PIN_MODE_EANA2      (GPIO_NUM_15)
#define OIMIXED_ADC_PIN_MODE_EANA3      (GPIO_NUM_16)
#define OIMIXED_ADC_PIN_MODE_EANA4      (GPIO_NUM_10)

/* DAC8760 Configuration */
#define OIMIXED_DAC_NB                  (uint8_t)2
#define OIMIXED_DAC_PIN_CS1             (GPIO_NUM_39)
#define OIMIXED_DAC_PIN_CS2             (GPIO_NUM_40)

#endif

#if defined CONFIG_OI_MIXED

class OIMixed : public OIModuleMaster
{
typedef void (*voidFuncPtr)(void);
typedef void (*voidFuncPtrArg)(void*);

public:

    OIMixed();

    void init(void);

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
     * @brief Read the value of STOR sensor. 
     * The function return a float that correspond to the current of the STOR.
     * 
     * @param stor STOR to monitor.
     * @return float Value of the STOR output current.
     */
    float getStorCurrent(Stor_t stor);

    
    /******* Analog inputs functions ********/

    /**
     * @brief Read a voltage measure on analog pins
     * @param[in] ana : Analog input
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
     * @brief Set AdcMode of the current input
     * @param[in] ana : Analog input
     * @param[in] mode : Voltage or current measurement
     */    
    void analogReadMode(Eana_t ana, AdcMode_t mode);

    /**
     * @brief Set adc resolution measurement
     * @param[in] res : Adc resolution
     */  
    void analogReadResolution(AdcRes_t res);

    /**
     * @brief Get adc internal reference voltage
     * @param[in] units : Format of the returned value
     */ 
    float getAnalogReference(Units_t units);

    /** Analog outputs functions **/

    /**
     * @brief Configure voltage range meaurement of the analog output
     * @param[in] sana : Analog output
     * @param[in] mode : Configure voltage range output
     */ 
    void setAnalogMode(Sana_t sana, DACAnalogMode_t mode);

    /**
     * @brief Get configured voltage range of analog output
     * @param[in] sana : Analog output
     * @returns DACAnalogMode - Configured mode of the analog output
     */ 
    DACAnalogMode_t getAnalogMode(Sana_t sana);

    /**
     * @brief Set a voltage analog output to desired value
     * @param[in] sana : Analog output
     * @param[in] value : Desired value
     */ 
    void voltageWrite(Sana_t sana, uint32_t value);

    /**
     * @brief Set a voltage analog output to desired value
     * @param[in] sana : Analog output
     * @param[in] value : Desired value
     * @param[in] units : Specify format of the value
     */ 
    void voltageWrite(Sana_t sana, float value, Units_t units);

    /**
     * @brief Configure current range meaurement of the analog output
     * @param[in] sana : Analog output
     * @param[in] mode : Configure current range output
     */ 
    void setCurrentMode(Sana_t sana, DACCurrentMode_t mode);

    /**
     * @brief Get configured current range of the analog output
     * @param[in] sana : Analog output
     * @returns DACCurrentMode - Configured mode of the analog output
     */ 
    DACCurrentMode_t getCurrentMode(Sana_t sana);    

    /**
     * @brief Set a current analog output to desired value
     * @param[in] sana : Analog output
     * @param[in] value : Desired value
     */ 
    void currentWrite(Sana_t sana, uint32_t value);

    /**
     * @brief Set a current analog output to desired value
     * @param[in] sana : Analog output
     * @param[in] value : Desired value
     * @param[in] units : Specify format of the value
     */ 
    void currentWrite(Sana_t sana, float value, Units_t units);


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
    static Dac8760_DeviceConfig_t _dac8760DeviceConfig;
    static Ads866x_DeviceConfig_t _ads866xDeviceConfig;

    OIStor<4> _stor;
    static OIStorDescr_s _storConf[4];
    OIEtor<4> _etor;
    static OIEtorDescr_s _etorConf[4];

    void _initEtor(void);
    void _initEAna(void);
    void _initSAna(void);

    std::map<Eana_t, adc_channel_t> _anaToAdcChannel;

    gpio_num_t getGpioFromEtor(Etor_t etor) { return _etor.getGpioFromEtor(etor);};

    static xQueueHandle _mixed_evt_queue;
    static esp_adc_cal_characteristics_t *_adc_chars_ana;

    static void _mixed_isr_task(void* arg);    
};

#else

class OIMixed : public OIModuleSlave
{
public:

    OIMixed(uint8_t id) : OIModuleSlave(id) {}

    /**
     * @brief Set an output at high or low level.\n
     * First argument is the STOR to drive and second argument is the level to set.
     * 
     * @param stor STOR to drive.
     * @param level STOR level, HIGH or LOW.
     */
    inline void digitalWrite(Stor_t stor, uint8_t level) {
        setMessage(OIMessage(CMD_DIGITAL_WRITE, _senderId, (uint16_t)stor, (uint32_t)(level & 1)));
    }

    /**
     * @brief Set a PWM output at a given duty cycle.\n
     * First argument is the STOR to drive and second argument is the duty cycle to set (from 0 to 255).
     * 
     * @param stor STOR to drive.
     * @param duty Value of the duty cycle (from 0 to 255).
     */
    inline void analogWrite(Stor_t stor, uint8_t duty) {
        setMessage(OIMessage(CMD_ANALOG_WRITE, _senderId, (uint16_t) stor, (uint32_t) duty));
    }

    /**
     * @brief Read an input current level. Argument is the ETOR to read.\n
     * The function returns an integer that correspond to the value of the ETOR.
     * 
     * @param etor ETOR to monitor.
     * @return Value of the ETOR input (1 or 0). 
     */
    inline int digitalRead(Etor_t etor) const {
        return (int)getMessage(OIMessage(CMD_DIGITAL_READ, _senderId, (uint16_t) etor));
    }

    /**
     * @brief Read the value of STOR sensor.\n
     * The function return a float that correspond to the current of the STOR.
     * 
     * @param stor STOR sensor to monitor.
     * @return float Value of the STOR output current.
     */
    inline float getStorCurrent(Stor_t stor) const {
        uint32_t value = getMessage(OIMessage(CMD_STOR_SENSOR_READ, _senderId, (uint16_t) stor));
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
        setMessage(OIMessage(CMD_ATTACH_INTERRUPT, _senderId, (uint16_t) etor, (uint32_t) mode));
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
        setMessage(OIMessage(CMD_DETACH_INTERRUPT, _senderId, (uint16_t)etor, (uint32_t)mode));
    }

    /**
     * @brief Read a voltage measure on analog pins
     * @param[in] ana : Analog input
     * @return uint32_t : Adc raw value
     */
    inline uint32_t analogRead(Eana_t ana)
    {
        return getMessage(OIMessage(CMD_ANALOG_READ_ADC, _senderId, (uint16_t)((UNITS_RAW << 8) | ana)));
    }

    /**
     * @brief Read a voltage measure on analog pins with specified units.
     * @param[in] ana : Analog input
     * @param[in] units : Format of the returned value
     * @return float : Measure in specified units
     */
    inline float analogRead(Eana_t ana, Units_t units)
    {
        uint32_t value = getMessage(OIMessage(CMD_ANALOG_READ_ADC, _senderId, (uint16_t)((units << 8) | ana)));
        return reinterpret_cast<float &>(value);
    }

    /**
     * @brief Set AdcMode of the analog input
     * @param[in] ana : Analog input
     * @param[in] mode : Voltage or Current
     */    
    inline void analogReadMode(Eana_t ana, AdcMode_t mode)
    {
        setMessage(OIMessage(CMD_SET_ANALOG_ADC_MODE, _senderId, (uint16_t)ana, (uint32_t)mode));
    }

    /**
     * @brief Set adc resolution measurement
     * @param[in] res : Adc resolution
     */  
    inline void analogReadResolution(AdcRes_t res)
    {
        setMessage(OIMessage(CMD_SET_ANALOG_ADC_RESOLUTION, _senderId, (uint16_t)res));
    }

    /**
     * @brief Get adc internal reference voltage
     * @param[in] units : Format of the returned value
     */ 
    inline float getAnalogReference(Units_t units)
    {
        uint32_t value = getMessage(OIMessage(CMD_GET_ANALOG_ADC_REFERENCE, _senderId, (uint16_t)units));
        return reinterpret_cast<float &>(value);        
    }

    /**
     * @brief Configure voltage range meaurement of the analog output
     * @param[in] sana : Analog output
     * @param[in] mode : Configure voltage range output
     */ 
    inline void setAnalogMode(Sana_t sana, DACAnalogMode_t mode)
    {
        setMessage(OIMessage(CMD_SET_ANALOG_DAC_MODE, _senderId, (uint16_t)sana, (uint32_t)mode));
    }

    /**
     * @brief Get configured voltage range of analog output
     * @param[in] sana : Analog output
     * @returns DACAnalogMode - Configured mode of the analog output
     */ 
    inline DACAnalogMode_t getAnalogMode(Sana_t sana)
    {
        uint32_t value = getMessage(OIMessage(CMD_GET_ANALOG_DAC_MODE, _senderId, (uint16_t)sana));
        return reinterpret_cast<DACAnalogMode_t &>(value);               
    }

    /**
     * @brief Set a voltage analog output to desired value
     * @param[in] sana : Analog output
     * @param[in] value : Desired value
     */ 
    inline void voltageWrite(Sana_t sana, uint32_t value)
    {
        setMessage(OIMessage(CMD_VOLTAGE_WRITE_DAC, _senderId, (uint16_t)((UNITS_RAW << 8) | sana), value));
    }

    /**
     * @brief Set a voltage analog output to desired value
     * @param[in] sana : Analog output
     * @param[in] value : Desired value
     * @param[in] units : Specify format of the value
     */ 
    inline void voltageWrite(Sana_t sana, float value, Units_t units)
    {
        uint32_t val = reinterpret_cast<uint32_t &>(value);
        setMessage(OIMessage(CMD_VOLTAGE_WRITE_DAC, _senderId, (uint16_t)((units << 8) | sana), val));
    }

    /**
     * @brief Configure current range meaurement of the analog output
     * @param[in] sana : Analog output
     * @param[in] mode : Configure current range output
     */ 
    inline void setCurrentMode(Sana_t sana, DACCurrentMode_t mode)
    {
        setMessage(OIMessage(CMD_SET_CURRENT_DAC_MODE, _senderId, (uint16_t) sana, (uint32_t)mode));
    }

    /**
     * @brief Get configured current range of the analog output
     * @param[in] sana : Analog output
     * @returns DACCurrentMode - Configured mode of the analog output
     */ 
    inline DACCurrentMode_t getCurrentMode(Sana_t sana)
    {
        uint32_t value = getMessage(OIMessage(CMD_GET_CURRENT_DAC_MODE, _senderId, (uint16_t)sana));
        return reinterpret_cast<DACCurrentMode_t &>(value);  
    }

    /**
     * @brief Set a current analog output to desired value
     * @param[in] sana : Analog output
     * @param[in] value : Desired value
     */ 
    inline void currentWrite(Sana_t sana, uint32_t value)
    {
        setMessage(OIMessage(CMD_CURRENT_WRITE_DAC, _senderId, (uint16_t)((UNITS_RAW << 8) | sana), value));
    }

    /**
     * @brief Set a current analog output to desired value
     * @param[in] sana : Analog output
     * @param[in] value : Desired value
     * @param[in] units : Specify format of the value
     */ 
    inline void currentWrite(Sana_t sana, float value, Units_t units)
    {
        uint32_t val = reinterpret_cast<uint32_t &>(value);
        setMessage(OIMessage(CMD_CURRENT_WRITE_DAC, _senderId, (uint16_t)((units << 8) | sana), val));
    }

};

#endif /* CONFIG_OI_MIXED */