/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file OIMessage.h
 * @brief OpenIndus messages
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "OIProtocol.h"


class OIMessage 
{
public:

    OIMessage() {
        _type = 0;
        _id = 0;
        _conf = 0;
        _data = 0;
    }

    OIMessage(uint8_t type, uint8_t id, uint16_t conf=0, uint32_t data=0) {
        _type = type;
        _id = id;
        _conf = conf;
        _data = data;
    }

    ~OIMessage() {}

    /* operator surcharge */
    uint8_t operator[](uint8_t index) {
        return _byte[index];
    }

    uint8_t * operator()(void) {
        return _byte;
    }

    void operator()(uint8_t* byte, int size) {
        memcpy(_byte, byte, size);
    }

    friend bool operator==(OIMessage const& msg1, OIMessage const& msg2) {
        return (msg1._type == msg2._type);
    }

    friend bool operator!=(OIMessage const& msg1, OIMessage const& msg2) {
        return (msg1._type != msg2._type);
    }

    friend bool operator<(OIMessage const& msg1, OIMessage const& msg2) {
        return (msg1._type < msg2._type);
    }


    /* getters/setters */

    void setType(uint8_t type) { _type = type; }
    uint8_t getType() const { return _type; }

    void setId(uint8_t id) { _id = id; }
    uint8_t getId() const { return _id; }

    void setConf(uint16_t conf) { _conf = conf; }
    uint16_t getConf() const { return _conf; }
    uint8_t getConf(uint8_t byte) const { return _confByte[byte]; }

    void setData(uint32_t data) { _data = data; }
    uint32_t getData() const { return _data; }
    uint8_t getData(uint8_t byte) const { return _dataByte[byte]; }


private:

    union 
    {
        uint8_t _byte[8];
        struct
        {
            uint8_t _type;
            uint8_t _id;
            union {
                uint16_t _conf;
                uint8_t _confByte[2];
            };
            union {
                uint32_t _data;
                uint8_t _dataByte[4];
            };
        };
    };
};