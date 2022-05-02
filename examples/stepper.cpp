#include "OpenIndus.h"
#include "Arduino.h"

// void printTask(void *pvParameters)
// {
//     while (1)
//     {    
//         printf("***********************************************\n");
//         printf("position: %d\n", Stepper.getPosition(MOTOR1));
//         printf("mark: %d\n", Stepper.getMark(MOTOR1));
//         printf("speed: %f\n", Stepper.getSpeed(MOTOR1));
//         printf("acceleration: %f\n", Stepper.getAcceleration(MOTOR1));
//         printf("deceleration: %f\n", Stepper.getDeceleration(MOTOR1));
//         printf("max speed: %f\n", Stepper.getMaxSpeed(MOTOR1));
//         printf("min speed: %f\n", Stepper.getMinSpeed(MOTOR1));
//         printf("Fs speed: %f\n", Stepper.getFsSpeed(MOTOR1));
//         printf("KVal run: %f\n", Stepper.getKValRun(MOTOR1));
//         printf("KVal hold: %f\n", Stepper.getKValHold(MOTOR1));
//         printf("KVal acc: %f\n", Stepper.getKValAcceleration(MOTOR1));
//         printf("KVal dec: %f\n", Stepper.getKValDeceleration(MOTOR1));
//         printf("\n");
//         delay(100);
//     }
// }

void setup() 
{
    // /* Configuration */
    // Stepper.setHome(MOTOR1, 1000);
    // Stepper.setMark(MOTOR1, 9000);
    // Stepper.setAcceleration(MOTOR1, 1500);
    // Stepper.setDeceleration(MOTOR1, 1500);
    // Stepper.setMaxSpeed(MOTOR1, 900);
    // Stepper.setMinSpeed(MOTOR1, 100);
    // Stepper.setFsSpeed(MOTOR1, 700);
    // Stepper.setKValHold(MOTOR1, 15);
    // Stepper.setKValRun(MOTOR1, 15);
    // Stepper.setKValAcceleration(MOTOR1, 15);
    // Stepper.setKValDeceleration(MOTOR1, 15);

    // /* Print task */
    // xTaskCreatePinnedToCore(printTask, "Task to print parameters", 10000, NULL, 1, NULL, 0);
}

void loop() 
{
    // /* Commands */
    // Stepper.run(MOTOR1, FORWARD, 500);
    // delay(1000);
    // Stepper.softHiZ(MOTOR1);
    // delay(1000);
    // Stepper.stepClock(MOTOR1, FORWARD, 10000);
    // delay(1000);
    // Stepper.softHiZ(MOTOR1);
    // delay(1000);
    // Stepper.move(MOTOR1, FORWARD, 10000);
    // while(Stepper.busyCheck(MOTOR1)) delay(100);
    // delay(1000);
    // Stepper.move(MOTOR1, BACKWARD, 10000);
    // while(Stepper.busyCheck(MOTOR1)) delay(100);
    // delay(1000);
    // Stepper.goTo(MOTOR1, 3000);
    // while(Stepper.busyCheck(MOTOR1)) delay(10);
    // delay(1000);
    // Stepper.goHome(MOTOR1);
    // delay(1000);
    // Stepper.goMark(MOTOR1);
    // delay(1000);
}