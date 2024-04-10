#include "OpenIndus.h"
#include "Arduino.h"

using namespace OI;

Core core;

int counter = 0;

void setup(void)
{
    Serial.begin(115200);

    /* Initialize RS. Mode: RS485, Baudrate 9600 */
    core.rs.begin(RS_485, 9600);
}

void loop(void)
{
    /* Read */
    if (core.rs.available()) {
        Serial.printf("%d\n", core.rs.read());
    }

    /* Write */
    if (core.rs.availableForWrite()) {
        core.rs.write(counter);
        counter++;
    }
    
    delay(1000);
}