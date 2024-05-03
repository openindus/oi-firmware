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
<<<<<<< HEAD
    setup();
=======
    
    delay(1000);

    /* Homing */
    printf("Homing\n");
    stepper.attachLimitSwitch(MOTOR_1, DIN_1, ACTIVE_HIGH);
    stepper.attachLimitSwitch(MOTOR_2, DIN_4, ACTIVE_HIGH);
    stepper.homing(MOTOR_1, 100);
    stepper.homing(MOTOR_2, 100);
    stepper.wait(MOTOR_1);
    stepper.wait(MOTOR_2);

    stepper.detachLimitSwitch(MOTOR_1, DIN_1);
    stepper.detachLimitSwitch(MOTOR_2, DIN_4);

    stepper.moveAbsolute(MOTOR_1, 50);
    stepper.moveAbsolute(MOTOR_2, 50);


