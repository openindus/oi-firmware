#include "OpenIndus.h"
#include "Arduino.h"

#ifdef OI_CORE
OICore core;
#endif
OIMixed mixed;

void setup(void)
{
    Serial.begin(115200);

    /* Configure Analog input 1 in voltage mode */
    mixed.analogInputMode(AIN_1, AIN_MODE_VOLTAGE);
}

void loop(void)
{
    float value = mixed.analogReadMilliVolt(AIN_1);
    Serial.printf("Value of analog input 1: %f\n", value);
    delay(1000);
}