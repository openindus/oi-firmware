/**
 * @file MasterCLI.cpp
 * @brief Command line interface - Master
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "Master.h"
#include "Common.h"
#include "argtable3/argtable3.h"

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

    Master::program(type, sn);

    return 0;
}

static int _registerProgramCmd(void)
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
    if (Master::ping(type, sn)) {
        t = esp_timer_get_time();
        printf("Ping module: %u time: %lld us\n", sn, (t-t0));
    } else {
        printf("Cannot ping module: %u\n", sn);
        return 2;
    }
    return 0;
}

static int _registerPingCmd(void)
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

/* --- get-slave-id --- */

static struct {
    struct arg_int *type;
    struct arg_int *sn;
    struct arg_end *end;
} getSlaveIdArgs;

static int getSlaveIdCmd(int argc, char **argv)
{
    uint16_t type;
    uint32_t sn;
    uint16_t id;

    int nerrors = arg_parse(argc, argv, (void **) &getSlaveIdArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, getSlaveIdArgs.end, argv[0]);
        return 1;
    }

    type = getSlaveIdArgs.type->ival[0];
    sn = getSlaveIdArgs.sn->ival[0];

    id = Master::getSlaveId(type, sn);
    
    if (id != 0) {
        printf("Slave ID: %u\n", id);
    } else {
        printf("No slave found for board type %u with SN %u\n", type, sn);
        return 2;
    }
    
    return 0;
}

static int _registerGetSlaveIdCmd(void)
{
    getSlaveIdArgs.type = arg_int1(NULL, NULL, "<TYPE>", "Board type");
    getSlaveIdArgs.sn = arg_int1(NULL, NULL, "<SN>", "Board serial number");
    getSlaveIdArgs.end = arg_end(1);
    const esp_console_cmd_t cmd = {
        .command = "get-slave-id",
        .help = "Get slave ID from board type and serial number",
        .hint = NULL,
        .func = &getSlaveIdCmd,
        .argtable = &getSlaveIdArgs
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
    Master::autoId();
    return 0;
}

static int _registerAutoIdCmd(void)
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
    
    std::map<uint16_t,std::pair<uint16_t, uint32_t>,std::greater<uint16_t>> _ids = Master::discoverSlaves();

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

static int _registerDiscoverSlavesCmd(void)
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
} getBoardInfoArgs;

static int getBoardInfoCmd(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &getBoardInfoArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, getBoardInfoArgs.end, argv[0]);
        return 1;
    }

    Board_Info_t boardInfo;

    if (getBoardInfoArgs.type->count == 1 && getBoardInfoArgs.sn->count == 1) {
        Master::getBoardInfo((uint16_t)getBoardInfoArgs.type->ival[0], (uint32_t)getBoardInfoArgs.sn->ival[0], &boardInfo);
    } else {
        return -1;
    }

    if (getBoardInfoArgs.boardType->count > 0) {
        printf("%u\n", boardInfo.efuse.board_type);
    }
    if (getBoardInfoArgs.versionHW->count > 0) {
        printf("%u\n", boardInfo.efuse.hardware_variant);
    }
    if (getBoardInfoArgs.serialNum->count > 0) {
        printf("%u\n", boardInfo.efuse.serial_number);
    }
    if (getBoardInfoArgs.dateCode->count > 0) {
        printf("%lli\n", boardInfo.efuse.timestamp);
    }
    if (getBoardInfoArgs.versionSW->count > 0) {
        printf("%s\n", boardInfo.software_version);
    }

    return 0;
}

static int _registerGetSlaveInfoCmd(void)
{
    getBoardInfoArgs.type = arg_int1(NULL, NULL, "<TYPE>", "Board type");
    getBoardInfoArgs.sn = arg_int1(NULL, NULL, "<SN>", "Board serial number");
    getBoardInfoArgs.boardType = arg_lit0("t", "type", "Board type");
    getBoardInfoArgs.serialNum = arg_lit0("n", "serial-num","Serial number");
    getBoardInfoArgs.versionHW = arg_lit0("h", "version-hw", "Hardware version");
    getBoardInfoArgs.dateCode = arg_lit0("d", "timestamp", "Board timestamp");
    getBoardInfoArgs.versionSW = arg_lit0("s", "version-sw", "Software version");
    getBoardInfoArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "get-slave-info",
        .help = "Get board information from slave",
        .hint = NULL,
        .func = &getBoardInfoCmd,
        .argtable = &pingArgs
    };

    if (esp_console_cmd_register(&cmd) == ESP_OK) {
        return 0;
    } else {
        return -1;
    }
}

/* --- stop --- */

static int masterStopCmd(int argc, char **argv)
{
    Master::stop();
    return 0;
}

static int _registerStopCmd(void)
{
    const esp_console_cmd_t cmd = {
        .command = "stop-master",
        .help = "Stop master tasks",
        .hint = NULL,
        .func = &masterStopCmd,
        .argtable = NULL
    };
    
    if (esp_console_cmd_register(&cmd) == ESP_OK) {
        return 0;
    } else {
        return -1;
    }
}

/* --- start --- */

static int masterStartCmd(int argc, char **argv)
{
    Master::start();
    return 0;
}

static int _registerStartCmd(void)
{
    const esp_console_cmd_t cmd = {
        .command = "start-master",
        .help = "Start master tasks",
        .hint = NULL,
        .func = &masterStartCmd,
        .argtable = NULL
    };
    
    if (esp_console_cmd_register(&cmd) == ESP_OK) {
        return 0;
    } else {
        return -1;
    }
}

/* --- get-status --- */

static int masterGetStatusCmd(int argc, char **argv)
{
    int status;

    status = Master::getStatus();

    switch (status) {
        case STATE_IDLE:
            printf("Master status: IDLE\n");
            break;
        case STATE_RUNNING:
            printf("Master status: RUNNING\n");
            break;
        case STATE_ERROR:
            printf("Master status: ERROR\n");
            break;
        default:
            printf("Master status: UNDEFINED\n");
            break;
    }

    return 0;
}

static int _registerGetStatusCmd(void)
{
    const esp_console_cmd_t cmd = {
        .command = "status-master",
        .help = "Get master status",
        .hint = NULL,
        .func = &masterGetStatusCmd,
        .argtable = NULL
    };
    
    if (esp_console_cmd_register(&cmd) == ESP_OK) {
        return 0;
    } else {
        return -1;
    }
}

/* --- run-callback --- */

static struct {
    struct arg_int *id;
    struct arg_int *callback;
    struct arg_int *args;
    struct arg_end *end;
} runCallbackArgs;

static int runCallbackCmd(int argc, char **argv)
{
    uint16_t id;
    uint8_t callback;
    std::vector<uint8_t> args;

    int nerrors = arg_parse(argc, argv, (void **) &runCallbackArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, runCallbackArgs.end, argv[0]);
        return 1;
    }

    id = runCallbackArgs.id->ival[0];
    callback = runCallbackArgs.callback->ival[0];
    args.push_back(runCallbackArgs.args->ival[0]);

    Master::runCallback(id, callback, args);

    return 0;
}

static int _registerRunCallback(void)
{
    runCallbackArgs.id = arg_int1(NULL, NULL, "<ID>", "Slave ID");
    runCallbackArgs.callback = arg_int1(NULL, NULL, "<CB>", "Callback ID");
    runCallbackArgs.args = arg_int1(NULL, NULL, "<ARGS>", "Arguments");
    runCallbackArgs.end = arg_end(1);
    const esp_console_cmd_t cmd = {
        .command = "run-callback",
        .help = "Run a callback function on a slave",
        .hint = NULL,
        .func = &runCallbackCmd,
        .argtable = &runCallbackArgs
    };

    if (esp_console_cmd_register(&cmd) == ESP_OK) {
        return 0;
    } else {
        return -1;
    }
}

/* --- module-restart --- */

static struct {
    struct arg_int *id;
    struct arg_end *end;
} moduleRestartArgs;

static int moduleRestartCmd(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &moduleRestartArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, moduleRestartArgs.end, argv[0]);
        return 1;
    }
    uint16_t slaveId = moduleRestartArgs.id->ival[0];
    Master::moduleRestart(slaveId);
    printf("Restart command sent to module ID: %u\n", slaveId);
    return 0;
}

static int _registerModuleRestartCmd(void)
{
    moduleRestartArgs.id = arg_int1("i", "id", "<ID>", "Slave ID");
    moduleRestartArgs.end = arg_end(1);
    const esp_console_cmd_t cmd = {
        .command = "module-restart",
        .help = "Restart a slave module by ID",
        .hint = NULL,
        .func = &moduleRestartCmd,
        .argtable = &moduleRestartArgs
    };
    return esp_console_cmd_register(&cmd);
}

int Master::_registerCLI(void)
{
    int err = 0;
    err |= _registerPingCmd();
    err |= _registerProgramCmd();
    err |= _registerGetSlaveIdCmd();
    err |= _registerAutoIdCmd();
    err |= _registerDiscoverSlavesCmd();
    err |= _registerGetSlaveInfoCmd();
    err |= _registerStopCmd();
    err |= _registerStartCmd();
    err |= _registerGetStatusCmd();
    err |= _registerRunCallback();
    err |= _registerModuleRestartCmd();
    return err;
}

#endif