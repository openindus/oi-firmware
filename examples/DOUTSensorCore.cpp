#include "OpenIndus.h"
#include "Arduino.h"

#include "CorePinout.h"

/* Instanciate */
Core core;

void setup(void)
{
    /* Set DOUT 1 to high */
    core.digitalWrite(DOUT_1, 1);
    core.digitalWrite(DOUT_2, 1);
    core.digitalWrite(DOUT_3, 1);
    core.digitalWrite(DOUT_4, 1);
}

void loop(void)
{
    /* Print value in monitor */
    printf("DOUT Sensor 1: %u\t 2: %u\t 3: %u\t 4: %u\n", \
        core.digitalReadOverCurrent(DOUT_1),\
        core.digitalReadOverCurrent(DOUT_2),\
        core.digitalReadOverCurrent(DOUT_3),\
        core.digitalReadOverCurrent(DOUT_4));

    /* Wait for 200ms before reading value again */
    delay(200);
}