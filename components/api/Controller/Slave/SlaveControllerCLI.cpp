/**
 * @file SlaveControllerCLI.cpp
 * @brief Command line interface - Slave Controller
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "SlaveController.h"
#include "Global.h"
#include "argtable3/argtable3.h"

#if defined(MODULE_SLAVE)

/* --- stop --- */

static int controllerStopCmd(int argc, char **argv)
{
    SlaveController::stop();
    return 0;
}

static int _registerStopCmd(void)
{
    const esp_console_cmd_t cmd = {
        .command = "stop-controller",
        .help = "Stop controller tasks",
        .hint = NULL,
        .func = &controllerStopCmd,
        .argtable = NULL
    };
    
    if (esp_console_cmd_register(&cmd) == ESP_OK) {
        return 0;
    } else {
        return -1;
    }
}

/* --- start --- */

static int controllerStartCmd(int argc, char **argv)
{
    SlaveController::start();
    return 0;
}

static int _registerStartCmd(void)
{
    const esp_console_cmd_t cmd = {
        .command = "start-controller",
        .help = "Start controller tasks",
        .hint = NULL,
        .func = &controllerStartCmd,
        .argtable = NULL
    };
    
    if (esp_console_cmd_register(&cmd) == ESP_OK) {
        return 0;
    } else {
        return -1;
    }
}

/* --- get-status --- */

static int controllerGetStatusCmd(int argc, char **argv)
{
    int state;

    state = SlaveController::getStatus();

    switch (state) {
        case STATE_IDLE:
            printf("Bus controller state: IDLE\n");
            break;
        case STATE_RUNNING:
            printf("Bus controller state: RUNNING\n");
            break;
        case STATE_ERROR:
            printf("Bus controller state: ERROR\n");
            break;
        default:
            printf("Bus controller state: UNDEFINED\n");
            break;
    }

    return 0;
}

static int _registerGetStatusCmd(void)
{
    const esp_console_cmd_t cmd = {
        .command = "status-controller",
        .help = "Get controller status",
        .hint = NULL,
        .func = &controllerGetStatusCmd,
        .argtable = NULL
    };
    
    if (esp_console_cmd_register(&cmd) == ESP_OK) {
        return 0;
    } else {
        return -1;
    }
}

int SlaveController::_registerCLI(void)
{
    int err = 0;
    err |= _registerStopCmd();
    err |= _registerStartCmd();
    err |= _registerGetStatusCmd();
    return err;
}

#endif