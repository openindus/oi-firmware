#include "OpenIndus.h"
#include "Arduino.h"

OICore core;

int value = 0;

void setup(void)
{
    Serial.begin(115200);
}

void loop(void)
{
    value = core.analogRead(AIN_1);
    Serial.print("AIN_1 :");
    Serial.println(value);

    value = core.analogRead(AIN_2);
    Serial.print("AIN_1 :");
    Serial.println(value);

    value = core.analogReadMilliVolt(AIN_1);
    Serial.print("AIN_1 :");
    Serial.print(value);
    Serial.println("mV");

    value = core.analogReadMilliVolt(AIN_2);
    Serial.print("AIN_2 :");
    Serial.print(value);
    Serial.println("mV");

    delay(1000);
}