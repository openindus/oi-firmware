/**
 * @file DigitalOutputs_CLI.cpp
 * @brief Command line interface - Digital Outputs
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "DigitalOutputs.h"
#include "esp_log.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"

static struct {
    struct arg_int *dout;
    struct arg_int *level;
    struct arg_end *end;
} _digitalWriteArgs;

static int _digitalWrite(int argc, char **argv)
{
    int err = arg_parse(argc, argv, (void **) &_digitalWriteArgs);
    if (err != 0) {
        arg_print_errors(stderr, _digitalWriteArgs.end, argv[0]);
        return -1;
    }

    DOut_Num_t dout = (DOut_Num_t)(_digitalWriteArgs.dout->ival[0] - 1);
    uint8_t level = (uint8_t)_digitalWriteArgs.level->ival[0];

    DigitalOutputs::digitalWrite(dout, level);

    return 0;
}

static int _registerDigitalWrite(void)
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

static int _getOutputCurrent(int argc, char **argv)
{
    int err = arg_parse(argc, argv, (void **) &_digitalGetArgs);
    if (err != 0) {
        arg_print_errors(stderr, _digitalGetArgs.end, argv[0]);
        return -1;
    }

    DOut_Num_t dout = (DOut_Num_t)(_digitalGetArgs.dout->ival[0] - 1);

    printf("%.3f\n", DigitalOutputs::getOutputCurrent(dout));

    return 0;
}

static int _registerGetOutputCurrent(void)
{
    _digitalGetArgs.dout = arg_int1(NULL, NULL, "<DOUT>", "[1-4]");
    _digitalGetArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "get-ouput-current",
        .help = "Get output current",
        .hint = NULL,
        .func = &_getOutputCurrent,
        .argtable = &_digitalGetArgs
    };
    return esp_console_cmd_register(&cmd);
}

static int _outputIsOvercurrent(int argc, char **argv)
{
    int err = arg_parse(argc, argv, (void **) &_digitalGetArgs);
    if (err != 0) {
        arg_print_errors(stderr, _digitalGetArgs.end, argv[0]);
        return -1;
    }

    DOut_Num_t dout = (DOut_Num_t)(_digitalGetArgs.dout->ival[0] - 1);

    printf("%d\n", DigitalOutputs::outputIsOvercurrent(dout));

    return 0;
}

static int _registerOuputIsOvercurrent(void)
{
    _digitalGetArgs.dout = arg_int1(NULL, NULL, "<DOUT>", "[1-4]");
    _digitalGetArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "output-is-overcurrent",
        .help = "Check if output is overcurrent",
        .hint = NULL,
        .func = &_outputIsOvercurrent,
        .argtable = &_digitalGetArgs
    };
    return esp_console_cmd_register(&cmd);
}

int DigitalOutputs::_registerCLI(void)
{
    int err = 0;
    err |= _registerDigitalWrite();
    err |= _registerGetOutputCurrent();
    return err;
};
