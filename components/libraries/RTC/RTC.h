/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file RTC.h
 * @brief Functions for RTC
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "esp_log.h"
#include "pcal6524.h"
#include "m41t62.h"

#if defined(CONFIG_CORE)

class DateTime
{
public:
    DateTime(uint32_t t =0);
    DateTime(uint16_t year, uint8_t month, uint8_t day,
                uint8_t hour =0, uint8_t min =0, uint8_t sec =0);
    DateTime(const DateTime& copy);
    DateTime(const char* date, const char* time);
    uint16_t year() const       { return 2000 + yOff; }
    uint8_t month() const       { return m; }
    uint8_t day() const         { return d; }
    uint8_t hour() const        { return hh; }
    uint8_t minute() const      { return mm; }
    uint8_t second() const      { return ss; }
    uint8_t dayOfWeek() const;

    // 32-bit times as seconds since 1/1/2000
    long secondstime() const;   
    // 32-bit times as seconds since 1/1/1970
    uint32_t unixtime(void) const; 

protected:
    uint8_t yOff;
    uint8_t m;
    uint8_t d;
    uint8_t hh;
    uint8_t mm;
    uint8_t ss;
};

class OIRTC
{
public:

    OIRTC() {}

    void init(void);

    /**
     * @brief Get the current time compute by RTC Clock.
     * @return time_t : time since epoch in seconds.
     */
    virtual time_t time(void);

    /**
     * @brief Get the current time compute by RTC Clock.
     * @return DateTime : current time.
     */
    virtual DateTime now(void);

    /**
     * @brief Set the time of internal RTC Clock
     * @param[in] time Current time since epoch in seconds
     * @note Must be used with care, RTC Time is setted during factory calibration
     */    
    virtual void setTime(time_t time);

    /**
     * @brief Set the time of internal RTC Clock
     * @param[in] datetime Current date in a DateTime object format
     * @note Must be used with care, RTC Time is setted during factory calibration
     */    
    virtual void setTime(DateTime datetime);

    /**
     * @brief Enable RTC Alarm
     * @note A callback must be registedred before calling this function. \n See attachRTCAlarm function.
     */
    virtual void enableRTCAlarm(void);

    /**
     * @brief Disable RTC Alarm
     */
    virtual void disableRTCAlarm(void);

    /**
     * @brief Set the time for next RTC Alarm
     * @param[in] alarm Time since epoch in seconds
     * @note If no callback has been registered, nothing will happens.
     */  
    virtual void setRTCAlarm(time_t alarm);

    /**
     * @brief Set the time for next RTC Alarm
     * @param[in] alarm Alarm in a DateTime object format
     * @note If no callback has been registered, nothing will happens.
     */  
    virtual void setRTCAlarm(DateTime alarm);

    /**
     * @brief Attach a user callback to the RTC Alarm interrupt.
     * 
     * @param callback function to attach
     * @param args optional function arguments
     */
    virtual void attachRTCAlarm(void (*callback)(void), void * args);

    /**
     * @brief Detach the RTC interrupt handler if exists
     */
    virtual void detachRTCAlarm(void);

private:

    static ioex_device_t *_ioex;

};

#define SECONDS_FROM_1970_TO_2000 946684800

extern OIRTC RTC;

#endif