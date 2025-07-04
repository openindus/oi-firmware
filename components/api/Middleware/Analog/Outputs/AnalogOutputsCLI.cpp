/**
 * @file AnalogOutputsCLI.cpp
 * @brief Command line interface - Anlog Outputs
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "AnalogOutputs.h"
#include "esp_log.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"

static struct {
    struct arg_int *aout;
    struct arg_int *mode;
    struct arg_end *end;
} _analogOutputModeArgs;

static int _analogOutputMode(int argc, char **argv)
{
    int err = arg_parse(argc, argv, (void **) &_analogOutputModeArgs);
    if (err != 0) {
        arg_print_errors(stderr, _analogOutputModeArgs.end, argv[0]);
        return -1;
    }

    AnalogOutput_Num_t ain = (AnalogOutput_Num_t)(_analogOutputModeArgs.aout->ival[0] - 1);
    AnalogOutput_Mode_t mode = (AnalogOutput_Mode_t)(_analogOutputModeArgs.mode->ival[0]);

    AnalogOutputs::analogOutputMode(ain, mode);

    return 0;
}

static int _registerAnalogOutputMode()
{
    _analogOutputModeArgs.aout = arg_int1(NULL, NULL, "<AOUT>", "[1-2]");
    _analogOutputModeArgs.mode = arg_int1(NULL, NULL, "<MODE>", "0 = Voltage, 1 = Current");
    _analogOutputModeArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "analog-output-mode",
        .help = "Set AOUT mode between Voltage (-10V to 10V) and Current (0mA to 20mA)",
        .hint = NULL,
        .func = &_analogOutputMode,
        .argtable = &_analogOutputModeArgs
    };
    return esp_console_cmd_register(&cmd);
}

static struct {
    struct arg_int *aout;
    struct arg_dbl *value;
    struct arg_end *end;
} _analogOutputWriteArgs;

static int _analogOutputWrite(int argc, char **argv)
{
    int err = arg_parse(argc, argv, (void **) &_analogOutputWriteArgs);
    if (err != 0) {
        arg_print_errors(stderr, _analogOutputWriteArgs.end, argv[0]);
        return -1;
    }

    AnalogOutput_Num_t ain = (AnalogOutput_Num_t)(_analogOutputWriteArgs.aout->ival[0] - 1);
    float value = _analogOutputWriteArgs.value->dval[0];

    AnalogOutputs::analogWrite(ain, value);

    return 0;
}

static int _registerAnalogOutputWrite()
{
    _analogOutputWriteArgs.aout = arg_int1(NULL, NULL, "<AOUT>", "[1-2]");
    _analogOutputWriteArgs.value = arg_dbl1("v", "value", "<VALUE>", "In V for voltage or mA for current");
    _analogOutputWriteArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "analog-write",
        .help = "Set AOUT to specified voltage (-10V to 10V) or current (0mA to 20mA)",
        .hint = NULL,
        .func = &_analogOutputWrite,
        .argtable = &_analogOutputWriteArgs
    };
    return esp_console_cmd_register(&cmd);
}

int AnalogOutputs::_registerCLI(void) 
{
    int err = 0;
    err |= _registerAnalogOutputMode();
    err |= _registerAnalogOutputWrite();
    return err;
}