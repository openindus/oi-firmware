/**
 * @file CLI_Board.cpp
 * @brief Command line interface - Module
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "CLI_Board.h"

/* --- set-board-info --- */

static struct {
    struct arg_int *boardType;
    struct arg_int *serialNum;
    struct arg_str *versionHW;
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

    uint16_t board_type = setBoardInfoArgs.boardType->ival[0];
    uint32_t serial_number = setBoardInfoArgs.serialNum->ival[0];
    char hardware_version[4];
    strncpy(hardware_version, setBoardInfoArgs.versionHW->sval[0], 4);
    int64_t timestamp = atoll(setBoardInfoArgs.timestamp->sval[0]);

    if (Board::setBoardInfo(board_type, serial_number, hardware_version, timestamp)) {
        return 0;
    }
    return -1;
}

int CLI_Board::_registerSetBoardInfoCmd(void)
{
    setBoardInfoArgs.boardType = arg_int1("t", "type", "TYPE", "Board type");
    setBoardInfoArgs.serialNum = arg_int1("n", "serial-num", "NUM", "Serial number");
    setBoardInfoArgs.versionHW = arg_str1("h", "version-hw", "VERSION", "Hardware version");
    setBoardInfoArgs.timestamp = arg_str1("d", "date", "DATE", "Board date as a timestamp");
    setBoardInfoArgs.end = arg_end(5);

    const esp_console_cmd_t cmd = {
        .command = "set-board-info",
        .help = "WARNING ! This operation can be done only once !\nSet board informations: \"type, serial number and hardware version\"\nExample: set-board-info -t 12 -n 259 -h AE01 -d 1715947670",
        .hint = NULL,
        .func = &setBoardInfoCmd,
        .argtable = &setBoardInfoArgs
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
    struct arg_lit *serialNum;
    struct arg_lit *versionHW;
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
    if (getBoardInfoArgs.serialNum->count > 0) {
        printf("%u\n", Board::getSerialNum());
    }
    if (getBoardInfoArgs.versionHW->count > 0) {
        char version[5];
        Board::getHardwareVersion(version);
        printf("%.*s\n", 4, version);
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

int CLI_Board::_registerGetBoardInfoCmd(void)
{
    getBoardInfoArgs.boardType = arg_lit0("t", "type", "Board type");
    getBoardInfoArgs.serialNum = arg_lit0("n", "serial-num","Serial number");
    getBoardInfoArgs.versionHW = arg_lit0("h", "version-hw", "Hardware version");
    getBoardInfoArgs.dateCode = arg_lit0("d", "date-code", "Date code");
    getBoardInfoArgs.versionSW = arg_lit0("s", "version-sw", "Software version");
    getBoardInfoArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "get-board-info",
        .help = "Get board informations: \"type, serial number, hardware and software version\"",
        .hint = NULL,
        .func = &getBoardInfoCmd,
        .argtable = &getBoardInfoArgs
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

int CLI_Board::_registerRestartCmd(void)
{
    const esp_console_cmd_t cmd = {
        .command = "restart",
        .help = "Software reset of the chip",
        .hint = NULL,
        .func = &restartCmd,
        .argtable = NULL
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

int CLI_Board::_registerLogCmd(void)
{
    logArgs.level = arg_str1(NULL, NULL, "<LEVEL>", "[NONE, ERROR, WARN, INFO, DEBUG, VERBOSE]");
    logArgs.tag = arg_str0(NULL, NULL, "<TAG>", "specific tag");
    logArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "log",
        .help = "set the log level",
        .hint = NULL,
        .func = &logCmd,
        .argtable = &logArgs
    };
    return esp_console_cmd_register(&cmd);
}