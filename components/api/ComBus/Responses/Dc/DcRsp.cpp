/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DcRsp.h
 * 
 * For more information on OpenIndus:
 * @see https://openindus.com
 * 
 * @copyright Copyright (c) 2024
**/


#if defined(MODULE_SLAVE)

#include "DcRsp.h"

int DcRsp::init() {
    
    int err = 0;

    ControllerSlave::addCtrlCallback(REQUEST_MOTOR_DC_RUN, [](std::vector<uint8_t>& data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        MotorDirection_t direction = static_cast<MotorDirection_t>(data[2]);
        MotorDc::run(motor, direction);
        data.clear();
    });

    ControllerSlave::addCtrlCallback(REQUEST_MOTOR_DC_STOP, [](std::vector<uint8_t>& data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        MotorDc::stop(motor);
        data.clear();
    });

    return err;
}

#endif