#include "OpenIndus.h"
#include "Arduino.h"

#ifdef CONFIG_OI_CORE
Core core;
#endif
Discrete discrete;

void setup(void)
{

}

void loop(void)
{
    int raw = discrete.analogRead(AIN_1);
    printf("Raw value of analog input 1: %d\n", raw);
    delay(1000);
}