#include "OpenIndus.h"
#include "Arduino.h"

/* Instanciate */
OICore core;

void setup(void)
{
    Serial.begin(115200);
    
    /* Set DOUT 1 to high */
    core.digitalWrite(DOUT_1, 1);
    core.digitalWrite(DOUT_2, 1);
    core.digitalWrite(DOUT_3, 1);
    core.digitalWrite(DOUT_4, 1);
}

void loop(void)
{
    /* Print value in monitor */
    Serial.printf("DOUT Sensor 1: %u\t 2: %u\t 3: %u\t 4: %u\n", \
        core.outputIsOvercurrent(DOUT_1),\
        core.outputIsOvercurrent(DOUT_2),\
        core.outputIsOvercurrent(DOUT_3),\
        core.outputIsOvercurrent(DOUT_4));

    /* Wait for 200ms before reading value again */
    delay(200);
}