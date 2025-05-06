/**
 * @file Controller.cpp
 * @brief Controller class implementation
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#if defined(MODULE_MASTER)

#include "Controller.h"
#include "Master.h"

static const char TAG[] = "Controller";

Controller::Controller(uint16_t id) : _id(id), _type(0), _sn(0) {}

Controller::Controller(uint16_t type, uint32_t sn) : _id(0xFFFF), _type(type), _sn(sn)
{
    Master::addControllerInstance(this);
}

int Controller::performRequest(const uint8_t request, std::vector<uint8_t> &msgBytes, bool ackNeeded)
{
    return Master::performRequest(_id, request, msgBytes, ackNeeded);
}

int Controller::performRequest(std::vector<uint8_t> &msgBytes, bool ackNeeded)
{
    return Master::performRequest(_id, msgBytes, ackNeeded);
}

void Controller::restart(void)
{ 
    Master::restart(_id);
}

void Controller::ledOn(LedColor_t color)
{
    Master::setLed(_id, LED_ON, color, 0);
}

void Controller::ledOff(void)
{
    Master::setLed(_id, LED_OFF, LED_NONE, 0);
}

void Controller::ledBlink(LedColor_t color, uint32_t period)
{
    Master::setLed(_id, LED_BLINK, color, period);
};

#endif