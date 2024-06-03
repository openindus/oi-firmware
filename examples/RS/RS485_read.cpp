#include "OpenIndus.h"
#include "Arduino.h"

OICore core;

void setup(void)
{
    Serial.begin(115200);

    /* Initialize RS. Mode: RS485, Baudrate 9600 */
    core.rs.begin(RS_485, 9600);
}

void loop(void)
{
    if (core.rs.available()) {
        Serial.printf("%d\n", core.rs.read());
    }
    
    delay(10);
}