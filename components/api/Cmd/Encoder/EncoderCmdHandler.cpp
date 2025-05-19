/**
 * @file EncoderCmdHandler.cpp
 * @brief EncoderCmdHandler class implementation
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#if defined(MODULE_SLAVE)

#include "EncoderCmdHandler.h"

static const char TAG[] = "EncoderCmdHandler";

Encoder **EncoderCmdHandler::_encoder = {nullptr};

int EncoderCmdHandler::init(Encoder** encoder)
{
    _encoder = encoder;

    Slave::addCallback(CALLBACK_ENCODER_BEGIN, [](std::vector<uint8_t> &msgBytes) {
        int instance  = msgBytes[1];
        DIn_Num_t A   = (DIn_Num_t)msgBytes[2];
        DIn_Num_t B   = (DIn_Num_t)msgBytes[3];
        uint32_t *ppr = reinterpret_cast<uint32_t *>(&msgBytes[4]);
        if (_encoder[instance] != nullptr) {
            _encoder[instance]->begin(A, B, *ppr);
        }
    });

    Slave::addCallback(CALLBACK_ENCODER_END, [](std::vector<uint8_t> &msgBytes) {
        int instance = msgBytes[1];
        if (_encoder[instance] != nullptr) {
            _encoder[instance]->end();
        }
    });

    Slave::addCallback(CALLBACK_ENCODER_RESET, [](std::vector<uint8_t> &msgBytes) {
        int instance = msgBytes[1];
        if (_encoder[instance] != nullptr) {
            _encoder[instance]->reset();
        }
    });

    Slave::addCallback(CALLBACK_ENCODER_GET_REVOLUTIONS, [](std::vector<uint8_t> &msgBytes) {
        int instance = msgBytes[1];
        if (_encoder[instance] != nullptr) {
            int revolutions = _encoder[instance]->getRevolutions();
            uint8_t *ptr    = reinterpret_cast<uint8_t *>(&revolutions);
            msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(int));
        }
    });

    Slave::addCallback(CALLBACK_ENCODER_GET_PULSES, [](std::vector<uint8_t> &msgBytes) {
        int instance = msgBytes[1];
        if (_encoder[instance] != nullptr) {
            int pulses = _encoder[instance]->getPulses();
            uint8_t *ptr = reinterpret_cast<uint8_t *>(&pulses);
            msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(int));
        }
    });

    Slave::addCallback(CALLBACK_ENCODER_GET_ANGLE, [](std::vector<uint8_t> &msgBytes) {
        int instance = msgBytes[1];
        if (_encoder[instance] != nullptr) {
            float angle  = _encoder[instance]->getAngle();
            uint8_t *ptr = reinterpret_cast<uint8_t *>(&angle);
            msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
        }
    });

    Slave::addCallback(CALLBACK_ENCODER_GET_SPEED, [](std::vector<uint8_t> &msgBytes) {
        int instance = msgBytes[1];
        if (_encoder[instance] != nullptr) {
            float speed  = _encoder[instance]->getSpeed();
            uint8_t *ptr = reinterpret_cast<uint8_t *>(&speed);
            msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
        }
    });

    return 0;
}

#endif