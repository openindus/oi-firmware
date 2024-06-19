/**
 * @file Discrete.h
 * @brief Discrete
 * @author Georges de Massol
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "Module.h"

#if (defined(OI_RELAY_HP)) && defined(MODULE_STANDALONE)
#define RelayHP RelayHPStandalone
#elif (defined(OI_RELAY_HP)) && defined(MODULE_SLAVE)
#define RelayHP RelayHPSlave
#elif defined(MODULE_MASTER)
#define RelayHP RelayHPController
#endif

#if defined(OI_RELAY_HP)

#include "RelayPinout.h"
#include "Relays.h"

class RelayHPStandalone : 
    public Module, 
    public Relays
{
public:

    static int init(void);
};

#endif

#if (defined(OI_RELAY_HP)) && defined(MODULE_SLAVE)

#include "ControllerSlave.h"
#include "ResponseRelay.h"

class RelayHPSlave : 
    public ControllerSlave, 
    public RelayHPStandalone, 
    public ResponseRelay
{
public:

    static int init(void);
};

#elif defined(MODULE_MASTER)

#include "Controller.h"
#include "CommandRelay.h"

class RelayHPController : 
    public Controller, 
    public CommandRelay
{
public:

    RelayHPController(uint32_t sn = 0) :
        Controller(TYPE_OI_RELAY_HP, sn),
        CommandRelay(this){}

    static int init(void);
};
#endif
