#include "OpenIndus.h"
#include "Arduino.h"

OIDiscrete discrete;

void callback(void*)
{
    Serial.println("Interrupt on DIN_1 !"); 
    discrete.detachInterrupt(DIN_1); 
}

void setup(void)
{
    discrete.attachInterrupt(DIN_1, callback, RISING_MODE);
}

void loop(void)
{
    delay(1000);
}