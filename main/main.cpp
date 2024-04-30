#include "OpenIndus.h"
#include "Arduino.h"

OIDiscrete d;

void setup(void)
{
    d.digitalModePWM(DOUT_1, 100);
}

void loop(void)
{
    d.digitalSetPWM(DOUT_1, (uint32_t)(16383.0*0.5)/10); // 1ms
    delay(1000);
    
    d.digitalSetPWM(DOUT_1, (uint32_t)(16383.0*2)/10); // 2ms
    delay(1000);
}