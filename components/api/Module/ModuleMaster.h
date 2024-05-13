/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file ModuleMaster.h
 * @brief Functions for module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#if defined(MODULE_MASTER)

#include "Global.h"
#include "ModuleStandalone.h"
#include "Command.h"

class ModuleMaster
{
public:

    static int init(void);

    static bool autoId(void);
    static void autoTest(void);
    static void program(int num);
    static bool ping(int num); 

    static uint16_t getIdFromSN(int num);
    static void getBoardInfo(int num, Module_Info_t* board_info);
    static std::map<uint16_t,int,std::greater<uint16_t>> discoverSlaves(void);

private:

    static std::map<uint16_t,int,std::greater<uint16_t>> _ids; // List of ids and serial number received via CMD_DISCOVER

    static void _busTask(void *pvParameters);
    static void _programmingTask(void *pvParameters);

};

#endif