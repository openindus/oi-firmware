/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file ConsoleMixed::h
 * @brief OpenIndus console
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "ConsoleMixed.h"

#if defined(CONFIG_MIXED)

void ConsoleMixed::registerCli(void)
{
    _registerDigitalWrite();
    _registerDigitalRead();
    _registerGetCurrent();
}

/** 'digital-write' */

static struct {
    struct arg_int *dout;
    struct arg_int *level;
    struct arg_end *end;
} digitalWriteArgs;

static int dWrite(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &digitalWriteArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, digitalWriteArgs.end, argv[0]);
        return 1;
    }

    DigitalOutputNum_t dout = (DigitalOutputNum_t)(digitalWriteArgs.dout->ival[0] - 1);
    uint8_t level = (uint8_t)digitalWriteArgs.level->ival[0];

    MixedStandalone::digitalWrite(dout, level);

    return 0;
}

void ConsoleMixed::_registerDigitalWrite(void)
{
    digitalWriteArgs.dout = arg_int1(NULL, NULL, "<DOUT>", "[1-8]");
    digitalWriteArgs.level = arg_int1(NULL, NULL, "<LEVEL>", "0 = LOW, 1 = HIGH");
    digitalWriteArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "digital-write",
        .help = "Set DOUT level",
        .hint = NULL,
        .func = &dWrite,
        .argtable = &digitalWriteArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/** 'digital-read' */

static struct {
    struct arg_int *din;
    struct arg_end *end;
} digitalReadArgs;

static int dRead(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &digitalReadArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, digitalReadArgs.end, argv[0]);
        return 1;
    }

    DigitalInputNum_t din = (DigitalInputNum_t)(digitalReadArgs.din->ival[0] - 1);

    printf("%d\n", MixedStandalone::digitalRead(din));

    return 0;
}

void ConsoleMixed::_registerDigitalRead(void)
{
    digitalReadArgs.din = arg_int1(NULL, NULL, "<DIN>", "[1-10]");
    digitalReadArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "digital-read",
        .help = "Get DIN level",
        .hint = NULL,
        .func = &dRead,
        .argtable = &digitalReadArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}


/** 'get-current' */

static struct {
    struct arg_int *dout;
    struct arg_end *end;
} getCurrentArgs;

static int getCurrent(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &getCurrentArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, getCurrentArgs.end, argv[0]);
        return 1;
    }

    DigitalOutputNum_t dout = (DigitalOutputNum_t)(getCurrentArgs.dout->ival[0] - 1);

    printf("%.3f\n", DiscreteStandalone::getCurrent(dout));

    return 0;
}

void ConsoleMixed::_registerGetCurrent(void)
{
    getCurrentArgs.dout = arg_int1(NULL, NULL, "<DOUT>", "[1-8]");
    getCurrentArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "get-current",
        .help = "Get Dout current",
        .hint = NULL,
        .func = &getCurrent,
        .argtable = &getCurrentArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}