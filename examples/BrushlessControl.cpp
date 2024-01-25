#include "OpenIndus.h"
#include "Arduino.h"

OICore core;
OIBrushless brushless;

float speed;

void setup(void)
{
    brushless.setDirection(1);
    brushless.setBrake(0);
}

void loop(void)
{  
    brushless.setSpeed(150);
    speed = brushless.getSpeed();
    printf("Motor speed : %f \n", speed);
    delay(1000);
}