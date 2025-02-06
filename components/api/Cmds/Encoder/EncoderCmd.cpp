/**
 * @file EncoderCmd.cpp
 * @brief EncoderCmd class implementation
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#if defined(MODULE_MASTER)

#include "EncoderCmd.h"
#include "Cmds.h"

int EncoderCmd::begin(DIn_Num_t A, DIn_Num_t B, uint32_t ppr)
{
    std::vector<uint8_t> msgBytes = {REQUEST_ENCODER_BEGIN, (uint8_t)A, (uint8_t)B};
    uint8_t *ptr                  = reinterpret_cast<uint8_t *>(&ppr);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(uint32_t));
    return _controller->sendRequest(msgBytes);
    // return _controller->sendRequest(REQUEST_ENCODER_BEGIN, A, B, ppr);
}

void EncoderCmd::end(void)
{
    std::vector<uint8_t> msgBytes = {REQUEST_ENCODER_END};
    _controller->sendRequest(msgBytes);
}

float EncoderCmd::getPosition(void)
{
    std::vector<uint8_t> msgBytes = {REQUEST_ENCODER_GET_POSITION};
    _controller->sendRequest(msgBytes);
    float *position = reinterpret_cast<float *>(&msgBytes[2]);
    return *position;
}

float EncoderCmd::getSpeed(void)
{
    std::vector<uint8_t> msgBytes = {REQUEST_ENCODER_GET_SPEED};
    _controller->sendRequest(msgBytes);
    float *speed = reinterpret_cast<float *>(&msgBytes[2]);
    return *speed;
}

float EncoderCmd::getAngle(void)
{
    std::vector<uint8_t> msgBytes = {REQUEST_ENCODER_GET_ANGLE};
    _controller->sendRequest(msgBytes);
    float *angle = reinterpret_cast<float *>(&msgBytes[2]);
    return *angle;
}

#endif