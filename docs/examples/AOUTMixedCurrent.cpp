#include "OpenIndus.h"
#include "Arduino.h"

OIMixed mixed;

void setup(void) 
{
    Serial.begin(115200);

    /* Init AOUT */
    mixed.analogOutputMode(AOUT_1, AOUT_MODE_0mA_20mA);
    mixed.analogOutputMode(AOUT_2, AOUT_MODE_0mA_20mA);
}

//Output values which will be changed with this variable
float counter = 1;

void loop(void) 
{
    //mixed.analogWrite(CHANNEL, OUTPUT_VOLTAGE_VALUE);
    mixed.analogWrite(AOUT_1, counter);
    mixed.analogWrite(AOUT_2, counter);

    Serial.println("All channels set at "+String(counter)+"V");
    
    counter = counter + 0.1;
    //Maximum output value is 20mA
    if (counter >= 20.0)
    {
        counter = 0;
        //Additional 100 ms delay introduced to manage 20mA -> 0V fall time of 150 ms
        delay(100);
    }
    delay(100);
}