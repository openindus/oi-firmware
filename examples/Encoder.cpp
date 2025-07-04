/**
 * @file Encoder.cpp
 * @brief Encoder example using OpenIndus API
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "OpenIndus.h"
#include "Arduino.h"

OIStepper stepper;

void setup(void)
{
    printf("Hello OpenIndus!\n");
    stepper.encoder[0]->begin(DIN_1, DIN_2, 1024);
}

void loop(void)
{
    delay(1000);
    printf("Revolutions : %d | Pulses : %d | Angle : %.2frad | Speed : %.2fp/s\n", 
        stepper.encoder[0]->getRevolutions(),
        stepper.encoder[0]->getPulses(),
        stepper.encoder[0]->getAngle(),
        stepper.encoder[0]->getSpeed());
}