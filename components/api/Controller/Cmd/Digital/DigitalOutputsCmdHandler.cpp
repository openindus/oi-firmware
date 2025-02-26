/**
 * @file DigitalOutputsCmdHandler.cpp
 * @brief Digital outputs commands handler
 * @author 
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "DigitalOutputsCmdHandler.h"
#include "Cmd.h"

#if defined(MODULE_SLAVE)

int DigitalOutputsCmdHandler::init(void)
{
    SlaveController::addCmdHandler(REQUEST_DIGITAL_WRITE, [](std::vector<uint8_t> &data) {
        DigitalOutputs::digitalWrite((DOut_Num_t)data[1], (bool)data[2]);
        data.clear();
    });

    SlaveController::addCmdHandler(REQUEST_TOGGLE_OUTPUT, [](std::vector<uint8_t> &data) {
        DigitalOutputs::toggleOutput((DOut_Num_t)data[1]);
        data.clear();
    });

    SlaveController::addCmdHandler(REQUEST_OUTPUT_MODE, [](std::vector<uint8_t> &data) {
        DigitalOutputs::outputMode((DOut_Num_t)data[1], (DOut_Mode_t)data[2]);
        data.clear();
    });

    SlaveController::addCmdHandler(REQUEST_SET_PWM_FREQUENCY, [](std::vector<uint8_t> &data) {
        uint32_t *freq = reinterpret_cast<uint32_t *>(&data[2]);
        DigitalOutputs::setPWMFrequency((DOut_Num_t)data[1], *freq);
        data.clear();
    });

    SlaveController::addCmdHandler(REQUEST_SET_PWM_DUTY_CYCLE, [](std::vector<uint8_t> &data) {
        uint32_t *duty = reinterpret_cast<uint32_t *>(&data[2]);
        DigitalOutputs::setPWMDutyCycle((DOut_Num_t)data[1], *duty);
        data.clear();
    });

    SlaveController::addCmdHandler(REQUEST_GET_OUTPUT_CURRENT, [](std::vector<uint8_t> &data) {
        float current = DigitalOutputs::getOutputCurrent((DOut_Num_t)data[1]);
        uint8_t *ptr  = reinterpret_cast<uint8_t *>(&current);
        data.insert(data.end(), ptr, ptr + sizeof(float));
    });

    return 0;
}

#endif