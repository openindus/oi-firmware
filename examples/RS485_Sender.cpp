#include "OpenIndus.h"
#include "Arduino.h"
#include "RS.h"

void setup()
{
    RS485.begin();
}

void loop()
{
    RS485.write("Hello OpenIndus\r\n");
    delay(10);
}