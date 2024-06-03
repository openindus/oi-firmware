#include "OpenIndus.h"
#include "Arduino.h"
#include "RTClock.h"

OICore core;
DateTime currTime;

void rtc_alarm(void)
{
    DateTime currTime= core.rtc.now();
    DateTime awakeTime(currTime.year(), currTime.month(), 
        currTime.day(), currTime.hour(), currTime.minute() + 1, currTime.second());    
    core.rtc.setRTCAlarm(awakeTime);
}

void setup(void)
{
    core.rtc.begin();
    core.rtc.setTime(DateTime(__DATE__, __TIME__));

    currTime = core.rtc.now();
    DateTime awakeTime(currTime.year(), currTime.month(), 
        currTime.day(), currTime.hour(), currTime.minute() + 1, currTime.second());

    core.rtc.attachRTCAlarm(rtc_alarm, NULL);
    core.rtc.enableRTCAlarm();
    core.rtc.setRTCAlarm(awakeTime);
}

void loop(void)
{
    currTime = core.rtc.now();
    
    printf("[%d-%d %d:%d:%d]\r\n", 
            currTime.day(), 
            currTime.month(), 
            currTime.hour(), 
            currTime.minute(), 
            currTime.second());
    
    delay(1000);
}