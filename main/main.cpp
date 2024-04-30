#include "OpenIndus.h"
#include "Arduino.h"

OIDiscrete d;

void setup(void)
{
    d.digitalModePWM(DOUT_1, 50);
}

void loop(void)
{
    d.digitalSetPWM(DOUT_1, (uint32_t)(16383.0*1.0)/20); // 1ms
    delay(1000);
    
    d.digitalSetPWM(DOUT_1, (uint32_t)(16383.0*1.5)/20); // 1.5ms
    delay(1000);

    
    d.digitalSetPWM(DOUT_1, (uint32_t)(16383.0*2)/20); // 2ms
    delay(1000);

}