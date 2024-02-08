#include "OpenIndus.h"
#include "Arduino.h"

#include "CorePinout.h"

/* Instanciate */
OICore core;
int dout_sensor_value;

void setup(void)
{
    /* Set DOUT 1 to high */
    core.digitalWrite(DOUT_1, 1);
}

void loop(void)
{
    /* Work only if code is modified to give full access to _ioex in CoreStandalone Class */
    dout_sensor_value = ioex_get_level(core._ioex, CORE_IOEX_PIN_DOUT_CURRENT_1);

    /* Print value in monitor */
    printf("DOUT 1 Sensor: %i\n", dout_sensor_value);

    /* Wait for 200ms before reading value again */
    delay(200);
}