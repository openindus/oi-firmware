/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file OICommand.h
 * @brief OpenIndus function table
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include <stdint.h>
#include <functional>
#include <string>
#include <map>

#include "esp_log.h"

#include "OIMessage.h"

typedef std::function<uint32_t(OIMessage msg)> OIFonction;


class OICommand
{
    std::multimap<OIMessage, OIFonction> _commandTable;

public:

    void add(OIMessage const& msg, OIFonction function);
    void remove(OIMessage const& msg);
    bool exist(OIMessage const& msg);
    uint32_t run(OIMessage const& msg);
    void list(void);
};

extern OICommand CMD;