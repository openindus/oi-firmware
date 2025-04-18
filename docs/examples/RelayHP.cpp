#include "OpenIndus.h"
#include "Arduino.h"
#include "RelayHP.h"

OIRelayHP relay;

void setup(void)
{
    /* Nothing to do in setup */
}

void loop(void)
{
    /* Activate all */
    relay.digitalWrite(RELAY_1, HIGH);
    delay(200);
    relay.digitalWrite(RELAY_2, HIGH);
    delay(200);
    relay.digitalWrite(RELAY_3, HIGH);
    delay(200);
    relay.digitalWrite(RELAY_4, HIGH);
    delay(1000);

    /* Deactivate all */
    relay.digitalWrite(RELAY_1, LOW);
    delay(200);
    relay.digitalWrite(RELAY_2, LOW);
    delay(200);
    relay.digitalWrite(RELAY_3, LOW);
    delay(200);
    relay.digitalWrite(RELAY_4, LOW);
    delay(5000);

}