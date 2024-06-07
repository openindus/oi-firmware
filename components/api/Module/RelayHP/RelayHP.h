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
#define RelayHP RelayHPBusCtrl
#endif

#if defined(OI_RELAY_HP)

#include "RelayPinout.h"
#include "Relays.h"

class RelayHPStandalone : 
    public ModuleStandalone, 
    public Relays
{
public:

    static int init(void);
};

#include "CLI_Relay.h"

class RelayHPCLI : 
    public CLI_Relay
{
public:

    static int init(void);
};

#endif

#if (defined(OI_RELAY_HP)) && defined(MODULE_SLAVE)

#include "BusCtrlSlave.h"
#include "BusCtrlSlave_Relay.h"

class RelayHPSlave : 
    public BusCtrlSlave, 
    public RelayHPStandalone, 
    public BusCtrlSlave_Relay
{
public:

    static int init(void);
};

#elif defined(MODULE_MASTER)

#include "BusCtrlCmd.h"
#include "BusCtrlCmd_Relay.h"

class RelayHPBusCtrl : 
    public BusCtrlCmd, 
    public BusCtrlCmd_Relay
{
public:

    RelayHPBusCtrl(uint32_t sn = 0) :
        BusCtrlCmd(TYPE_OI_RELAY_HP, sn),
        BusCtrlCmd_Relay(this){}

    static int init(void);
};
#endif
