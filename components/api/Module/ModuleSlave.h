/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file ModuleSlave.h
 * @brief Functions for Module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "ModuleStandalone.h"
#include "Module.h"

class ModuleSlave
{
public:

    static void init(void);

    static void event(Module_Event_t event, int num);
    static void onRequest(Module_Request_t request, Module_RequestCallback_t callback);
    static uint32_t handleRequest(Module_RequestMsg_t msg);

private:

    static uint16_t _id;
    static std::map<Module_Request_t, Module_RequestCallback_t> _request;

    static void _busTask(void *pvParameters);
};
