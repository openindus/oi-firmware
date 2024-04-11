#include "OpenIndus.h"
#include "Arduino.h"

OIRelay relay;

void setup(void)
{
    /* Nothing to do in setup */
}

void loop(void)
{
    /* Activate all */
    relay.digitalWrite(RELAY_1, HIGH);
    relay.digitalWrite(RELAY_2, HIGH);
    relay.digitalWrite(RELAY_3, HIGH);
    relay.digitalWrite(RELAY_4, HIGH);
    relay.digitalWrite(RELAY_5, HIGH);
    relay.digitalWrite(RELAY_6, HIGH);
    delay(1000);

    /* Deactivate all */
    relay.digitalWrite(RELAY_1, LOW);
    relay.digitalWrite(RELAY_2, LOW);
    relay.digitalWrite(RELAY_3, LOW);
    relay.digitalWrite(RELAY_4, LOW);
    relay.digitalWrite(RELAY_5, LOW);
    relay.digitalWrite(RELAY_6, LOW);
    delay(1000);
}