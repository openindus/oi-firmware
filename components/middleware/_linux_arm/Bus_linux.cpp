/**
 * @file Bus_linux.cpp
 * @brief Bus interface for Linux
 * @author 
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "../Bus/Bus.h"

void BusIO::writeSync(uint8_t sync)
{
    /** @todo */
}

uint8_t BusIO::readSync(void)
{
    /** @todo */
    return 0;
}

int BusRS::read(Frame_t *frame, uint32_t timeout)
{
    /** @todo */
    return 0;
}

void BusRS::write(Frame_t *frame, uint32_t timeout)
{
    /** @todo */
}

int BusCAN::read(Frame_t* frame, uint16_t* id, uint8_t* size, uint32_t timeout)
{
    /** @todo */
    return 0;
}

int BusCAN::write(Frame_t* frame, uint16_t id, uint8_t size)
{
    /** @todo */
    return 0;
}