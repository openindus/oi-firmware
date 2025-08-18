#include "OpenIndus.h"
#include "Arduino.h"

Core core;

void setup(void)
{
    core.rs.begin(115200);
}

void loop(void)
{
    if (core.rs.available()) {
        printf("%d\n", core.rs.read());
    }
    
    delay(10);
}