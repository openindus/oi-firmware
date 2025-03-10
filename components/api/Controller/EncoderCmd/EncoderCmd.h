/**
 * @file EncoderCmd.h
 * @brief EncoderCmd class definition
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#if defined(MODULE_MASTER)

#include "ControllerTypes.h"
#include "Slave.h"
#include "Encoder.h"
#include "Master.h"

class EncoderCmd
{
private:
    Controller* _controller;
    int _instance;

public:
    EncoderCmd(Controller* controller, int instance) : 
        _controller(controller), _instance(instance) {}
    
    ~EncoderCmd() {}

    int begin(DIn_Num_t A, DIn_Num_t B, int16_t ppr);
    void end(void);

    void reset(void);
    int getRevolutions(void);
    int getPulses(void);
    float getAngle(void);
    float getSpeed(void);
};

#endif