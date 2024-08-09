#include "OpenIndus.h"
#include "Arduino.h"

#ifdef OI_CORE
Core core;
#endif
Discrete discrete;

void callback(void*)
{
    printf("Interrupt on DIN_1 !\n");
}

void setup(void)
{
    // Attach an interrupt callback to digital input 1
    discrete.attachInterrupt(DIN_1, callback, RISING_MODE);
}

void loop(void)
{
    // Read digital input 2
    int value = discrete.digitalRead(DIN_2);
    printf("Value of DIN_2: %d\n", value);

    delay(1000); // Wait 1s
}
