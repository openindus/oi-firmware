#include "OpenIndus.h"
#include "Arduino.h"

#ifdef OI_CORE
OICore core;
#endif
OIDiscrete discrete;

void setup(void)
{
    Serial.begin(115200);
}

void loop(void)
{
    int raw = discrete.analogRead(AIN_1);
    Serial.printf("Raw value of analog input 1: %d\n", raw);
    delay(1000);
}