#include "OpenIndus.h"
#include "Arduino.h"

void setup()
{
    RS485.begin();
}

void loop()
{
    RS485.write("Hello OpenIndus\n");
    delay(1000);
}