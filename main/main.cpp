#include "OpenIndus.h"
#include "Arduino.h"

void setup(void)
{
    Serial.begin(115200);
    Serial.println("Hello World !");
}

void loop(void)
{
    delay(1000);
}