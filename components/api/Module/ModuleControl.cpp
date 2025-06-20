/**
 * @file ModuleControl.cpp
 * @brief ModuleControl class implementation
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#if defined(MODULE_MASTER)

#include "ModuleControl.h"
#include "Master.h"

static const char TAG[] = "ModuleControl";

ModuleControl::ModuleControl(uint16_t id) : _id(id), _type(0), _sn(0) {}

ModuleControl::ModuleControl(uint16_t type, uint32_t sn) : _id(0xFFFF), _type(type), _sn(sn)
{
    Master::addModuleControlInstance(this);
}

int ModuleControl::runCallback(const uint8_t callbackId, std::vector<uint8_t> &args, bool ackNeeded)
{
    return Master::runCallback(_id, callbackId, args, ackNeeded);
}

int ModuleControl::runCallback(std::vector<uint8_t> &msgBytes, bool ackNeeded)
{
    return Master::runCallback(_id, msgBytes, ackNeeded);
}

void ModuleControl::restart(void)
{ 
    Master::moduleRestart(_id);
}

void ModuleControl::ledOn(LedColor_t color)
{
    Master::ledCtrl(_id, LED_ON, color);
}

void ModuleControl::ledOff(void)
{
    Master::ledCtrl(_id, LED_OFF);
}

void ModuleControl::ledBlink(LedColor_t color, uint32_t period)
{
    Master::ledCtrl(_id, LED_BLINK, color, period);
};

void ModuleControl::ledSync(void)
{
    Master::ledCtrl(_id, LED_SYNC);
}

void ModuleControl::registerModuleEventCallback(ModuleControl* module, 
    uint8_t eventId, uint8_t eventArg, 
    uint8_t callbackId, std::vector<uint8_t> callbackArgs)
{
    Master::registerEventCallback(_id, module, eventId, eventArg, callbackId, callbackArgs);
}

#endif