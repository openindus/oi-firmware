/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file ModuleCLI.h
 * @brief OpenIndus console
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "ModuleCLI.h"

void ModuleCLI::init(void)
{
    _registerSetBoardInfo();
    _registerGetBoardInfo();
    _registerRestart();
    _registerLog();
    _registerLed();
    _registerReadId();
    _registerWriteSync();
    _registerReadSync();
}

/** 'restart' */

static int restart(int argc, char **argv)
{
    esp_restart();
    return 0;
}

void ModuleCLI::_registerRestart(void)
{
    const esp_console_cmd_t cmd = {
        .command = "restart",
        .help = "Software reset of the chip",
        .hint = NULL,
        .func = &restart,
        .argtable = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/** 'set-board-info' */

static struct {
    struct arg_int *boardType;
    struct arg_int *serialNum;
    struct arg_str *versionHW;
    struct arg_str *versionSW;
    struct arg_end *end;
} setBoardInfoArgs;

static int setBoardInfo(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &setBoardInfoArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, setBoardInfoArgs.end, argv[0]);
        return 1;
    }

    if (setBoardInfoArgs.boardType->count > 0) {
        ModuleStandalone::setBoardType(setBoardInfoArgs.boardType->ival[0]);
    }
    if (setBoardInfoArgs.serialNum->count > 0) {
        ModuleStandalone::setSerialNum(setBoardInfoArgs.serialNum->ival[0]);
    }

    return 0;
}

void ModuleCLI::_registerSetBoardInfo(void)
{
    setBoardInfoArgs.boardType = arg_int0("t", "type", "TYPE", "Board type");
    setBoardInfoArgs.serialNum = arg_int0("n", "serial-num", "NUM", "Serial number");
    setBoardInfoArgs.versionHW = arg_str0("h", "version-hw", "VERSION", "Hardware version");
    setBoardInfoArgs.versionSW = arg_str0("s", "version-sw", "VERSION", "Software version");
    setBoardInfoArgs.end = arg_end(5);

    const esp_console_cmd_t cmd = {
        .command = "set-board-info",
        .help = "Set board informations: \"type, serial number, hardware and software version\"",
        .hint = NULL,
        .func = &setBoardInfo,
        .argtable = &setBoardInfoArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/** 'get-board-info' */

static struct {
    struct arg_lit *boardType;
    struct arg_lit *serialNum;
    struct arg_lit *versionHW;
    struct arg_lit *versionSW;
    struct arg_end *end;
} getBoardInfoArgs;

static int getBoardInfo(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &getBoardInfoArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, getBoardInfoArgs.end, argv[0]);
        return 1;
    }

    if (getBoardInfoArgs.boardType->count > 0) {
        printf("%d\n", ModuleStandalone::getBoardType());
    }
    if (getBoardInfoArgs.serialNum->count > 0) {
        printf("%d\n", ModuleStandalone::getSerialNum());
    }
    if (getBoardInfoArgs.versionHW->count > 0) {
        printf("AA00\n");
    }
    if (getBoardInfoArgs.versionSW->count > 0) {
        printf("v0.0.0\n");
    }

    return 0;
}

void ModuleCLI::_registerGetBoardInfo(void)
{
    getBoardInfoArgs.boardType = arg_lit0(NULL, "type", "Board type");
    getBoardInfoArgs.serialNum = arg_lit0(NULL, "serial-num","Serial number");
    getBoardInfoArgs.versionHW = arg_lit0(NULL, "version-hw", "Hardware version");
    getBoardInfoArgs.versionSW = arg_lit0(NULL, "version-sw", "Software version");
    getBoardInfoArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "get-board-info",
        .help = "Get board informations: \"type, serial number, hardware and software version\"",
        .hint = NULL,
        .func = &getBoardInfo,
        .argtable = &getBoardInfoArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/** 'led' */

static struct {
    struct arg_str *state;
    struct arg_int *color;
    struct arg_end *end;
} ledArgs;

static int led(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &ledArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, ledArgs.end, argv[0]);
        return 1;
    }

    if(strcmp(ledArgs.state->sval[0], "on") == 0) {
        if (ledArgs.color->count > 0) {
            ModuleStandalone::ledOn((LedColor_t)ledArgs.color->ival[0]);
        } else {
            ModuleStandalone::ledOn(LED_BLUE);
        }
    } else if (strcmp(ledArgs.state->sval[0], "off") == 0) {
        ModuleStandalone::ledOff();
    } else if (strcmp(ledArgs.state->sval[0], "blink") == 0) {
        if (ledArgs.color->count > 0) {
            ModuleStandalone::ledBlink((LedColor_t)ledArgs.color->ival[0], 1000);
        }
    } else {
        arg_print_errors(stderr, ledArgs.end, argv[0]);
        return 2;
    }
    return 0;
}

void ModuleCLI::_registerLed(void)
{
    ledArgs.state = arg_str1(NULL, NULL, "<STATE>", "[on, off, blink]");
    ledArgs.color = arg_int0(NULL, NULL, "color", "[0: None, 1: red, 2: green, 3: yellow, 4: blue, 5: purple, 6: cyan, 7; white]");
    ledArgs.end = arg_end(6);

    const esp_console_cmd_t cmd = {
        .command = "led",
        .help = "Change led state",
        .hint = NULL,
        .func = &led,
        .argtable = &ledArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));

}

/** 'log' */

static struct {
    struct arg_str *level;
    struct arg_str *tag;
    struct arg_end *end;
} logArgs;

static int log(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &logArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, logArgs.end, argv[0]);
        return 1;
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
        return 2;
    }

    return 0;
}

void ModuleCLI::_registerLog(void)
{
    logArgs.level = arg_str1(NULL, NULL, "<LEVEL>", "[NONE, ERROR, WARN, INFO, DEBUG, VERBOSE]");
    logArgs.tag = arg_str0(NULL, NULL, "<TAG>", "specific tag");
    logArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "log",
        .help = "set the log level",
        .hint = NULL,
        .func = &log,
        .argtable = &logArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/** 'read-id' */

static int readId(int argc, char **argv) 
{
    printf("%umV\n", BusIO::readId());
    return 0;
}

void ModuleCLI::_registerReadId(void)
{
    const esp_console_cmd_t cmd = {
        .command = "read-id",
        .help = "Read the bus ID of the device",
        .hint = NULL,
        .func = &readId,
        .argtable = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/** 'write-sync' */

static struct {
    struct arg_int *level;
    struct arg_end *end;
} writeSyncArgs;

static int writeSync(int argc, char **argv) 
{
    int nerrors = arg_parse(argc, argv, (void **) &writeSyncArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, writeSyncArgs.end, argv[0]);
        return 1;
    }

    BusIO::writeSync((uint8_t)writeSyncArgs.level->ival[0]);

    return 0;
}

void ModuleCLI::_registerWriteSync(void)
{
    writeSyncArgs.level = arg_int1(NULL, NULL, "<LEVEL>", "0 = LOW, 1 = HIGH");
    writeSyncArgs.end = arg_end(1);

    const esp_console_cmd_t cmd = {
        .command = "write-sync",
        .help = "set sync pin level",
        .hint = NULL,
        .func = &writeSync,
        .argtable = &writeSyncArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/** 'read-sync' */


static int readSync(int argc, char **argv) 
{
    printf("%u\n", BusIO::readSync());

    return 0;
}

void ModuleCLI::_registerReadSync(void)
{
    const esp_console_cmd_t cmd = {
        .command = "read-sync",
        .help = "get sync pin level",
        .hint = NULL,
        .func = &readSync,
        .argtable = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}