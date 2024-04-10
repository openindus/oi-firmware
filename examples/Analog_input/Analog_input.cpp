#include "OpenIndus.h"
#include "Arduino.h"

#ifdef MODULE_OI_CORE
OICore core;
#endif
OIDiscrete discrete;

void setup(void)
{
    Serial.begin(115200);
}

void loop(void)
{
    int value = discrete.analogRead(AIN_1);
    Serial.printf("Value of analog input 1: %d\n", value);
    delay(1000);
}