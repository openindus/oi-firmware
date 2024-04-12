/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file BrushlessControl.h
 * @brief Functions for brushless module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "ModuleControl.h"
#include "ModuleMaster.h"

#if !defined(OI_BRUSHLESS)

class BrushlessControl : public ModuleControl
{
public:

    BrushlessControl(int sn = 0) : ModuleControl(sn) {}
    void setSpeed(uint32_t duty_cycle);
    void setBrake(bool brake);
    void setDirection(bool direction);
    float getSpeed(void);
    bool getDirection(void);
    float getSpeedEncoder(void);
    float getPosition(void);

private:

    IsrCallback_t _isrCallback[DIN_MAX];
};
#endif