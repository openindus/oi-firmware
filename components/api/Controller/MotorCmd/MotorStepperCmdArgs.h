/**
 * @file MotorStepperCmdArgs.h
 * @brief Motor stepper command arguments
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Common.h"
#include "ControllerTypes.h"
#include "MotorStepper.h"
#include "MotorStepperParam.h"

namespace MotorStepperCmdArgs
{
struct SetAdvancedParamArgs_s {
    MotorNum_t motor;
    AdvancedParameter_t advParam;
    void *value;

    operator std::vector<uint8_t>() const {
        std::vector<uint8_t> bytes = {(uint8_t)motor, (uint8_t)advParam};
        uint8_t *ptr               = reinterpret_cast<uint8_t *>(value);
        bytes.insert(bytes.end(), ptr, ptr + sizeof(value));
        return bytes;
    }
};

struct GetAdvancedParamArgs_s {
    MotorNum_t motor;
    AdvancedParameter_t advParam;
    void *value;

    operator std::vector<uint8_t>() const {
        std::vector<uint8_t> bytes = {(uint8_t)motor, (uint8_t)advParam};
        uint8_t *ptr               = reinterpret_cast<uint8_t *>(value);
        bytes.insert(bytes.end(), ptr, ptr + sizeof(value));
        return bytes;
    }
};
} // namespace MotorStepperCmdArgs