#include "OpenIndus.h"
#include "Arduino.h"

using namespace OI;

OICore core;

void setup(void)
{
    Serial.begin(115200);

    /* Initialize RS. Mode: RS485, Baudrate 9600 */
    core.rs.begin(RS_485, 9600);
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