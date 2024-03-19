#include "OpenIndus.h"
#include "Arduino.h"

using namespace OI;

Core core;

void setup()
{
    core.rs.begin(RS_485);
}

void loop()
{
    char data[100];
    size_t size = core.rs.available();

    if (size)
    {
        core.rs.read(data, size);
        printf("%s\n", data);
    }
    delay(10);
}