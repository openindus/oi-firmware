#include "OpenIndus.h"
#include "Arduino.h"
#include "ArduinoModbus.h"

#if defined(CONFIG_TEST)
Core core;
Discrete discrete;
Mixed mixed;
RelayHP relay;
Stepper stepper;
#endif

void setup(void)
{
    printf("Hello OpenIndus!\n");
}

void loop(void)
{
    delay(1000);
}