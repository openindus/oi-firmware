#include "OpenIndus.h"
#include "Arduino.h"

/* Instanciate */
OICore core;

void setup(void)
{
    /* Nothing to do in setup */
}

void loop(void)
{
    /* Activate all */
    core.digitalWrite(DOUT_1, HIGH);
    core.digitalWrite(DOUT_2, HIGH);
    core.digitalWrite(DOUT_3, HIGH);
    core.digitalWrite(DOUT_4, HIGH);
    delay(1000);

    /* Deactivate all */
    core.digitalWrite(DOUT_1, LOW);
    core.digitalWrite(DOUT_2, LOW);
    core.digitalWrite(DOUT_3, LOW);
    core.digitalWrite(DOUT_4, LOW);
    delay(1000);
}