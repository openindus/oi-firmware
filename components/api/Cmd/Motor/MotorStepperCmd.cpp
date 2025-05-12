/**
 * @file MotorStepperCmd.cpp
 * @brief Stepper motor command functions
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#if defined(MODULE_MASTER)

#include "MotorStepperCmd.h"
#include "MotorStepperCmdArgs.h"

using namespace MotorStepperCmdArgs;

MotorStepperCmd::MotorStepperCmd(ModuleControl* module) : _module(module)
{
    /* Create the queue for wait function and add callback for CAN event */
    _motorWaitEvent[MOTOR_1] = xQueueCreate(1, 0);
    _motorWaitEvent[MOTOR_2] = xQueueCreate(1, 0);
}

void MotorStepperCmd::attachLimitSwitch(MotorNum_t motor, DIn_Num_t din, Logic_t logic)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_ATTACH_LIMIT_SWITCH, (uint8_t)motor, (uint8_t)din, (uint8_t)logic};
    _module->runCallback(msgBytes);
}

void MotorStepperCmd::detachLimitSwitch(MotorNum_t motor, DIn_Num_t din)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_DETACH_LIMIT_SWITCH, (uint8_t)motor, (uint8_t)din};
    _module->runCallback(msgBytes);
}

void MotorStepperCmd::setStepResolution(MotorNum_t motor, MotorStepResolution_t res)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_SET_STEP_RESOLUTION, (uint8_t)motor, (uint8_t)res};
    _module->runCallback(msgBytes);
}

void MotorStepperCmd::setAcceleration(MotorNum_t motor, float acc)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_SET_ACCELERATION, (uint8_t)motor};
    uint8_t *ptr                  = reinterpret_cast<uint8_t *>(&acc);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    _module->runCallback(msgBytes);
}

void MotorStepperCmd::setDeceleration(MotorNum_t motor, float dec)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_SET_ACCELERATION, (uint8_t)motor};
    uint8_t *ptr                  = reinterpret_cast<uint8_t *>(&dec);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    _module->runCallback(msgBytes);
}

void MotorStepperCmd::setMaxSpeed(MotorNum_t motor, float speed)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_SET_MAX_SPEED, (uint8_t)motor};
    uint8_t *ptr                  = reinterpret_cast<uint8_t *>(&speed);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    _module->runCallback(msgBytes);
}

void MotorStepperCmd::setMinSpeed(MotorNum_t motor, float speed)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_SET_MIN_SPEED, (uint8_t)motor};
    uint8_t *ptr                  = reinterpret_cast<uint8_t *>(&speed);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    _module->runCallback(msgBytes);
}

void MotorStepperCmd::setFullStepSpeed(MotorNum_t motor, float speed)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_SET_FULL_STEP_SPEED, (uint8_t)motor};
    uint8_t *ptr                  = reinterpret_cast<uint8_t *>(&speed);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    _module->runCallback(msgBytes);
}

int32_t MotorStepperCmd::getPosition(MotorNum_t motor)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_GET_POSITION, (uint8_t)motor};
    _module->runCallback(msgBytes);
    int32_t *position = reinterpret_cast<int32_t *>(&msgBytes[2]);
    return *position;
}

float MotorStepperCmd::getSpeed(MotorNum_t motor)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_GET_SPEED, (uint8_t)motor};
    _module->runCallback(msgBytes);
    float *speed = reinterpret_cast<float *>(&msgBytes[2]);
    return *speed;
}

void MotorStepperCmd::resetHomePosition(MotorNum_t motor)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_RESET_HOME_POSITION, (uint8_t)motor};
    _module->runCallback(msgBytes);
}

void MotorStepperCmd::setPosition(MotorNum_t motor, int32_t position)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_SET_POSITION, (uint8_t)motor};
    uint8_t *ptr                  = reinterpret_cast<uint8_t *>(&position);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(int32_t));
    _module->runCallback(msgBytes);
}

void MotorStepperCmd::stop(MotorNum_t motor, MotorStopMode_t mode)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_STOP, (uint8_t)motor, (uint8_t)mode};
    _module->runCallback(msgBytes);
}

void MotorStepperCmd::moveAbsolute(MotorNum_t motor, uint32_t position, bool microStep)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_MOVE_ABSOLUTE, (uint8_t)motor};
    uint8_t *ptr                  = reinterpret_cast<uint8_t *>(&position);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(uint32_t));
    msgBytes.push_back((uint8_t)microStep);
    _module->runCallback(msgBytes);
}

void MotorStepperCmd::moveRelative(MotorNum_t motor, int32_t position, bool microStep)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_MOVE_RELATIVE, (uint8_t)motor};
    uint8_t *ptr                  = reinterpret_cast<uint8_t *>(&position);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(int32_t));
    msgBytes.push_back((uint8_t)microStep);
    _module->runCallback(msgBytes);
}

void MotorStepperCmd::run(MotorNum_t motor, MotorDirection_t direction, float speed)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_RUN, (uint8_t)motor, (uint8_t)direction};
    uint8_t *ptr                  = reinterpret_cast<uint8_t *>(&speed);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    _module->runCallback(msgBytes);
}

void MotorStepperCmd::wait(MotorNum_t motor)
{
    // Add callback (callback is rewrite at each call of wait function)
    Master::addEventCallback(
        EVENT_MOTOR_READY, _module->getId(),
        [this](uint8_t *data) { xQueueSend(_motorWaitEvent[data[1]], NULL, pdMS_TO_TICKS(100)); });

    // Send a message to slave
    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_WAIT, (uint8_t)motor};
    _module->runCallback(msgBytes, false);
    // Wait for event
    xQueueReset(_motorWaitEvent[motor]);
    xQueueReceive(_motorWaitEvent[motor], NULL, portMAX_DELAY);
}

void MotorStepperCmd::homing(MotorNum_t motor, float speed)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_HOMING, (uint8_t)motor};
    uint8_t *ptr                  = reinterpret_cast<uint8_t *>(&speed);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    _module->runCallback(msgBytes);
}

int MotorStepperCmd::setAdvancedParam(MotorNum_t motor, AdvancedParameter_t advParam, void* value)
{
    std::vector<uint8_t> args = SetAdvancedParamArgs_s{motor, advParam, value};
    return _module->runCallback(CALLBACK_MOTOR_SET_ADVANCED_PARAM, args);
}

int MotorStepperCmd::getAdvancedParam(MotorNum_t motor, AdvancedParameter_t advParam, void* value)
{
    std::vector<uint8_t> args = GetAdvancedParamArgs_s{motor, advParam, value};
    int ret = _module->runCallback(CALLBACK_MOTOR_GET_ADVANCED_PARAM, args);
    if (ret == 0) {
        std::memcpy(value, &args[2], sizeof(void*));
    }
    return ret;
}

float MotorStepperCmd::getSupplyVoltage(void)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_GET_SUPPLY_VOLTAGE};
    _module->runCallback(msgBytes);
    float *voltage = reinterpret_cast<float *>(&msgBytes[1]);
    return *voltage;
}

void MotorStepperCmd::attachFlagInterrupt(void(*callback)(MotorNum_t, MotoStepperFlag_t))
{
    _flagIsrCallback = callback;
    Master::addEventCallback(
        EVENT_MOTOR_FLAG_INTERRUPT, _module->getId(), [this](uint8_t *data) { 
            _flagIsrCallback((MotorNum_t)data[1], (MotoStepperFlag_t)data[2]); 
        });

    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_ATTACH_FLAG_INTERRUPT};
    _module->runCallback(msgBytes);
}

void MotorStepperCmd::detachFlagInterrupt(void)
{
    _flagIsrCallback = nullptr;
    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_DETACH_FLAG_INTERRUPT};
    _module->runCallback(msgBytes);
}

#endif