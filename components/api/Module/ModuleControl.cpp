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

uint32_t ModuleControl::request(ModuleCmd_RequestMsg_t msg)
{
    BusRs::Frame_t frame;
    frame.command = CMD_REQUEST;
    frame.id = _id;
    frame.dir = 1;
    frame.ack = true;
    frame.length = sizeof(msg.byte);
    frame.data = msg.byte;
    BusRs::write(&frame, pdMS_TO_TICKS(100));
    if (BusRs::read(&frame, pdMS_TO_TICKS(100)) < 0) {
        ESP_LOGE(MODULE_TAG, "requestFrom error");
        return 0xFFFFFFFF;
    } else {
        ModuleCmd_RequestMsg_t newMsg;
        memcpy(&newMsg, frame.data, sizeof(newMsg));
        return newMsg.data;
    }
}

void ModuleControl::ledOn(LedColor_t color)
{
    _ledCtrl(LED_ON, color, 0);
}

void ModuleControl::ledOff(void)
{
    _ledCtrl(LED_OFF, LED_NONE, 0);
}

void ModuleControl::ledBlink(LedColor_t color, uint32_t period)
{
    _ledCtrl(LED_BLINK, color, period);
}

void ModuleControl::_ledCtrl(LedState_t state, LedColor_t color, uint32_t period)
{
    uint8_t payload[6];
    payload[0] = (uint8_t)state;
    payload[1] = (uint8_t)color;
    memcpy(&payload[2], &period, sizeof(uint32_t));
    BusRs::Frame_t frame;
    frame.command = CMD_LED_CTRL;
    frame.id = _id;
    frame.dir = 1;
    frame.ack = false;
    frame.length = 6;
    frame.data = payload;
    BusRs::write(&frame, pdMS_TO_TICKS(100));
    vTaskDelay(10 / portTICK_PERIOD_MS);
}