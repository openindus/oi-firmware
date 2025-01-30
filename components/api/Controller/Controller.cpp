/**
 * @file Controller.cpp
 * @brief Controller interface
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#if defined(MODULE_MASTER)

#include "Controller.h"
#include "MasterController.h"

static const char TAG[] = "Controller";

Controller::Controller(uint16_t type, uint32_t sn) : _id(0xFFFF), _type(type), _sn(sn)
{
    MasterController::addControllerInstance(this);
}

template <typename... Args> int Controller::sendCmd(const uint8_t cmd, Args... args)
{
    std::vector<uint8_t> msgBytes = {cmd, args...};
    return sendCmd(cmd, msgBytes);
}

/**
 * Sends a command over the communication bus.
 *
 * @param cmd The command to send.
 * @param msgBytes The message bytes to send along with the command.
 * @param ackNeeded Indicates whether an acknowledgment is needed for this command.
 * @return 0 if the command was sent successfully, -1 if there was an error.
 */
int Controller::sendCmd(const uint8_t cmd, std::vector<uint8_t> &msgBytes, bool ackNeeded)
{
    BusRS::Frame_t frame;
    frame.cmd    = cmd;
    frame.id     = _id;
    frame.dir    = 1;
    frame.ack    = ackNeeded;
    frame.length = msgBytes.size();
    frame.data   = msgBytes.data();
    int err      = BusRS::transfer(&frame, 100);
    if (ackNeeded) {
        if (err < 0) {
            goto error;
        } else {
            msgBytes.assign(frame.data, frame.data + frame.length);
        }
    }
    return 0;

error:
#ifndef LINUX_ARM
    ESP_LOGE(TAG, "Request error: CMD: %x", cmd);
#endif
    return -1;
}

template <typename... Args> int Controller::sendRequest(const uint8_t cmd, Args... args)
{
    // std::array<uint8_t, sizeof...(args)> bytes = { static_cast<uint8_t>(args)... };
    std::vector<uint8_t> msgBytes = {cmd, args...};
    return sendRequest(CMD_REQUEST, msgBytes);
}

/**
 * @brief Request a control Controller
 *
 * @param byte Byte array
 * @return -1: error, 0 success
 */
int Controller::sendRequest(std::vector<uint8_t> &msgBytes, bool ackNeeded)
{
    return sendCmd(CMD_REQUEST, msgBytes, ackNeeded);
}

/**
 * @brief Restart the module
 *
 */
void Controller::restart(void)
{
    std::vector<uint8_t> msgBytes;
    this->sendCmd(CMD_RESTART, msgBytes, false);
}

/**
 * @brief Send Controller to change led status
 *
 * @param state Led state (On, Off, Blink)
 * @param color Led color
 * @param period Period in ms
 */
void Controller::_setLed(LedState_t state, LedColor_t color, uint32_t period)
{
    std::vector<uint8_t> msgBytes = {(uint8_t)state, (uint8_t)color};
    // Add period to message (4 bytes)
    msgBytes.insert(msgBytes.end(), (uint8_t *)&period, (uint8_t *)&period + sizeof(uint32_t));
    this->sendCmd(CMD_SET_LED, msgBytes, false);
}

#endif