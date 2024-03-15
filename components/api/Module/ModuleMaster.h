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

#include "Global.h"
#include "ModuleStandalone.h"
#include "Module.h"

class ModuleMaster
{
public:

    static void init(void);

    static bool autoId(void);
    static void autoTest(void);
    static void program(uint32_t num);
    static bool ping(uint32_t num); 
    
    static void onEvent(Module_Event_t event, uint16_t id, Module_EventCallback_t callback);
    static void handleEvent(Module_Event_t event, uint16_t id, int num);

    static uint16_t getIdFromSN(int num);

private:

    static std::map<std::pair<Module_Event_t,uint16_t>, Module_EventCallback_t> _event;
    static std::vector<uint16_t> _ids;

    static void _busTask(void *pvParameters);
    static void _programmingTask(void *pvParameters);

};
