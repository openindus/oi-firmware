/**
 * @file DigitalInputsCmd.h
 * @brief Digital Inputs commands
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
#include "DigitalInputsInterface.h"

class DigitalInputsCmd : public DigitalInputsInterface
{
public:
    DigitalInputsCmd(ModuleControl *module) : _module(module), _isrCallback{NULL} {}
    DigitalInputsCmd(uint16_t id) : _module(new ModuleControl(id)), _isrCallback{NULL} {}
    ~DigitalInputsCmd() { delete _module; }

    int digitalRead(DIn_Num_t num) override;
    void attachInterrupt(DIn_Num_t num, IsrCallback_t callback, InterruptMode_t mode, void *arg = NULL) override;
    void detachInterrupt(DIn_Num_t num) override;

private:
    ModuleControl *_module;
    IsrCallback_t _isrCallback[DIN_MAX];

protected:
    friend class Core;

};

#endif