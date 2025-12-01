#include "OpenIndus.h"
#include "Arduino.h"

OICore core;

void setup(void)
{
    Serial.begin(115200);
    
    /* Initialize RS485 */
    core.rs.begin(9600, RS::Config::_8N1, RS::Mode::RS485);
}

void loop(void)
{
    uint8_t data[64] = {0};
    size_t size = 0;
    
    size = core.rs.available(); // Get number of available bytes
    if (size) {
        core.rs.read(data, size); // Read available data
        Serial.printf("%s\n", data);
    }

    core.rs.write(data, size); // Echo back the received data

    delay(10);
}