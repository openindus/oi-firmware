#include "OpenIndus.h"
#include "Arduino.h"

Analogls analogls;

void setup(void)
{
    Serial.begin(115200);
    Serial.print("PT100 sensor example");
}

void loop(void)
{   
    delay(1000);
}