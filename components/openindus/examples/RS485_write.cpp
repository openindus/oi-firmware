#include "OpenIndus.h"
#include "Arduino.h"

Core core;

void setup(void)
{
    core.rs.begin(115200, RS::Config::_8N1, RS::Mode::RS485);
}

void loop(void)
{
    static int counter = 0;

    core.rs.write(counter);
    counter++;
    
    delay(1000);
}