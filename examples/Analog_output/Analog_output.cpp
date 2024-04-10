#include "OpenIndus.h"
#include "Arduino.h"

#ifdef MODULE_OI_CORE
OICore core;
#endif
Mixed mixed;

void setup(void)
{
    Serial.begin(115200);

    /* Configure Analog output 1 in voltage mode */
    mixed.analogOutputMode(AOUT_1, AOUT_MODE_M10V5_10V5);

    /* Configure Analog output 2 in current mode */
    mixed.analogOutputMode(AOUT_2, AOUT_MODE_0mA_20mA);
}

void loop(void)
{
    /* Ramp from -10V to +10V on output 1 */
    for (float f=-10.0; f<=10.0; f+=0.1) {
        mixed.analogWrite(AOUT_1, f);
        delay(100);
    }

    /* Ramp from 4mA to 20mA on output 2 */
    for (float f=4.0; f<=20.0; f+=0.1) {
        mixed.analogWrite(AOUT_2, f);
        delay(100);
    }

    delay(100);
}