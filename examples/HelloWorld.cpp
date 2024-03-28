#include "OpenIndus.h"
#include "Arduino.h"

void setup(void)
{
    Serial.begin(115200);
    Serial.print("Hello World !\n");
}

void loop(void)
{
    delay(1000);
}