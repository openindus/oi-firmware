/**
 * @file DigitalInputsExp_CLI.cpp
 * @brief Command line interface - Digital  Inputs
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "DigitalInputsExp.h"
#include "esp_log.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"

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

    printf("%d\n", DigitalInputsExp::digitalRead(din));
    
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

int DigitalInputsExp::_registerCLI(void)
{
    int err = 0;
    err |= _registerDigitalRead();
    return err;
}
