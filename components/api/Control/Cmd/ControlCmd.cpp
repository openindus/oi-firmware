/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file ControlCmd.cpp
 * @brief Generic functions for Module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#if defined(MODULE_MASTER)

#include "ControlCmd.h"

static const char TAG[] = "Module";

std::map<std::pair<uint8_t, uint16_t>, std::function<void(uint8_t)>> ControlCmd::_eventCallbacks;
std::vector<ControlCmd*> ControlCmd::_instances;

/**
 * @brief 
 * 
 * @param color 
 */
void ControlCmd::ledOn(LedColor_t color)
{
    _ledStatus(LED_ON, color, 0);
}

/**
 * @brief 
 * 
 */
void ControlCmd::ledOff(void)
{
    _ledStatus(LED_OFF, LED_NONE, 0);
}

/**
 * @brief 
 * 
 * @param color 
 * @param period 
 */
void ControlCmd::ledBlink(LedColor_t color, uint32_t period)
{
    _ledStatus(LED_BLINK, color, period);
}

/**
 * @brief Request a control command
 * 
 * @param byte Byte array
 * @return -1: error, 0 success
 */
int ControlCmd::request(std::vector<uint8_t>& msgBytes, bool ackNeeded)
{
    BusRS::Frame_t frame;
    frame.cmd = CMD_REQUEST;
    frame.id = _id;
    frame.dir = 1;
    frame.ack = ackNeeded;
    frame.length = msgBytes.size();
    frame.data = msgBytes.data();
    BusRS::write(&frame, pdMS_TO_TICKS(100));
    if (ackNeeded) {
        if (BusRS::read(&frame, pdMS_TO_TICKS(100)) < 0) {
            ESP_LOGE(TAG, "control error");
            return -1;
        } else {
            msgBytes.assign(frame.data, frame.data + frame.length);
        }
    }
    return 0;
}

/**
 * @brief Send command to change led status
 * 
 * @param state Led state (On, Off, Blink)
 * @param color Led color
 * @param period Period in ms
 */
void ControlCmd::_ledStatus(LedState_t state, LedColor_t color, uint32_t period)
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