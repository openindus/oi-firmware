/**
 * @file CLI.h
 * @brief Command line interface
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"

/**
 * @brief Register CLI bus commands
 * 
 * @return int 0: success, -1: error
 */
int CLI_Bus_registerCommands(void);

/**
 * @brief Register CLI module (Slave & Master) commands
 * 
 * @return int 0: success, -1: error
 */
int CLI_Module_registerCommands(void);