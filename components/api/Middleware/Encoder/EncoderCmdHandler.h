/**
 * @file EncoderCmdHandler.h
 * @brief EncoderCmdHandler class definition
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#if defined(MODULE_SLAVE)

#include "Slave.h"
#include "Encoder.h"

class EncoderCmdHandler
{
public:
    static int init(Encoder **encoder);

private:
    static Encoder **_encoder;
};

#endif