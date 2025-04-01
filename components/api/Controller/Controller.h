/**
 * @file Controller.h
 * @brief Controller class definition
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#if defined(MODULE_MASTER)

#include "Common.h"
#include "Board.h"
#include "Led.h"
#include "Bus.h"
#include "FlashLoader.h"

class Controller
{
public:
    Controller(uint16_t id);
    Controller(uint16_t type, uint32_t sn);

    inline void setId(uint16_t id) { _id = id; }
    inline uint16_t getId(void) { return _id; }
    inline void setSN(uint32_t sn) { _sn = sn; }
    inline uint32_t getSN(void) { return _sn; }
    inline uint16_t getType(void) { return _type; }

    int performRequest(const uint8_t request, std::vector<uint8_t> &msgBytes, bool ackNeeded = true);
    int performRequest(std::vector<uint8_t> &msgBytes, bool ackNeeded = true);

    void ledOn(LedColor_t color);
    void ledOff(void);
    void ledBlink(LedColor_t color, uint32_t period);

    void restart(void);

private:
    uint16_t _id;   // Board id
    uint16_t _type; // Board
    uint32_t _sn;   // Serial number
};

#endif