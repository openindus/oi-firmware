#include "OpenIndus.h"
#include "Arduino.h"

#if defined(OI_CORE)
Core c;
AnalogLS ls;

void setup(void)
{
    // Serial.begin(115200);
    // Serial.println("Hello World !");
    ls.addSensor(RTD_PT100, {AIN_A_P, AIN_A_N});
}

void loop(void)
{
    delay(1000);
}

#endif