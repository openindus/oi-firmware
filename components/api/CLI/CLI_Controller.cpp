/**
 * @file CLI_Controller.cpp
 * @brief Command line interface - Module
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "CLI_Controller.h"
#include "ControllerMaster.h"
#include "ControllerSlave.h"

#if defined(MODULE_MASTER)

/* --- program --- */

static struct {
    struct arg_int *type;
    struct arg_int *sn;
    struct arg_end *end;
} programArgs;

static int programCmd(int argc, char **argv)
{
    uint16_t type;
    uint32_t sn;

    int nerrors = arg_parse(argc, argv, (void **) &programArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, programArgs.end, argv[0]);
        return 1;
    }

    type = programArgs.type->ival[0];
    sn = programArgs.sn->ival[0];

    ControllerMaster::program(type, sn);

    return 0;
}

int CLI_Controller::_registerProgramCmd(void)
{
    programArgs.type = arg_int1(NULL, NULL, "<TYPE>", "Board type");
    programArgs.sn = arg_int1(NULL, NULL, "<SN>", "Board serial number");
    programArgs.end = arg_end(1);
    const esp_console_cmd_t cmd = {
        .command = "program",
        .help = "System in programming mode",
        .hint = NULL,
        .func = &programCmd,
        .argtable = &programArgs
    };

    if (esp_console_cmd_register(&cmd) == ESP_OK) {
        return 0;
    } else {
        return -1;
    }
}

/* --- ping --- */

static struct {
    struct arg_int *type;
    struct arg_int *sn;
    struct arg_end *end;
} pingArgs;

static int pingCmd(int argc, char **argv)
{
    uint16_t type;
    uint32_t sn;
    int64_t t, t0;

    int nerrors = arg_parse(argc, argv, (void **) &pingArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, pingArgs.end, argv[0]);
        return 1;
    }

    type = pingArgs.type->ival[0];
    sn = pingArgs.sn->ival[0];

    t0 = esp_timer_get_time();
    if (ControllerMaster::ping(type, sn)) {
        t = esp_timer_get_time();
        printf("Ping module: %u time: %lld us\n", sn, (t-t0));
    } else {
        printf("Cannot ping module: %u\n", sn);
        return 2;
    }
    return 0;
}

int CLI_Controller::_registerPingCmd(void)
{
    pingArgs.type = arg_int1(NULL, NULL, "<TYPE>", "Board type");
    pingArgs.sn = arg_int1(NULL, NULL, "<SN>", "Board serial number");
    pingArgs.end = arg_end(1);
    const esp_console_cmd_t cmd = {
        .command = "ping",
        .help = "ping a board",
        .hint = NULL,
        .func = &pingCmd,
        .argtable = &pingArgs
    };

    if (esp_console_cmd_register(&cmd) == ESP_OK) {
        return 0;
    } else {
        return -1;
    }
}

/* --- auto-id --- */

static int autoId(int argc, char **argv) 
{
    ControllerMaster::autoId();
    return 0;
}

int CLI_Controller::_registerAutoIdCmd(void)
{
    const esp_console_cmd_t cmd = {
        .command = "auto-id",
        .help = "Send a frame to get all slaves IDs",
        .hint = NULL,
        .func = &autoId,
        .argtable = NULL
    };

    if (esp_console_cmd_register(&cmd) == ESP_OK) {
        return 0;
    } else {
        return -1;
    }
}


/* --- discover-slaves --- */

static int discoverSlavesCmd(int argc, char **argv) 
{
    
    std::map<uint16_t,std::pair<uint16_t, uint32_t>,std::greater<uint16_t>> _ids = ControllerMaster::discoverSlaves();

    printf("[");
            
    std::map<uint16_t, std::pair<uint16_t, uint32_t>,std::greater<uint16_t>>::iterator it = _ids.begin();

    while (it != _ids.end()) {
        printf("{\"id\":%i,\"type\":%i,\"sn\":%i}", it->first, it->second.first, it->second.second);
        ++it;
        // Print "," if it is not the end of the table
        if (it != _ids.end()) {
            printf(",");
        }
    }

    printf("]\n");

    return 0;
}

int CLI_Controller::_registerDiscoverSlavesCmd(void)
{
    const esp_console_cmd_t cmd = {
        .command = "discover-slaves",
        .help = "Discover all slaves on Bus and return information as json table: [{\"id\":1,\"type\":3,\"sn\":1},{\"id\":233,\"type\":13,\"sn\":563633},...]",
        .hint = NULL,
        .func = &discoverSlavesCmd,
        .argtable = NULL
    };

    if (esp_console_cmd_register(&cmd) == ESP_OK) {
        return 0;
    } else {
        return -1;
    }
}

/* --- get-slave-info --- */

static struct {
    struct arg_int *type;
    struct arg_int *sn;
    struct arg_lit *boardType;
    struct arg_lit *serialNum;
    struct arg_lit *versionHW;
    struct arg_lit *dateCode;
    struct arg_lit *versionSW;
    struct arg_end *end;
} getSlaveInfoArgs;

static int getSlaveInfoCmd(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &getSlaveInfoArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, getSlaveInfoArgs.end, argv[0]);
        return 1;
    }

    Board_Info_t boardInfo;

    if (getSlaveInfoArgs.type->count == 1 && getSlaveInfoArgs.sn->count == 1) {
        ControllerMaster::getBoardInfo((uint16_t)getSlaveInfoArgs.type->ival[0], (uint32_t)getSlaveInfoArgs.sn->ival[0], &boardInfo);
    } else {
        return -1;
    }

    if (getSlaveInfoArgs.boardType->count > 0) {
        printf("%u\n", boardInfo.efuse.board_type);
    }
    if (getSlaveInfoArgs.serialNum->count > 0) {
        printf("%u\n", boardInfo.efuse.serial_number);
    }
    if (getSlaveInfoArgs.versionHW->count > 0) {
        printf("%.*s\n", 4, boardInfo.efuse.hardware_version);
    }
    if (getSlaveInfoArgs.dateCode->count > 0) {
        printf("%lli\n", boardInfo.efuse.timestamp);
    }
    if (getSlaveInfoArgs.versionSW->count > 0) {
        printf("%s\n", boardInfo.software_version);
    }

    return 0;
}

int CLI_Controller::_registerGetSlaveInfoCmd(void)
{
    getSlaveInfoArgs.type = arg_int1(NULL, NULL, "<TYPE>", "Board type");
    getSlaveInfoArgs.sn = arg_int1(NULL, NULL, "<SN>", "Board serial number");
    getSlaveInfoArgs.boardType = arg_lit0("t", "type", "Board type");
    getSlaveInfoArgs.serialNum = arg_lit0("n", "serial-num","Serial number");
    getSlaveInfoArgs.versionHW = arg_lit0("h", "version-hw", "Hardware version");
    getSlaveInfoArgs.dateCode = arg_lit0("d", "timestamp", "Board timestamp");
    getSlaveInfoArgs.versionSW = arg_lit0("s", "version-sw", "Software version");
    getSlaveInfoArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "get-slave-info",
        .help = "Get info from a slave board",
        .hint = NULL,
        .func = &getSlaveInfoCmd,
        .argtable = &pingArgs
    };

    if (esp_console_cmd_register(&cmd) == ESP_OK) {
        return 0;
    } else {
        return -1;
    }
}

#endif

#if !defined(MODULE_STANDALONE)

/* --- stop --- */

static int moduleStopCmd(int argc, char **argv)
{
#if defined(MODULE_MASTER)
    ControllerMaster::stop();
#elif defined(MODULE_SLAVE)
    ControllerSlave::stop();
#endif
    return 0;
}

int CLI_Controller::_registerStopCmd(void)
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

/* --- start --- */

static int moduleStartCmd(int argc, char **argv)
{
#if defined(MODULE_MASTER)
    ControllerMaster::start();
#elif defined(MODULE_SLAVE)
    ControllerSlave::start();
#endif
    return 0;
}

int CLI_Controller::_registerStartCmd(void)
{
    const esp_console_cmd_t cmd = {
        .command = "start",
        .help = "Start module tasks",
        .hint = NULL,
        .func = &moduleStartCmd,
        .argtable = NULL
    };
    
    if (esp_console_cmd_register(&cmd) == ESP_OK) {
        return 0;
    } else {
        return -1;
    }
}

/* --- get-status --- */

static int moduleGetStatusCmd(int argc, char **argv)
{
    int state;

#if defined(MODULE_MASTER)
    state = ControllerMaster::getStatus();
#elif defined(MODULE_SLAVE)
    state = ControllerSlave::getStatus();
#else 
    state = STATE_UNDEFINED;
#endif

    switch (state) {
        case STATE_IDLE:
            printf("Module state: IDLE\n");
            break;
        case STATE_RUNNING:
            printf("Module state: RUNNING\n");
            break;
        case STATE_ERROR:
            printf("Module state: ERROR\n");
            break;
        default:
            printf("Module state: UNDEFINED\n");
            break;
    }

    return 0;
}

int CLI_Controller::_registerGetStatusCmd(void)
{
    const esp_console_cmd_t cmd = {
        .command = "status",
        .help = "Get module status",
        .hint = NULL,
        .func = &moduleGetStatusCmd,
        .argtable = NULL
    };
    
    if (esp_console_cmd_register(&cmd) == ESP_OK) {
        return 0;
    } else {
        return -1;
    }
}

#endif