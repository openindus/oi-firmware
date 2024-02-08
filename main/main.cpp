#include "OpenIndus.h"
#include "Arduino.h"

Stepper s1;
Stepper s2;
Stepper s3;

void setup(void)
{
    printf("Hello World !\n");
}

void loop(void)
{
    s1.ledOn(LED_PURPLE);
    s2.ledOn(LED_CYAN);
    s3.ledOn(LED_YELLOW);
    delay(1000);
    s1.ledOff();
    s2.ledOff();
    s3.ledOff();
    delay(1000);
}