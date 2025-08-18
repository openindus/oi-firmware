/**
 * @file RelaysCLI.h
 * @brief Command line interface for Relays module
 * @author Georges de Massol
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "Relays.h"
#include "esp_log.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"

static struct {
    struct arg_int *dout;
    struct arg_int *level;
    struct arg_end *end;
} _digitalWriteArgs;

static struct {
    struct arg_int *dout;
    struct arg_end *end;
} _digitalReadArgs;

static int _digitalWrite(int argc, char **argv)
{
    Relay_Num_t dout = (Relay_Num_t)(_digitalWriteArgs.dout->ival[0] - 1);
    uint8_t level = (uint8_t)_digitalWriteArgs.level->ival[0];
    return Relays::digitalWrite(dout, level);
}

static int _digitalRead(int argc, char **argv)
{
    Relay_Num_t dout = (Relay_Num_t)(_digitalReadArgs.dout->ival[0] - 1);
    return Relays::digitalRead(dout);
}

static int _relay_parse(int argc, char **argv)
{
    // try to parse as digitalWrite
    int err = arg_parse(argc, argv, (void **) &_digitalWriteArgs);
    if (err == 0){
        return _digitalWrite(argc, argv);
    }

    err = arg_parse(argc, argv, (void **) &_digitalReadArgs);
    if (err == 0){
        int val = _digitalRead(argc, argv);
        if (val == 0 || val == 1){
            printf("%d\n", val);
            return 0;
            }
        return val;
    }
    arg_print_errors(stderr, _digitalReadArgs.end, argv[0]);
    return -1;
}

static int _registerDigitalWrite(void)
{
    _digitalWriteArgs.dout = arg_int1(NULL, NULL, "<DOUT>", "[1-4]");
    _digitalWriteArgs.level = arg_int1(NULL, NULL, "<LEVEL>", "0 = LOW, 1 = HIGH");
    _digitalWriteArgs.end = arg_end(2);

    _digitalReadArgs.dout = arg_int1(NULL, NULL, "<DOUT>", "[1-4]");
    _digitalReadArgs.end = arg_end(1);

    const esp_console_cmd_t cmd = {
        .command = "relay",
        .help = "Set DOUT level",
        .hint = NULL,
        .func = &_relay_parse,
        .argtable = &_digitalWriteArgs,
        .func_w_context = NULL,
        .context = NULL
    };
    return esp_console_cmd_register(&cmd);
}

int Relays::_registerCLI(void)
{
    int err = 0;
    err |= _registerDigitalWrite();
    return err;
};
