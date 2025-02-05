/**
 * @file EncoderCmdHandler.cpp
 * @brief EncoderCmdHandler class implementation
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#if defined(MODULE_SLAVE)

#include "EncoderCmdHandler.h"
#include "Commands.h"

int EncoderCmdHandler::init(void)
{
    SlaveController::addCmdHandler(REQUEST_ENCODER_BEGIN, [](std::vector<uint8_t> &msg) {
        DIn_Num_t A, B;
        uint32_t ppr;
        A = (DIn_Num_t)msg[1];
        B = (DIn_Num_t)msg[2];
        memcpy(&ppr, &msg[3], sizeof(uint32_t));
        Encoder::begin(A, B, ppr);
    });

    SlaveController::addCmdHandler(REQUEST_ENCODER_END, [](std::vector<uint8_t> &msg) {
        Encoder::end();
    });

    SlaveController::addCmdHandler(REQUEST_ENCODER_GET_POSITION, [](std::vector<uint8_t> &msg) {
        float position = Encoder::getPosition();
        std::vector<uint8_t> msgBytes = {REQUEST_ENCODER_GET_POSITION};
        uint8_t *ptr = reinterpret_cast<uint8_t *>(&position);
        msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
        SlaveController::sendEvent(msgBytes);
    });

    SlaveController::addCmdHandler(REQUEST_ENCODER_GET_SPEED, [](std::vector<uint8_t> &msg) {
        float speed = Encoder::getSpeed();
        std::vector<uint8_t> msgBytes = {REQUEST_ENCODER_GET_SPEED};
        uint8_t *ptr = reinterpret_cast<uint8_t *>(&speed);
        msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
        SlaveController::sendEvent(msgBytes);
    });

    SlaveController::addCmdHandler(REQUEST_ENCODER_GET_ANGLE, [](std::vector<uint8_t> &msg) {
        float angle = Encoder::getAngle();
        std::vector<uint8_t> msgBytes = {REQUEST_ENCODER_GET_ANGLE};
        uint8_t *ptr = reinterpret_cast<uint8_t *>(&angle);
        msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
        SlaveController::sendEvent(msgBytes);
    });

    return 0;
}

#endif