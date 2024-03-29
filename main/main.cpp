#include "OpenIndus.h"
#include "Arduino.h"

#ifdef OI_CORE
Core core;
Discrete discrete;
#endif

void setup(void)
{
    printf("Hello OpenIndus !\n");
}

void loop(void)
{
#ifdef OI_CORE
    discrete.ledOn(LED_CYAN);
    delay(1000);
    discrete.ledOn(LED_PURPLE);
#endif
    delay(1000);
}