/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DcCmdHandler.h
 * 
 * For more information on OpenIndus:
 * @see https://openindus.com
 * 
 * @copyright Copyright (c) 2024
**/

#include "DcCmdHandler.h"

#if defined(CONFIG_MODULE_SLAVE)

static const char* TAG = "DcCmdHandler";

int DcCmdHandler::init() {
    
    int err = 0;

    Slave::addCallback(CALLBACK_MOTOR_DC_RUN, [](std::vector<uint8_t>& data) {
        // if (data.size() < 7) { // 1 byte cmd + 1 byte motor + 1 byte direction + 4 bytes float
        //     ESP_LOGE(TAG, "Invalid data size for RUN: %d", data.size());
        //     return;
        // }
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        MotorDirection_t direction = static_cast<MotorDirection_t>(data[2]);
        float *dutyCycle = reinterpret_cast<float *>(&data[3]);
        MotorDc::run(motor, direction, *dutyCycle);
        data.clear();
    });

    Slave::addCallback(CALLBACK_MOTOR_DC_STOP, [](std::vector<uint8_t>& data) {
        // if (data.size() < 2) { // 1 byte cmd + 1 byte motor
        //     ESP_LOGE(TAG, "Invalid data size for STOP: %d", data.size());
        //     return;
        // }
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        MotorDc::stop(motor);
        data.clear();
    });

    Slave::addCallback(CALLBACK_MOTOR_DC_GET_CURRENT, [](std::vector<uint8_t>& data) {
        // if (data.size() < 2) { // 1 byte cmd + 1 byte motor
        //     ESP_LOGE(TAG, "Invalid data size for GET_CURRENT: %d", data.size());
        //     return;
        // }
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        float current = MotorDc::getCurrent(motor);
        uint8_t *ptr = reinterpret_cast<uint8_t *>(&current);
        data.insert(data.end(), ptr, ptr + sizeof(float));
    });

    return err;
}

#endif