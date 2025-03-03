/**
 * @file EncoderCmd.cpp
 * @brief EncoderCmd class implementation
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#if defined(MODULE_MASTER)

#include "EncoderCmd.h"

int EncoderCmd::begin(DIn_Num_t A, DIn_Num_t B, int16_t ppr)
{
    std::vector<uint8_t> msgBytes = {REQUEST_ENCODER_BEGIN, (uint8_t)_instance, (uint8_t)A,
                                     (uint8_t)B};
    uint8_t *ptr                  = reinterpret_cast<uint8_t *>(&ppr);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(uint32_t));
    return _controller->sendRequest(msgBytes);
    // return _controller->sendRequest(REQUEST_ENCODER_BEGIN, A, B, ppr);
}

void EncoderCmd::end(void)
{
    std::vector<uint8_t> msgBytes = {REQUEST_ENCODER_END, (uint8_t)_instance};
    _controller->sendRequest(msgBytes);
}

void EncoderCmd::reset(void)
{
    std::vector<uint8_t> msgBytes = {REQUEST_ENCODER_RESET, (uint8_t)_instance};
    _controller->sendRequest(msgBytes);
}

int EncoderCmd::getRevolutions(void)
{
    std::vector<uint8_t> msgBytes = {REQUEST_ENCODER_GET_REVOLUTIONS, (uint8_t)_instance};
    _controller->sendRequest(msgBytes);
    int *revolutions = reinterpret_cast<int *>(&msgBytes[2]);
    return *revolutions;
}

int EncoderCmd::getPulses(void)
{
    std::vector<uint8_t> msgBytes = {REQUEST_ENCODER_GET_PULSES, (uint8_t)_instance};
    _controller->sendRequest(msgBytes);
    int *pulses = reinterpret_cast<int *>(&msgBytes[2]);
    return *pulses;
}

float EncoderCmd::getAngle(void)
{
    std::vector<uint8_t> msgBytes = {REQUEST_ENCODER_GET_ANGLE, (uint8_t)_instance};
    _controller->sendRequest(msgBytes);
    float *angle = reinterpret_cast<float *>(&msgBytes[2]);
    return *angle;
}

float EncoderCmd::getSpeed(void)
{
    std::vector<uint8_t> msgBytes = {REQUEST_ENCODER_GET_SPEED, (uint8_t)_instance};
    _controller->sendRequest(msgBytes);
    float *speed = reinterpret_cast<float *>(&msgBytes[2]);
    return *speed;
}

#endif