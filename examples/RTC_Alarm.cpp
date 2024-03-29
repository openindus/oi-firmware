#include "OpenIndus.h"
#include "Arduino.h"
#include "RTC.h"

DateTime currTime;

void rtc_alarm(void)
{
    static uint8_t color = LED_RED;

    DateTime currTime= RTC.now();
    DateTime awakeTime(currTime.year(), currTime.month(), currTime.day(), currTime.hour(), currTime.minute() + 1, currTime.second());    
    RTC.setRTCAlarm(awakeTime);
}

void setup(void)
{
    RTC.setTime(DateTime(__DATE__, __TIME__));

    currTime = RTC.now();
    DateTime awakeTime(currTime.year(), currTime.month(), currTime.day(), currTime.hour(), currTime.minute() + 1, currTime.second());

    RTC.attachRTCAlarm(rtc_alarm, NULL);
    RTC.enableRTCAlarm();
    RTC.setRTCAlarm(awakeTime);
}

void loop(void)
{
    currTime = RTC.now();
    printf("[%d-%d %d:%d:%d]", 
            currTime.day(), 
            currTime.month(), 
            currTime.hour(), 
            currTime.minute(), 
            currTime.second());
    
    delay(1000);
}