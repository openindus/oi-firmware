#include "OpenIndus.h"
#include "Arduino.h"

OIRelay relay;

void setup(void)
{

}

void loop(void)
{
    relay.digitalWrite(RELAY_1, HIGH);
    delay(1000);
    relay.digitalWrite(RELAY_1, LOW);
    delay(1000);
}