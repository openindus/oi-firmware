#include "OpenIndus.h"
#include "Arduino.h"

Core core;

void setup(void)
{
    core.rs.begin(115200);
}

void loop(void)
{
    static int counter = 0;

    if (core.rs.availableForWrite()) {
        core.rs.write(counter);
        counter++;
    }
    
    delay(1000);
}