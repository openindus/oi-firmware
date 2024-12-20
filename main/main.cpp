#include "OpenIndus.h"
#include "Arduino.h"

void setup(void)
{
    Serial.begin(115200);
    Serial.println("Hello Openindus !");
}

void loop(void)
{
    delay(1000);
}