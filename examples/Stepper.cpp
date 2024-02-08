#include "OpenIndus.h"
#include "Arduino.h"

OICore core;
OIStepper stepper;

void printTask(void *pvParameters)
{
    while (1)
    {    
        printf("position: %d | speed: %f\n", stepper.getPosition(MOTOR_1), stepper.getSpeed(MOTOR_1));
        delay(100);
    }
}

void setup() 
{
    /* Print task */
    xTaskCreatePinnedToCore(printTask, "Task to print parameters", 10000, NULL, 1, NULL, 0);

    /* Settings */
    stepper.setMaxSpeed(MOTOR_1, 1000);
    stepper.resetHomePosition(MOTOR_1);

    delay(1000);

    /* Homing */
    printf("Homing\n");
    stepper.setLimitSwitch(MOTOR_1, DIN_1, ACTIVE_HIGH);
    stepper.homing(MOTOR_1, 800);
    delay(3000);
    printf("position: %d\n", stepper.getPosition(MOTOR_1));

    /* Move relative */
    printf("moveRelative\n");
    stepper.moveRelative(MOTOR_1, 100);
    delay(1000);
    printf("position: %d\n", stepper.getPosition(MOTOR_1));

    /* Move relative */
    printf("moveRelative\n");
    stepper.moveRelative(MOTOR_1, -200);
    delay(1000);
    printf("position: %d\n", stepper.getPosition(MOTOR_1));

    /* Move absolute */
    printf("moveAbsolute\n");
    stepper.moveAbsolute(MOTOR_1, 200);
    delay(1000);
    printf("position: %d\n", stepper.getPosition(MOTOR_1));
}

void loop() 
{
    delay(1000);
}