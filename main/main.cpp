#include "OpenIndus.h"
#include "Arduino.h"

OIStepper stepper;

void printTask(void*)
{
    while (1) {    
        printf("position: %d | speed: %f\n", 
            stepper.getPosition(MOTOR_1), stepper.getSpeed(MOTOR_1));
        delay(100);
    }
}

void setup(void) 
{
   
}

void loop(void) 
{
    setup();
}