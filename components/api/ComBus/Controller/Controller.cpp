/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file Controller.cpp
 * @brief Generic functions for Module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#if defined(MODULE_MASTER)

#include "Controller.h"

static const char TAG[] = "Controller";

std::map<std::pair<uint8_t, uint16_t>, std::function<void(uint8_t)>> Controller::_eventCallbacks;
std::vector<Controller*> Controller::_instances;

/**
 * @brief Request a control Controller
 * 
 * @param byte Byte array
 * @return -1: error, 0 success
 */
int Controller::request(std::vector<uint8_t>& msgBytes, bool ackNeeded)
{
    BusRS::Frame_t frame;
    frame.cmd = CMD_REQUEST;
    frame.id = _id;
    frame.dir = 1;
    frame.ack = ackNeeded;
    frame.length = msgBytes.size();
    frame.data = msgBytes.data();
    int err = BusRS::transfer(&frame, pdMS_TO_TICKS(100));
    if (ackNeeded) {
        if (err < 0) {
            goto error;
        } else {
            msgBytes.assign(frame.data, frame.data + frame.length);
        }
    }
    return 0;

error:
    ESP_LOGE(TAG, "request error");
    return -1;
}

/**
 * @brief Send Controller to change led status
 * 
 * @param state Led state (On, Off, Blink)
 * @param color Led color
 * @param period Period in ms
 */
void Controller::_ledStatus(LedState_t state, LedColor_t color, uint32_t period)
{
    uint8_t payload[6];
    payload[0] = (uint8_t)state;
    payload[1] = (uint8_t)color;
    memcpy(&payload[2], &period, sizeof(uint32_t));
    BusRS::Frame_t frame;
    frame.cmd = CMD_LED_STATUS;
    frame.id = _id;
    frame.dir = 1;
    frame.ack = false;
    frame.length = 6;
    frame.data = payload;
    BusRS::write(&frame, pdMS_TO_TICKS(100));
    vTaskDelay(1 / portTICK_PERIOD_MS);
}

#endif