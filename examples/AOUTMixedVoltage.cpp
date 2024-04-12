#include "OpenIndus.h"
#include "Arduino.h"

OIMixed mixed;

void setup(void) 
{
    Serial.begin(115200);

    /* Init AOUT */
    mixed.analogOutputMode(AOUT_1, AOUT_MODE_M10V5_10V5);
    mixed.analogOutputMode(AOUT_2, AOUT_MODE_M10V5_10V5);
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
    //Maximum output value is 10.4V
    if (counter >= 10.5)
    {
        counter = 0;
        //Additional 100 ms delay introduced to manage 10.5V -> 0V fall time of 150 ms
        delay(100);
    }
    delay(100);
}