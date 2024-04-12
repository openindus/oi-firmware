#include "OpenIndus.h"
#include "Arduino.h"

OIMixed mixed;

float value = 0.0;

void setup(void) 
{
    Serial.begin(115200);

    /* Init AIN */
    mixed.analogInputMode(AIN_1, AIN_MODE_CURRENT);
    mixed.analogInputMode(AIN_2, AIN_MODE_CURRENT);
    mixed.analogInputMode(AIN_3, AIN_MODE_CURRENT);
    mixed.analogInputMode(AIN_4, AIN_MODE_CURRENT);
    mixed.analogInputResolution(AIN_RES_12_BITS);
}

void loop(void) 
{
    value = mixed.analogReadMilliAmps(AIN_1);
    Serial.print("AIN_1 :");
    Serial.print(value);
    Serial.println("mA");

    value = mixed.analogReadMilliAmps(AIN_2);
    Serial.print("AIN_2 :");
    Serial.print(value);
    Serial.println("mA");

    value = mixed.analogReadMilliAmps(AIN_3);
    Serial.print("AIN_3 :");
    Serial.print(value);
    Serial.println("mA");

    value = mixed.analogReadMilliAmps(AIN_4);
    Serial.print("AIN_4 :");
    Serial.print(value);
    Serial.println("m1");

    delay(1000);
}