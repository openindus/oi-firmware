/**
 * @file DigitalOutputsCmd.h
 * @brief Digital Outputs commands
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Common.h"

#if defined(CONFIG_MODULE_MASTER)

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

    void digitalWrite(DoutNum_t num, bool level) override;
    void toggleOutput(DoutNum_t num) override;
    void outputMode(DoutNum_t num, DoutMode_t mode) override;
    void setPWMFrequency(DoutNum_t num, uint32_t freq) override;
    void setPWMDutyCycle(DoutNum_t num, float duty) override;
    float getOutputCurrent(DoutNum_t num) override;
    int outputIsOvercurrent(DoutNum_t num) override;
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