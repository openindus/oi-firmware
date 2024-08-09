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
    if (core.rs.available()) {
        printf("%d\n", core.rs.read());
    }
    
    delay(10);
}