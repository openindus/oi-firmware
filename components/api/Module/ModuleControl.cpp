/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file ModuleControl.cpp
 * @brief Generic functions for Module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "ModuleControl.h"

static const char MODULE_TAG[] = "Module";

std::vector<ModuleControl*> ModuleControl::_instances;

uint32_t ModuleControl::request(Module_RequestMsg_t msg)
{
    BusRs::Frame_t frame;
    frame.command = MODULE_REQUEST;
    frame.identifier = _id;
    frame.broadcast = false;
    frame.direction = 1;
    frame.ack = true;
    frame.length = sizeof(msg.byte);
    frame.data = msg.byte;
    if (BusRs::requestFrom(&frame, pdMS_TO_TICKS(100)) < 0) {
        ESP_LOGE(MODULE_TAG, "requestFrom error");
        return 0xFFFFFFFF;
    } else {
        Module_RequestMsg_t newMsg;
        memcpy(&newMsg, frame.data, sizeof(newMsg));
        return newMsg.data;
    }
}

void ModuleControl::ledOn(LedColor_t color)
{
    _ledState(LED_ON, color, 0);
}

void ModuleControl::ledOff(void)
{
    _ledState(LED_OFF, LED_NONE, 0);
}

void ModuleControl::ledBlink(LedColor_t color, uint32_t period)
{
    _ledState(LED_BLINK, color, period);
}

void ModuleControl::_ledState(LedState_t state, LedColor_t color, uint32_t period)
{
    uint8_t payload[6];
    payload[0] = (uint8_t)state;
    payload[1] = (uint8_t)color;
    memcpy(&payload[2], &period, sizeof(uint32_t));
    BusRs::Frame_t frame;
    frame.command = MODULE_LED_STATE;
    frame.identifier = _id;
    frame.broadcast = false;
    frame.direction = 1;
    frame.ack = false;
    frame.length = 6;
    frame.data = payload;
    BusRs::write(&frame, pdMS_TO_TICKS(100));
    vTaskDelay(10 / portTICK_PERIOD_MS);
}