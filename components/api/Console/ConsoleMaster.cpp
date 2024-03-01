/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file ConsoleMaster.h
 * @brief OpenIndus console
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "ConsoleMaster.h"

#if defined(MODULE_MASTER)

void ConsoleMaster::registerCli(void)
{
    _registerPing();
    _registerProgram();
    _registerAutoId();
}

/** 'program' */

static struct {
    struct arg_int *sn;
    struct arg_end *end;
} programArgs;

static int program(int argc, char **argv)
{
    uint32_t sn;
    int nerrors = arg_parse(argc, argv, (void **) &programArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, programArgs.end, argv[0]);
        return 1;
    }
    sn = (uint32_t)programArgs.sn->ival[0];
    ModuleMaster::program(sn);
    return 0;
}

void ConsoleMaster::_registerProgram(void)
{
    programArgs.sn = arg_int1(NULL, NULL, "<SN>", "Board serial number");
    programArgs.end = arg_end(1);
    const esp_console_cmd_t cmd = {
        .command = "program",
        .help = "System in programming mode",
        .hint = NULL,
        .func = &program,
        .argtable = &programArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/** 'ping' */

static struct {
    struct arg_int *sn;
    struct arg_end *end;
} pingArgs;

static int ping(int argc, char **argv)
{
    uint32_t sn;
    int64_t t, t0;
    int nerrors = arg_parse(argc, argv, (void **) &pingArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, pingArgs.end, argv[0]);
        return 1;
    }
    sn = (uint32_t)pingArgs.sn->ival[0];
    t0 = esp_timer_get_time();
    if (ModuleMaster::ping(sn)) {
        t = esp_timer_get_time();
        printf("Ping module: %u time: %lld us\n", sn, (t-t0));
    } else {
        printf("Cannot ping module: %u\n", sn);
        return 2;
    }
    return 0;
}

void ConsoleMaster::_registerPing(void)
{
    pingArgs.sn = arg_int1(NULL, NULL, "<SN>", "Board serial number");
    pingArgs.end = arg_end(1);
    const esp_console_cmd_t cmd = {
        .command = "ping",
        .help = "ping a board",
        .hint = NULL,
        .func = &ping,
        .argtable = &pingArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/** 'auto-id' */

static int autoId(int argc, char **argv) 
{
    ModuleMaster::autoId();
    return 0;
}

void ConsoleMaster::_registerAutoId(void)
{
    const esp_console_cmd_t cmd = {
        .command = "auto-id",
        .help = "Send a frame to get all slaves IDs",
        .hint = NULL,
        .func = &autoId,
        .argtable = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

#endif
