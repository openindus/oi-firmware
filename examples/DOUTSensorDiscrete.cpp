#include "OpenIndus.h"
#include "Arduino.h"

/* Instanciate */
Discrete discrete;

void setup(void)
{
    /* Set DOUT 1 to high */
    discrete.digitalWrite(DOUT_1, 1);
    discrete.digitalWrite(DOUT_2, 1);
    discrete.digitalWrite(DOUT_3, 1);
    discrete.digitalWrite(DOUT_4, 1);
    discrete.digitalWrite(DOUT_5, 1);
    discrete.digitalWrite(DOUT_6, 1);
    discrete.digitalWrite(DOUT_7, 1);
    discrete.digitalWrite(DOUT_8, 1);
}

void loop(void)
{
    /* Print value in monitor */
    printf("DOUT Sensor 1: %.2fA\t 2: %.2fA\t 3: %.2fA\t 4: %.2fA\t 5: %.2fA\t 6: %.2fA\t 7: %.2fA\t 8: %.2fA\n", \
        discrete.getCurrent(DOUT_1),\
        discrete.getCurrent(DOUT_2),\
        discrete.getCurrent(DOUT_3),\
        discrete.getCurrent(DOUT_4),\
        discrete.getCurrent(DOUT_5),\
        discrete.getCurrent(DOUT_6),\
        discrete.getCurrent(DOUT_7),\
        discrete.getCurrent(DOUT_8));

    /* Wait for 200ms before reading value again */
    delay(200);
}