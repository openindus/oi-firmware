/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file ConsoleDiscrete::h
 * @brief OpenIndus console
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "ConsoleCore.h"

#if defined(CONFIG_CORE)

void ConsoleCore::registerCli(void)
{
    _registerDigitalWrite();
    _registerDigitalRead();
    _registerAnalogRead();
    _registerAnalogReadMillivolts();
}

/** 'digital-write' */

static struct {
    struct arg_int *dout;
    struct arg_int *level;
    struct arg_end *end;
} digitalWriteArgs;

static int digitalWrite(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &digitalWriteArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, digitalWriteArgs.end, argv[0]);
        return 1;
    }

    DigitalOutputNum_t dout = (DigitalOutputNum_t)(digitalWriteArgs.dout->ival[0] -1);
    uint8_t level = (uint8_t)digitalWriteArgs.level->ival[0];

    CoreStandalone::digitalWrite(dout, level);

    return 0;
}

void ConsoleCore::_registerDigitalWrite(void)
{
    digitalWriteArgs.dout = arg_int1(NULL, NULL, "<DOUT>", "[1-4]");
    digitalWriteArgs.level = arg_int1(NULL, NULL, "<LEVEL>", "0 = LOW, 1 = HIGH");
    digitalWriteArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "digital-write",
        .help = "Set DOUT level",
        .hint = NULL,
        .func = &digitalWrite,
        .argtable = &digitalWriteArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/** 'digital-read' */

static struct {
    struct arg_int *din;
    struct arg_end *end;
} digitalReadArgs;

static int digitalRead(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &digitalReadArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, digitalReadArgs.end, argv[0]);
        return 1;
    }

    DigitalInputNum_t din = (DigitalInputNum_t)(digitalReadArgs.din->ival[0] - 1);

    printf("%d\n", CoreStandalone::digitalRead(din));

    return 0;
}

void ConsoleCore::_registerDigitalRead(void)
{
    digitalReadArgs.din = arg_int1(NULL, NULL, "<DIN>", "[1-4]");
    digitalReadArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "digital-read",
        .help = "Get DIN level",
        .hint = NULL,
        .func = &digitalRead,
        .argtable = &digitalReadArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/** 'analog-read' */

static struct {
    struct arg_int *ain;
    struct arg_end *end;
} analogReadArgs;

static int analogRead(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &analogReadArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, analogReadArgs.end, argv[0]);
        return 1;
    }

    AnalogInputNum_t ain = (AnalogInputNum_t)(analogReadArgs.ain->ival[0] - 1);

    printf("%d\n", CoreStandalone::analogRead(ain));


    return 0;
}

void ConsoleCore::_registerAnalogRead(void)
{
    analogReadArgs.ain = arg_int1(NULL, NULL, "<AIN>", "[1-2]");
    analogReadArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "analog-read",
        .help = "Get analog input value (RAW)",
        .hint = NULL,
        .func = &analogRead,
        .argtable = &analogReadArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}


/** 'analog-read-millivolts' */

static int analogReadMilliVolts(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &analogReadArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, analogReadArgs.end, argv[0]);
        return 1;
    }

    AnalogInputNum_t ain = (AnalogInputNum_t)(analogReadArgs.ain->ival[0] - 1);

    printf("%f\n", CoreStandalone::analogReadMilliVolts(ain));


    return 0;
}

void ConsoleCore::_registerAnalogReadMillivolts(void)
{
    analogReadArgs.ain = arg_int1(NULL, NULL, "<AIN>", "[1-2]");
    analogReadArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "analog-read-millivolts",
        .help = "Get analog input value (MilliVolts)",
        .hint = NULL,
        .func = &analogReadMilliVolts,
        .argtable = &analogReadArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

#endif
