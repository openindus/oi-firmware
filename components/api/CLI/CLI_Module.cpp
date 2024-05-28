/**
 * @file CLI_Module.cpp
 * @brief Command line interface - Module
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "CLI.h"
#include "ModuleMaster.h"
#include "ModuleSlave.h"

/* --- Module stop --- */

static int moduleStopCmd(int argc, char **argv)
{
#if defined(MODULE_MASTER)
    ModuleMaster::stop();
#endif
    return 0;
}

static int registerModuleStopCmd(void)
{
    const esp_console_cmd_t cmd = {
        .command = "stop",
        .help = "Stop module tasks",
        .hint = NULL,
        .func = &moduleStopCmd,
        .argtable = NULL
    };
    
    if (esp_console_cmd_register(&cmd) == ESP_OK) {
        return 0;
    } else {
        return -1;
    }
}

/* --- */

int CLI_Module_registerCommands(void)
{
    int err = 0;
    err |= registerModuleStopCmd();
    return err;
}