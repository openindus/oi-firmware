#include "OpenIndus.h"
#include "Arduino.h"
#include "RS.h"

void setup()
{
    RS232.begin();
}

void loop()
{
    char data[100];
    size_t size = RS232.available();

    if (size)
    {
        RS232.read(data, size);
        printf("%s\n", data);
    }
    delay(10);
}