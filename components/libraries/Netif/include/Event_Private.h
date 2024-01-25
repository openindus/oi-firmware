#pragma once

#include "Netif_Private.h"

#define Task_StackDepth CONFIG_NETIF_TASK_STACK_DEPTH
#define Task_Priority CONFIG_NETIF_TASK_PRIORITY

void Netif_task_Connected(Netif_t *netif);
void Netif_task_Disconnected(Netif_t *netif);
