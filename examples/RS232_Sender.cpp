#include "OpenIndus.h"
#include "Arduino.h"
#include "RS.h"

void setup()
{
    RS232.begin();
}

void loop()
{
    RS232.write("Hello OpenIndus\r\n");
    delay(10);
}