#include "OpenIndus.h"
#include "Arduino.h"

using namespace OI;

Core core;

void setup()
{
    core.rs.begin(RS_232);
}

void loop()
{
    core.rs.write("Hello OpenIndus\r\n");
    delay(10);
}