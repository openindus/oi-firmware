/**
 * @file ModuleControl.h
 * @brief ModuleControl class definition
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Common.h"

#if defined(CONFIG_MODULE_MASTER)

#include "Board.h"
#include "Led.h"
#include "Bus.h"
#include "FlashLoader.h"

class ModuleControl
{
public:
    ModuleControl(uint16_t id);
    ModuleControl(uint16_t type, uint32_t sn);

    inline void setId(uint16_t id) { _id = id; }
    inline uint16_t getId(void) { return _id; }
    inline void setSN(uint32_t sn) { _sn = sn; }
    inline uint32_t getSN(void) { return _sn; }
    inline uint16_t getType(void) { return _type; }

    int runCallback(const uint8_t callbackId, std::vector<uint8_t> &args, bool ackNeeded = true);
    int runCallback(std::vector<uint8_t> &msgBytes, bool ackNeeded = true);

    void restart(void);

    void ledOn(LedColor_t color);
    void ledOff(void);
    void ledBlink(LedColor_t color, uint32_t period);
    void ledSync(void);

    void registerModuleEventCallback(ModuleControl* module, 
        uint8_t eventId, uint8_t eventArg, 
        uint8_t callbackId, std::vector<uint8_t> callbackArgs);

private:
    uint16_t _id;   // Slave id
    uint16_t _type; // Board type
    uint32_t _sn;   // Board serial number
};

#endif