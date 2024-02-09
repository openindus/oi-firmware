#include "OpenIndus.h"
#include "Arduino.h"

/* Instanciate */
Core core;
int din_value[4];

void setup(void)
{
    /* Nothing to do in setup */
}

void loop(void)
{
    /* Read value from DIN 1 to 4 */
    din_value[0] = core.digitalRead(DIN_1);
    din_value[1] = core.digitalRead(DIN_2);
    din_value[2] = core.digitalRead(DIN_3);
    din_value[3] = core.digitalRead(DIN_4);

    /* Print value in monitor */
    printf("DIN_1: %i | DIN_2: %i | DIN_3: %i | DIN_4: %i\n", din_value[0], din_value[1], din_value[2], din_value[3]);

    /* Wait for 200ms before reading value again */
    delay(200);
}