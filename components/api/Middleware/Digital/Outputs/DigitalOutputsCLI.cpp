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

#if defined(MODULE_STANDALONE) || defined(MODULE_MASTER)

int DigitalOutputsCLI::init() {
    int err = 0;

    err |= _registerDigitalWrite();
    err |= _registerDigitalGetCurrent();

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
} _digitalGetCurrentArgs;

int _digitalGetCurrent(int argc, char **argv)
{
    int err = arg_parse(argc, argv, (void **) &_digitalGetCurrentArgs);
    if (err != 0) {
        arg_print_errors(stderr, _digitalGetCurrentArgs.end, argv[0]);
        return -1;
    }

    DigitalOutputNum_t dout = (DigitalOutputNum_t)(_digitalGetCurrentArgs.dout->ival[0] - 1);

    printf("%.3f\n", DigitalOutputs::digitalGetCurrent(dout));

    return 0;
}

int DigitalOutputsCLI::_registerDigitalGetCurrent(void)
{
    _digitalGetCurrentArgs.dout = arg_int1(NULL, NULL, "<DOUT>", "[1-4]");
    _digitalGetCurrentArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "digital-get-current",
        .help = "Get Dout current",
        .hint = NULL,
        .func = &_digitalGetCurrent,
        .argtable = &_digitalGetCurrentArgs
    };
    return esp_console_cmd_register(&cmd);
}

#endif