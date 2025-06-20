/**
 * @file DigitalOutputsCmd.h
 * @brief Digital Outputs commands
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#if defined(MODULE_MASTER)

#include "Master.h"
#include "Slave.h"
#include "ModuleControl.h"
#include "DigitalOutputsInterface.h"

class DigitalOutputsCmd : public DigitalOutputsInterface
{
public:
    DigitalOutputsCmd(ModuleControl *module) : _module(module), _overcurrentCallback(NULL) {}
    DigitalOutputsCmd(uint16_t id) : _module(new ModuleControl(id)), _overcurrentCallback(NULL) {}
    ~DigitalOutputsCmd() { delete _module; }

    void digitalWrite(DOut_Num_t num, bool level) override;
    void toggleOutput(DOut_Num_t num) override;
    void outputMode(DOut_Num_t num, DOut_Mode_t mode) override;
    void setPWMFrequency(DOut_Num_t num, uint32_t freq) override;
    void setPWMDutyCycle(DOut_Num_t num, float duty) override;
    float getOutputCurrent(DOut_Num_t num) override;
    int outputIsOvercurrent(DOut_Num_t num) override;
    void setOvercurrentThreshold(float threshold, float thresholdSum = 8.0f);
    void attachOvercurrentCallback(void (*callback)(void*), void *arg = NULL) override;
    void detachOvercurrentCallback(void) override;

private:
    ModuleControl *_module;
    void (*_overcurrentCallback)(void*);

protected:
    friend class Core;
};

#endif