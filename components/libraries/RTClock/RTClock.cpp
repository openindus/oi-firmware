/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file RTC.cpp
 * @brief Functions for RTC
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "RTClock.h"

#if defined(CONFIG_CORE)

ioex_device_t *OIRTC::_ioex;

void OIRTC::init(void)
{
    rtc_i2c_set_port(CORE_I2C_PORT_NUM);
}

time_t OIRTC::time(void)
{
    DateTime seconds = now();
    return seconds.unixtime();
}

DateTime OIRTC::now(void)
{
    M41T62_Time_st time;
    M41T62_Date_st date;
    M41T62_Getting_Time(&time);
    M41T62_Getting_Date(&date);
    DateTime now(date.Year, date.Month, date.DayMonth, time.Hours, time.Minutes, time.Seconds);

    return now;
}

void OIRTC::setTime(time_t time)
{
    setTime(DateTime(time));
}

void OIRTC::setTime(DateTime datetime)
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

void OIRTC::enableRTCAlarm(void)
{
    M41T62_Set_AFE_Bit(M41T62_AFE_HIGH);
    ioex_interrupt_enable(_ioex, IOEX_NUM_25);
}

void OIRTC::disableRTCAlarm(void)
{
    ioex_interrupt_disable(_ioex, IOEX_NUM_25);
    M41T62_Set_AFE_Bit(M41T62_AFE_LOW);
}

void OIRTC::setRTCAlarm(time_t alarm)
{
    // Set Alarm
    setRTCAlarm(DateTime(alarm));
}

void OIRTC::setRTCAlarm(DateTime alarm)
{
    M41T62_Alarm_st alarm_set;

    alarm_set.AlMonth = alarm.month();
    alarm_set.AlDate = alarm.day();
    alarm_set.AlHour = alarm.hour();
    alarm_set.AlMinutes = alarm.minute();
    alarm_set.AlSeconds = alarm.second();

    M41T62_Alarm_Setting(alarm_set);
}

void OIRTC::attachRTCAlarm(void (*callback)(void), void * args)
{
    rtc_user_handler = (rtc_isr_t)callback;
    ioex_isr_handler_add(_ioex, IOEX_NUM_25, (ioex_isr_t) rtc_isr_handler, args, 10);
}

void OIRTC::detachRTCAlarm(void)
{
    ioex_isr_handler_remove(_ioex, IOEX_NUM_25);
}

const uint8_t daysInMonth [] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

// Number of days since 2000/01/01, valid for 2001..2099
static uint16_t date2days(uint16_t y, uint8_t m, uint8_t d)
{
    if (y >= 2000)
        y -= 2000;
    uint16_t days = d;
    for (uint8_t i = 1; i < m; ++i)
        days += daysInMonth[i - 1];
    if (m > 2 && y % 4 == 0)
        ++days;
    return days + 365 * y + (y + 3) / 4 - 1;
}

static long time2long(uint16_t days, uint8_t h, uint8_t m, uint8_t s)
{
    return ((days * 24L + h) * 60 + m) * 60 + s;
}

////////////////////////////////////////////////////////////////////////////////
// DateTime implementation - ignores time zones and DST changes
// NOTE: also ignores leap seconds, see http://en.wikipedia.org/wiki/Leap_second

DateTime::DateTime(uint32_t t)
{
    t -= SECONDS_FROM_1970_TO_2000;    // bring to 2000 timestamp from 1970

    ss = t % 60;
    t /= 60;
    mm = t % 60;
    t /= 60;
    hh = t % 24;
    uint16_t days = t / 24;
    uint8_t leap;
    for (yOff = 0; ; ++yOff) {
        leap = yOff % 4 == 0;
        if (days < 365 + leap)
            break;
        days -= 365 + leap;
    }
    for (m = 1; ; ++m) {
        uint8_t daysPerMonth = daysInMonth[m - 1];
        if (leap && m == 2)
            ++daysPerMonth;
        if (days < daysPerMonth)
            break;
        days -= daysPerMonth;
    }
    d = days + 1;
}

DateTime::DateTime (uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec)
{
    if (year >= 2000)
        year -= 2000;
    yOff = year;
    m = month;
    d = day;
    hh = hour;
    mm = min;
    ss = sec;
}

DateTime::DateTime (const DateTime& copy):
  yOff(copy.yOff),
  m(copy.m),
  d(copy.d),
  hh(copy.hh),
  mm(copy.mm),
  ss(copy.ss)
{}

static uint8_t conv2d(const char* p)
{
    uint8_t v = 0;
    if ('0' <= *p && *p <= '9')
        v = *p - '0';
    return 10 * v + *++p - '0';
}

// A convenient constructor for using "the compiler's time":
//   DateTime now (__DATE__, __TIME__);
DateTime::DateTime (const char* date, const char* time)
{
    // sample input: date = "Dec 26 2009", time = "12:34:56"
    yOff = conv2d(date + 9);
    // Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec 
    switch (date[0]) {
        case 'J': m = date[1] == 'a' ? 1 : date[2] == 'n' ? 6 : 7; break;
        case 'F': m = 2; break;
        case 'A': m = date[2] == 'r' ? 4 : 8; break;
        case 'M': m = date[2] == 'r' ? 3 : 5; break;
        case 'S': m = 9; break;
        case 'O': m = 10; break;
        case 'N': m = 11; break;
        case 'D': m = 12; break;
    }
    d = conv2d(date + 4);
    hh = conv2d(time);
    mm = conv2d(time + 3);
    ss = conv2d(time + 6);
}

uint8_t DateTime::dayOfWeek() const
{    
    uint16_t day = date2days(yOff, m, d);
    return (day + 6) % 7; // Jan 1, 2000 is a Saturday, i.e. returns 6
}

uint32_t DateTime::unixtime(void) const
{
  uint32_t t;
  uint16_t days = date2days(yOff, m, d);
  t = time2long(days, hh, mm, ss);
  t += SECONDS_FROM_1970_TO_2000;  // seconds from 1970 to 2000

  return t;
}

long DateTime::secondstime(void) const
{
  long t;
  uint16_t days = date2days(yOff, m, d);
  t = time2long(days, hh, mm, ss);
  return t;
}

OIRTC RTC;

#endif