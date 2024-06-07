#pragma once

#if defined(MODULE_SLAVE)

#include "BusCtrlSlave.h"
#include "Relays.h"

class BusCtrlSlave_Relay
{
public:

    static int init();
};

#endif