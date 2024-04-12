/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file BrushlessStandalone.h
 * @brief Functions for Brushless module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "ModuleStandalone.h"

#if defined(OI_BRUSHLESS)

class BrushlessStandalone : public ModuleStandalone
{
public:

    static void init(void);
    static void setSpeed(uint32_t duty_cycle);
    static void setBrake(bool brake);
    static void setDirection(bool direction);
    static float getSpeed(void);
    static bool getDirection(void);
    static float getSpeedEncoder(void);
    static float getPosition(void);

private:

};

#endif