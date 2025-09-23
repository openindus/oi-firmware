/**
 * @file Discrete.h
 * @brief Discrete
 * @author Georges de Massol
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Common.h"
#include "Module.h"

#if (defined(CONFIG_OI_RELAY_HP))
#define RelayHP RelayHPStandalone
#elif (defined(CONFIG_OI_RELAY_HP)) && defined(CONFIG_MODULE_SLAVE)
#define RelayHP RelayHPSlave
#elif defined(CONFIG_MODULE_MASTER)
#define RelayHP RelayHPSlave
#endif

#if defined(CONFIG_OI_RELAY_HP)

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

#if (defined(CONFIG_OI_RELAY_HP)) && defined(CONFIG_MODULE_SLAVE)

#include "Slave.h"
#include "RelayCmdHandler.h"

class RelayHPSlave : 
    public Slave, 
    public RelayHPStandalone, 
    public RelayCmdHandler
{
public:

    static int init(void);
};

#elif defined(CONFIG_MODULE_MASTER)

#include "ModuleControl.h"
#include "RelayCmd.h"

class RelayHPSlave : 
    public ModuleControl, 
    public RelayCmd
{
public:

    RelayHPSlave(uint32_t sn = 0) :
        ModuleControl(TYPE_OI_RELAY_HP, sn),
        RelayCmd(this){}

    static int init(void);
};
#endif
