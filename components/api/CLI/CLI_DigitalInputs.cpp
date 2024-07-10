/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file CLI_DigitalInputs.h
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "CLI.h"
#include "DigitalInputs.h"
#include "DigitalInputsExp.h"

#if defined(OI_CORE) || defined(OI_DISCRETE) || defined(OI_MIXED) || defined(OI_STEPPER) || defined(OI_DC)

static struct {
    struct arg_int *din;
    struct arg_end *end;
} _digitalReadArgs;

static int _digitalRead(int argc, char **argv)
{
    int err = arg_parse(argc, argv, (void **) &_digitalReadArgs);
    if (err != 0) {
        arg_print_errors(stderr, _digitalReadArgs.end, argv[0]);
        return -1;
    }

    DIn_Num_t din = (DIn_Num_t)(_digitalReadArgs.din->ival[0] - 1);

#if !defined(OI_CORE)
    printf("%d\n", DigitalInputs::digitalRead(din));
#else
    printf("%d\n", DigitalInputsExp::digitalRead(din));
#endif

    return 0;
}

static int _registerDigitalRead(void)
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

int CLI::_registerDigitalInputsCmd(void)
{
    int err = 0;
    err |= _registerDigitalRead();
    return err;
}

#endif