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
#include "ModuleCommand.h"

class ModuleSlave
{
public:

    static void init(void);

    static void event(EventType_t type, int num);
    static void onRequest(RequestCmd_t cmd, RequestCallback_t request);
    static uint32_t handleRequest(RequestMsg_t msg);

private:

    static uint16_t _id;
    static std::map<RequestCmd_t, RequestCallback_t> _request;

    static void _busTask(void *pvParameters);
};
