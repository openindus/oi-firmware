#include "OpenIndus.h"
#include "Arduino.h"

Core core;

void setup(void)
{
    core.rs.begin(115200, RS::Config::_8N1, RS::Mode::RS485);
}

void loop(void)
{
    if (core.rs.available()) {
        printf("%d\n", core.rs.read());
    }
    
    delay(10);
}