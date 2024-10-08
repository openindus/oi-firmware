#include "OpenIndus.h"
#include "Arduino.h"

OIStepper stepper;

void printTask(void*)
{
    while (1) {    
        Serial.printf("position: %d | speed: %f\n", 
            stepper.getPosition(MOTOR_1), stepper.getSpeed(MOTOR_1));
        delay(100);
    }
}

void setup(void) 
{
    Serial.begin(115200);

    /* Print task */
    xTaskCreatePinnedToCore(printTask, "Task to print parameters", 10000, NULL, 1, NULL, 0);

    /* Settings */
    stepper.setMaxSpeed(MOTOR_1, 1000);
    stepper.resetHomePosition(MOTOR_1);

    delay(1000);

    /* Homing */
    Serial.printf("Homing\n");
    stepper.attachLimitSwitch(MOTOR_1, DIN_1, ACTIVE_HIGH);
    stepper.homing(MOTOR_1, 800);
    stepper.wait(MOTOR_1);
    Serial.printf("position: %d\n", stepper.getPosition(MOTOR_1));

    /* Move relative */
    Serial.printf("moveRelative\n");
    stepper.moveRelative(MOTOR_1, 100);
    stepper.wait(MOTOR_1);
    Serial.printf("position: %d\n", stepper.getPosition(MOTOR_1));

    /* Move relative */
    Serial.printf("moveRelative\n");
    stepper.moveRelative(MOTOR_1, -200);
    stepper.wait(MOTOR_1);
    Serial.printf("position: %d\n", stepper.getPosition(MOTOR_1));

    /* Move absolute */
    Serial.printf("moveAbsolute\n");
    stepper.moveAbsolute(MOTOR_1, 200);
    stepper.wait(MOTOR_1);
    Serial.printf("position: %d\n", stepper.getPosition(MOTOR_1));
}

void loop(void) 
{
    delay(1000);
}