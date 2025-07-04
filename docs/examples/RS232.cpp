#include "OpenIndus.h"
#include "Arduino.h"

OICore core;

void setup(void)
{
    Serial.begin(115200);

    /* Init RS */
    core.rs.begin(9600, 0x001c, RS_232);
}

void loop(void)
{
    char data[100];
    size_t size = core.rs.available();

    if (size) {
        core.rs.read(data, size);
        Serial.printf("%s\n", data);
    }

    core.rs.write("Hello OpenIndus\r\n");
    delay(10);
}