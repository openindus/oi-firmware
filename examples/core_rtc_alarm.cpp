#include "OpenIndus.h"
#include "Arduino.h"

DateTime currTime;

void rtc_alarm(void)
{
    static uint8_t color = LED_RED;

    DateTime currTime= Core.now();
    DateTime awakeTime(currTime.year(), currTime.month(), currTime.day(), currTime.hour(), currTime.minute() + 1, currTime.second());    
    Core.setRTCAlarm(awakeTime);
}

void setup(void)
{
    Core.setTime(DateTime(__DATE__, __TIME__));

    currTime = Core.now();
    DateTime awakeTime(currTime.year(), currTime.month(), currTime.day(), currTime.hour(), currTime.minute() + 1, currTime.second());

    Core.attachRTCAlarm(rtc_alarm, NULL);
    Core.enableRTCAlarm();
    Core.setRTCAlarm(awakeTime);
}

void loop(void)
{
    currTime = Core.now();
    printf("[%d-%d %d:%d:%d]", 
            currTime.day(), 
            currTime.month(), 
            currTime.hour(), 
            currTime.minute(), 
            currTime.second());
    
    delay(1000);
}