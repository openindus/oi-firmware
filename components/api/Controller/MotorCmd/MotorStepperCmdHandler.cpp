/**
 * @file MotorStepperCmdHandler.cpp
 * @brief Stepper motor command handler
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#if defined(MODULE_SLAVE)

#include "MotorStepperCmdHandler.h"

MotorNum_t MotorStepperCmdHandler::_motorNums[MOTOR_MAX] = {MOTOR_1, MOTOR_2};

void MotorStepperCmdHandler::_waitTask(void *pvParameters)
{
    MotorNum_t motor = *(MotorNum_t *)pvParameters;    
    MotorStepper::wait(motor); // Wait motor to be ready
    Slave::sendEvent({EVENT_MOTOR_READY, (uint8_t)motor}); // Send a CAN event to master
    
    vTaskDelete(NULL); // Delete task
}

#endif