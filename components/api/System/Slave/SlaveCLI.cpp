/**
 * @file SlaveCLI.cpp
 * @brief Command line interface - Slave
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#if defined(MODULE_SLAVE)

#include "Slave.h"
#include "Common.h"
#include "argtable3/argtable3.h"

/* --- stop --- */

static int slaveStopCmd(int argc, char **argv)
{
    Slave::stop();
    return 0;
}

static int _registerStopCmd(void)
{
    const esp_console_cmd_t cmd = {
        .command = "stop-slave",
        .help = "Stop slave tasks",
        .hint = NULL,
        .func = &slaveStopCmd,
        .argtable = NULL
    };
    
    if (esp_console_cmd_register(&cmd) == ESP_OK) {
        return 0;
    } else {
        return -1;
    }
}

/* --- start --- */

static int slaveStartCmd(int argc, char **argv)
{
    Slave::start();
    return 0;
}

static int _registerStartCmd(void)
{
    const esp_console_cmd_t cmd = {
        .command = "start-slave",
        .help = "Start slave tasks",
        .hint = NULL,
        .func = &slaveStartCmd,
        .argtable = NULL
    };
    
    if (esp_console_cmd_register(&cmd) == ESP_OK) {
        return 0;
    } else {
        return -1;
    }
}

/* --- get-status --- */

static int slaveGetStatusCmd(int argc, char **argv)
{
    int status;

    status = Slave::getStatus();

    switch (status) {
        case STATE_IDLE:
            printf("Slave status: IDLE\n");
            break;
        case STATE_RUNNING:
            printf("Slave status: RUNNING\n");
            break;
        case STATE_ERROR:
            printf("Slave status: ERROR\n");
            break;
        default:
            printf("Slave status: UNDEFINED\n");
            break;
    }

    return 0;
}

static int _registerGetStatusCmd(void)
{
    const esp_console_cmd_t cmd = {
        .command = "status-slave",
        .help = "Get slave status",
        .hint = NULL,
        .func = &slaveGetStatusCmd,
        .argtable = NULL
    };
    
    if (esp_console_cmd_register(&cmd) == ESP_OK) {
        return 0;
    } else {
        return -1;
    }
}

int Slave::_registerCLI(void)
{
    int err = 0;
    err |= _registerStopCmd();
    err |= _registerStartCmd();
    err |= _registerGetStatusCmd();
    return err;
}

#endif