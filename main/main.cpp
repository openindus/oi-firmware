#include "OpenIndus.h"
#include "Arduino.h"

OIDiscrete d;

void setup(void)
{
    d.digitalModePWM(DOUT_1, 50);
    d.digitalSetPWM(DOUT_1, 16383*1/20);
}

void loop(void)
{
    delay(100);
}