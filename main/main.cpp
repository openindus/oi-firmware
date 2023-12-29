#include "OpenIndus.h"
#include "Arduino.h"
#include "RS.h"

void setup()
{
    RS485.begin();
}

void loop()
{
    int data;

    if (RS485.available())
    {
        data = RS485.read();
        printf("%c\n", data);
    }
    delay(10);
}

