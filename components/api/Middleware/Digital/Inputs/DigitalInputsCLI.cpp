/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DigitalInputsCLI.h
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "DigitalInputsCLI.h"

int DigitalInputsCLI::init() {
    int err = 0;

    err |= _registerDigitalRead();

    return err;
}

static struct {
    struct arg_int *din;
    struct arg_end *end;
} _digitalReadArgs;

int _digitalRead(int argc, char **argv)
{
    int err = arg_parse(argc, argv, (void **) &_digitalReadArgs);
    if (err != 0) {
        arg_print_errors(stderr, _digitalReadArgs.end, argv[0]);
        return -1;
    }

    DigitalInputNum_t din = (DigitalInputNum_t)(_digitalReadArgs.din->ival[0] - 1);

    printf("%d\n", DigitalInputs::digitalRead(din));

    return 0;
}

int DigitalInputsCLI::_registerDigitalRead(void)
{
    _digitalReadArgs.din = arg_int1(NULL, NULL, "<DIN>", "[1-4]");
    _digitalReadArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "digital-read",
        .help = "Get DIN level",
        .hint = NULL,
        .func = &_digitalRead,
        .argtable = &_digitalReadArgs
    };
    return esp_console_cmd_register(&cmd);
}