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
 * Sends a command over the communication bus.
 *
 * @param cmd The command to send.
 * @param msgBytes The message bytes to send along with the command.
 * @param ackNeeded Indicates whether an acknowledgment is needed for this command.
 * @return 0 if the command was sent successfully, -1 if there was an error.
 */
int Controller::command(const uint8_t cmd, std::vector<uint8_t>& msgBytes, bool ackNeeded)
{
    BusRS::Frame_t frame;
    frame.cmd = cmd;
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
 * @brief Request a control Controller
 * 
 * @param byte Byte array
 * @return -1: error, 0 success
 */
int Controller::request(std::vector<uint8_t>& msgBytes, bool ackNeeded)
{
    return command(CMD_REQUEST, msgBytes, ackNeeded);
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
    std::vector<uint8_t> msgBytes = {(uint8_t)state, (uint8_t)color};
    // Add period to message (4 bytes)
    msgBytes.insert(msgBytes.end(), (uint8_t*)&period, (uint8_t*)&period + sizeof(uint32_t));
    this->command(CMD_LED_STATUS, msgBytes, false);
}

void Controller::writeSync(uint8_t level)
{
    std::vector<uint8_t> msgBytes = {(uint8_t)level};
    this->command(CMD_OI_GPIO, msgBytes);
}

void Controller::toggleSync()
{
    std::vector<uint8_t> msgBytes = {TOGGLE};
    this->command(CMD_OI_GPIO, msgBytes);
}


int Controller::readSync()
{
    std::vector<uint8_t> msgBytes = {READ};
    this->command(CMD_OI_GPIO, msgBytes);
    return static_cast<int>(msgBytes[1]);
}
#endif