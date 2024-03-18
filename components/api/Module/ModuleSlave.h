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
#include "ModuleCmd.h"

class ModuleSlave
{
public:

    static void init(void);

    static void event(ModuleCmd_EventId_t eventId, int num);
    static void onRequest(ModuleCmd_RequestId_t requestId, ModuleCmd_RequestCallback_t callback);
    static uint32_t handleRequest(ModuleCmd_RequestMsg_t msg);

private:

    static uint16_t _id;
    static std::map<ModuleCmd_RequestId_t, ModuleCmd_RequestCallback_t> _callback;

    static void _busTask(void *pvParameters);
    static void _heartbeatTask(void *pvParameters);
};
