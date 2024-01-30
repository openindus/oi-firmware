#include "OpenIndus.h"
#include "Arduino.h"
#include "RS.h"

void setup()
{
    RS485.begin();
}

void loop()
{
    char data[100];
    size_t size = RS485.available();

    if (size)
    {
        RS485.read(data, size);
        printf("%s\n", data);
    }
    delay(10);
}