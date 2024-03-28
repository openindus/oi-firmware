#include "OpenIndus.h"
#include "Arduino.h"

/* Instanciate */
Discrete discrete;

DigitalInputNum_t din[] = {DIN_1, DIN_2, DIN_3, DIN_4, DIN_5, DIN_6, DIN_7, DIN_8, DIN_9, DIN_10};

void _callback(void* pvParameters)
{
    DigitalInputNum_t din = *((DigitalInputNum_t*) pvParameters);
    Serial.printf("\nInterrupt on DIN_%i !\n\n", din+1);
    /* You call put a delay in this function because it is not an isr, isr is handled at lower level */
    delay(100);
}

void setup(void)
{
    Serial.begin(115200);
    for (int i = 0; i < DIN_MAX; i++)
    {
        /* Read value and Print value */
        Serial.printf("DIN_%i: %i | ", i+1, discrete.digitalRead((DigitalInputNum_t)i));
        /* Attach interrupt*/
        discrete.attachInterrupt((DigitalInputNum_t)i, _callback, RISING_MODE, &(din[i]));
    }
}

void loop(void)
{
    delay(1000);
}