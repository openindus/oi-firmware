#include "OpenIndus.h"
#include "Arduino.h"

#ifdef CONFIG_OI_CORE
Core core;
#endif
Mixed mixed;

void setup(void)
{
    /* Configure Analog output 2 in current mode */
    mixed.analogOutputMode(AOUT_2, AOUT_MODE_0mA_20mA);
}

void loop(void)
{
    /* Ramp from 4mA to 20mA on output 2 */
    for (float f=4.0; f<=20.0; f+=0.1) {
        mixed.analogWrite(AOUT_2, f);
        delay(100);
    }
    delay(100);
}