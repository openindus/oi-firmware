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
<<<<<<< HEAD
    setup();
=======
    
    delay(1000);

=======
>>>>>>> bdcc50d... fix ps01 driver for good git status
    /* Homing */
    stepper.homing(MOTOR_1, 1000);
    stepper.homing(MOTOR_2, 1000);
    stepper.wait(MOTOR_1);
    stepper.wait(MOTOR_2);

<<<<<<< HEAD
    stepper.detachLimitSwitch(MOTOR_1, DIN_1);
    stepper.detachLimitSwitch(MOTOR_2, DIN_4);

    stepper.moveAbsolute(MOTOR_1, 50);
    stepper.moveAbsolute(MOTOR_2, 50);


=======
    stepper.moveAbsolute(MOTOR_1, 100);
    stepper.wait(MOTOR_1);

    stepper.moveAbsolute(MOTOR_1, 150);
    stepper.wait(MOTOR_1);

    stepper.moveAbsolute(MOTOR_2, 250);
    stepper.wait(MOTOR_2);

    // printf("status:%x\n", PS01_Cmd_GetStatus(MOTOR_2));
}
>>>>>>> bdcc50d... fix ps01 driver for good git status
