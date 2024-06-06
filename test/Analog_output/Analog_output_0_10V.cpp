#include "OpenIndus.h"
#include "Arduino.h"

#ifdef OI_CORE
Core core;
#endif
Mixed mixed;

void setup(void)
{
    /* Configure Analog output 1 in voltage mode */
    mixed.analogOutputMode(AOUT_1, AOUT_MODE_M10V5_10V5);
}

void loop(void)
{
    /* Ramp from -10V to +10V on output 1 */
    for (float f=-10.0; f<=10.0; f+=0.1) {
        mixed.analogWrite(AOUT_1, f);
        delay(100);
    }
    delay(100);
}