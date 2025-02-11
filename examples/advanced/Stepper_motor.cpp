#include "OpenIndus.h"
#include "Arduino.h"

Stepper stepper;

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
    /* Print task */
    xTaskCreatePinnedToCore(printTask, "Task to print parameters", 10000, NULL, 1, NULL, 0);

    /* Settings */
    stepper.setMaxSpeed(MOTOR_1, 1000);
    stepper.resetHomePosition(MOTOR_1);

    delay(1000);

    /* Homing */
    printf("Homing\n");
    stepper.attachLimitSwitch(MOTOR_1, DIN_1, ACTIVE_HIGH);
    stepper.homing(MOTOR_1, 800);
    stepper.wait(MOTOR_1);
    printf("position: %d\n", stepper.getPosition(MOTOR_1));

    /* Move relative */
    printf("moveRelative\n");
    stepper.moveRelative(MOTOR_1, 100);
    stepper.wait(MOTOR_1);
    printf("position: %d\n", stepper.getPosition(MOTOR_1));

    /* Move relative */
    printf("moveRelative\n");
    stepper.moveRelative(MOTOR_1, -200);
    stepper.wait(MOTOR_1);
    printf("position: %d\n", stepper.getPosition(MOTOR_1));

    /* Move absolute */
    printf("moveAbsolute\n");
    stepper.moveAbsolute(MOTOR_1, 200);
    stepper.wait(MOTOR_1);
    printf("position: %d\n", stepper.getPosition(MOTOR_1));
}

void loop(void) 
{
    delay(1000);
}