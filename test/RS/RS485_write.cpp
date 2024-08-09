#include "OpenIndus.h"
#include "Arduino.h"

Core core;

void setup(void)
{
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