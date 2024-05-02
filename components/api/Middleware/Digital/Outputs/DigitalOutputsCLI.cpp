/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DigitalOutputsCLI.h
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "DigitalOutputsCLI.h"

int DigitalOutputsCLI::init() {
    int err = 0;

    err |= _registerDigitalWrite();
    err |= _registerDigitalGetCurrent();
    err |= _registerDigitalGetOverCurrentStatus();

    return err;
}

static struct {
    struct arg_int *dout;
    struct arg_int *level;
    struct arg_end *end;
} _digitalWriteArgs;

int _digitalWrite(int argc, char **argv)
{
    int err = arg_parse(argc, argv, (void **) &_digitalWriteArgs);
    if (err != 0) {
        arg_print_errors(stderr, _digitalWriteArgs.end, argv[0]);
        return -1;
    }

    DigitalOutputNum_t dout = (DigitalOutputNum_t)(_digitalWriteArgs.dout->ival[0] - 1);
    uint8_t level = (uint8_t)_digitalWriteArgs.level->ival[0];

    DigitalOutputs::digitalWrite(dout, level);

    return 0;
}

int DigitalOutputsCLI::_registerDigitalWrite(void)
{
    _digitalWriteArgs.dout = arg_int1(NULL, NULL, "<DOUT>", "[1-4]");
    _digitalWriteArgs.level = arg_int1(NULL, NULL, "<LEVEL>", "0 = LOW, 1 = HIGH");
    _digitalWriteArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "digital-write",
        .help = "Set DOUT level",
        .hint = NULL,
        .func = &_digitalWrite,
        .argtable = &_digitalWriteArgs
    };
    return esp_console_cmd_register(&cmd);
}

static struct {
    struct arg_int *dout;
    struct arg_end *end;
} _digitalGetArgs;

int _digitalGetCurrent(int argc, char **argv)
{
    int err = arg_parse(argc, argv, (void **) &_digitalGetArgs);
    if (err != 0) {
        arg_print_errors(stderr, _digitalGetArgs.end, argv[0]);
        return -1;
    }

    DigitalOutputNum_t dout = (DigitalOutputNum_t)(_digitalGetArgs.dout->ival[0] - 1);

    printf("%.3f\n", DigitalOutputs::digitalGetCurrent(dout));

    return 0;
}

int DigitalOutputsCLI::_registerDigitalGetCurrent(void)
{
    _digitalGetArgs.dout = arg_int1(NULL, NULL, "<DOUT>", "[1-4]");
    _digitalGetArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "digital-get-current",
        .help = "Get DOUT current",
        .hint = NULL,
        .func = &_digitalGetCurrent,
        .argtable = &_digitalGetArgs
    };
    return esp_console_cmd_register(&cmd);
}

int _digitalGetOverCurrentStatus(int argc, char **argv)
{
    int err = arg_parse(argc, argv, (void **) &_digitalGetArgs);
    if (err != 0) {
        arg_print_errors(stderr, _digitalGetArgs.end, argv[0]);
        return -1;
    }

    DigitalOutputNum_t dout = (DigitalOutputNum_t)(_digitalGetArgs.dout->ival[0] - 1);

    printf("%d\n", DigitalOutputs::digitalGetOverCurrentStatus(dout));

    return 0;
}

int DigitalOutputsCLI::_registerDigitalGetOverCurrentStatus(void)
{
    _digitalGetArgs.dout = arg_int1(NULL, NULL, "<DOUT>", "[1-4]");
    _digitalGetArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "digital-get-over-current-status",
        .help = "Get DOUT over current status",
        .hint = NULL,
        .func = &_digitalGetOverCurrentStatus,
        .argtable = &_digitalGetArgs
    };
    return esp_console_cmd_register(&cmd);
}