/**
 * @file BoardCLI.cpp
 * @brief Command line interface - Board
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "Board.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"

/* --- set-board-info --- */

static struct {
    struct arg_int *boardType;
    struct arg_int *hardwareVariant;
    struct arg_int *serialNum;
    struct arg_str *timestamp;
    struct arg_end *end;
} setBoardInfoArgs;

static int setBoardInfoCmd(int argc, char **argv)
{
    int err = arg_parse(argc, argv, (void **) &setBoardInfoArgs);
    if (err != 0) {
        arg_print_errors(stderr, setBoardInfoArgs.end, argv[0]);
        return -1;
    }

    uint8_t board_type = setBoardInfoArgs.boardType->ival[0];
    uint8_t hardware_variant = setBoardInfoArgs.hardwareVariant->ival[0];
    uint32_t serial_number = setBoardInfoArgs.serialNum->ival[0];
    int64_t timestamp = atoll(setBoardInfoArgs.timestamp->sval[0]);

    if (Board::setBoardInfo(board_type, hardware_variant, serial_number, timestamp)) {
        return 0;
    }
    return -1;
}

static int _registerSetBoardInfoCmd(void)
{
    setBoardInfoArgs.boardType = arg_int1("t", "type", "TYPE", "Board type");
    setBoardInfoArgs.hardwareVariant = arg_int1("h", "hardware variant", "VAR", "Hardware variant");
    setBoardInfoArgs.serialNum = arg_int1("n", "serial-num", "NUM", "Serial number");
    setBoardInfoArgs.timestamp = arg_str1("d", "date", "DATE", "Board date as a timestamp");
    setBoardInfoArgs.end = arg_end(5);

    const esp_console_cmd_t cmd = {
        .command = "set-board-info",
        .help = "WARNING ! This operation can be done only once !\nSet board informations: \"type, serial number and hardware version\"\nExample: set-board-info -t 12 -h 1 -n 259 -d 1715947670",
        .hint = NULL,
        .func = &setBoardInfoCmd,
        .argtable = &setBoardInfoArgs,
        .func_w_context = NULL,
        .context = NULL
    };

    if (esp_console_cmd_register(&cmd) == ESP_OK) {
        return 0;
    } else {
        return -1;
    }
}

/* --- get-board-info --- */

static struct {
    struct arg_lit *boardType;
    struct arg_lit *hardwareVariant;
    struct arg_lit *serialNum;
    struct arg_lit *dateCode;
    struct arg_lit *versionSW;
    struct arg_end *end;
} getBoardInfoArgs;

static int getBoardInfoCmd(int argc, char **argv)
{
    int err = arg_parse(argc, argv, (void **) &getBoardInfoArgs);
    if (err != 0) {
        arg_print_errors(stderr, getBoardInfoArgs.end, argv[0]);
        return -1;
    }

    if (getBoardInfoArgs.boardType->count > 0) {
        printf("%u\n", Board::getBoardType());
    }
    if (getBoardInfoArgs.hardwareVariant->count > 0) {
        printf("%u\n", Board::getHardwareVariant());
    }
    if (getBoardInfoArgs.serialNum->count > 0) {
        printf("%lu\n", Board::getSerialNum());
    }
    if (getBoardInfoArgs.dateCode->count > 0) {
        printf("%lli\n", Board::getTimestamp());
    }
    if (getBoardInfoArgs.versionSW->count > 0) {
        char version[32];
        Board::getSoftwareVersion(version);
        printf("%s\n", version);
    }

    return 0;
}

static int _registerGetBoardInfoCmd(void)
{
    getBoardInfoArgs.boardType = arg_lit0("t", "type", "Board type");
    getBoardInfoArgs.hardwareVariant = arg_lit0("h", "hardware-variant", "Hardware variant");
    getBoardInfoArgs.serialNum = arg_lit0("n", "serial-num","Serial number");
    getBoardInfoArgs.dateCode = arg_lit0("d", "date-code", "Date code");
    getBoardInfoArgs.versionSW = arg_lit0("s", "version-sw", "Software version");
    getBoardInfoArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "get-board-info",
        .help = "Get board informations: \"type, serial number, hardware and software version\"",
        .hint = NULL,
        .func = &getBoardInfoCmd,
        .argtable = &getBoardInfoArgs,
        .func_w_context = NULL,
        .context = NULL
    };

    if (esp_console_cmd_register(&cmd) == ESP_OK) {
        return 0;
    } else {
        return -1;
    }
}

/* --- restart --- */

static int restartCmd(int argc, char **argv)
{
    esp_restart();
    return 0;
}

static int _registerRestartCmd(void)
{
    const esp_console_cmd_t cmd = {
        .command = "restart",
        .help = "Software reset of the chip",
        .hint = NULL,
        .func = &restartCmd,
        .argtable = NULL,
        .func_w_context = NULL,
        .context = NULL
    };
    return esp_console_cmd_register(&cmd);
}

/* --- log --- */

static struct {
    struct arg_str *level;
    struct arg_str *tag;
    struct arg_end *end;
} logArgs;

static int logCmd(int argc, char **argv)
{
    int err = arg_parse(argc, argv, (void **) &logArgs);
    if (err != 0) {
        arg_print_errors(stderr, logArgs.end, argv[0]);
        return -1;
    }

    if (logArgs.tag->count == 0) {
        logArgs.tag->sval[0] = "*";
    } 

    printf("Setting log %s to %s\n", logArgs.tag->sval[0], logArgs.level->sval[0]);

    if(strcmp(logArgs.level->sval[0], "NONE") == 0) {
        esp_log_level_set(logArgs.tag->sval[0], ESP_LOG_NONE);
    } else if (strcmp(logArgs.level->sval[0], "ERROR") == 0) {
        esp_log_level_set(logArgs.tag->sval[0], ESP_LOG_ERROR);
    } else if (strcmp(logArgs.level->sval[0], "WARN") == 0) {
        esp_log_level_set(logArgs.tag->sval[0], ESP_LOG_WARN);
    } else if (strcmp(logArgs.level->sval[0], "INFO") == 0) {
        esp_log_level_set(logArgs.tag->sval[0], ESP_LOG_INFO);
    } else if (strcmp(logArgs.level->sval[0], "DEBUG") == 0) {
        esp_log_level_set(logArgs.tag->sval[0], ESP_LOG_DEBUG);
    } else if (strcmp(logArgs.level->sval[0], "VERBOSE") == 0) {
        esp_log_level_set(logArgs.tag->sval[0], ESP_LOG_VERBOSE);
    } else {
        arg_print_errors(stderr, logArgs.end, argv[0]);
        return -2;
    }

    return 0;
}

static int _registerLogCmd(void)
{
    logArgs.level = arg_str1(NULL, NULL, "<LEVEL>", "[NONE, ERROR, WARN, INFO, DEBUG, VERBOSE]");
    logArgs.tag = arg_str0(NULL, NULL, "<TAG>", "specific tag");
    logArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "log",
        .help = "set the log level",
        .hint = NULL,
        .func = &logCmd,
        .argtable = &logArgs,
        .func_w_context = NULL,
        .context = NULL
    };
    return esp_console_cmd_register(&cmd);
}

int Board::_registerCLI(void) 
{
    int err = 0;
    err |= _registerSetBoardInfoCmd();
    err |= _registerGetBoardInfoCmd();
    err |= _registerRestartCmd();
    err |= _registerLogCmd();
    return err;
}