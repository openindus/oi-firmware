#include "OpenIndus.h"
#include "Arduino.h"

/* Instanciate */
OIDiscrete discrete;

void setup(void)
{
    Serial.begin(115200);
    
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
    Serial.printf("DOUT Sensor 1: %.2fA\t 2: %.2fA\t 3: %.2fA\t 4: %.2fA\t 5: %.2fA\t 6: %.2fA\t 7: %.2fA\t 8: %.2fA\n", \
        discrete.getOutputCurrent(DOUT_1),\
        discrete.getOutputCurrent(DOUT_2),\
        discrete.getOutputCurrent(DOUT_3),\
        discrete.getOutputCurrent(DOUT_4),\
        discrete.getOutputCurrent(DOUT_5),\
        discrete.getOutputCurrent(DOUT_6),\
        discrete.getOutputCurrent(DOUT_7),\
        discrete.getOutputCurrent(DOUT_8));

    /* Wait for 200ms before reading value again */
    delay(200);
}