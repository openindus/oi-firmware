#include "OpenIndus.h"
#include "Arduino.h"

#ifdef CONFIG_OI_CORE
Core core;
#endif
Mixed mixed;

void setup(void)
{
    /* Configure Analog input 1 in voltage mode */
    mixed.analogInputMode(AIN_1, AIN_MODE_VOLTAGE);
}

void loop(void)
{
    float value = mixed.analogReadMilliVolt(AIN_1);
    printf("Value of analog input 1: %f\n", value);
    delay(1000);
}