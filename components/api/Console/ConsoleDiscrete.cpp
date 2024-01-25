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

#include "ConsoleDiscrete.h"

#if defined(CONFIG_DISCRETE) || defined(CONFIG_DISCRETE_VE)

void ConsoleDiscrete::registerCli(void)
{
    _registerDigitalWrite();
    _registerDigitalRead();
    _registerAnalogRead();
}

/** 'digital-write' */

static struct {
    struct arg_int *stor;
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

    DigitalOutputNum_t stor = (DigitalOutputNum_t)(digitalWriteArgs.stor->ival[0] -1);
    uint8_t level = (uint8_t)digitalWriteArgs.level->ival[0];

    DiscreteStandalone::digitalWrite(stor, level);

    return 0;
}

void ConsoleDiscrete::_registerDigitalWrite(void)
{
    digitalWriteArgs.stor = arg_int1(NULL, NULL, "<DOUT>", "[1-8]");
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
    struct arg_int *etor;
    struct arg_end *end;
} digitalReadArgs;

static int digitalRead(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &digitalReadArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, digitalReadArgs.end, argv[0]);
        return 1;
    }

    DigitalInputNum_t etor = (DigitalInputNum_t)(digitalReadArgs.etor->ival[0] - 1);

    printf("%d\n", DiscreteStandalone::digitalRead(etor));

    return 0;
}

void ConsoleDiscrete::_registerDigitalRead(void)
{
    digitalReadArgs.etor = arg_int1(NULL, NULL, "<DIN>", "[1-10]");
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
    struct arg_int *eana;
    struct arg_end *end;
} analogReadArgs;

static int analogRead(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &analogReadArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, analogReadArgs.end, argv[0]);
        return 1;
    }

    AnalogInputNum_t eana = (AnalogInputNum_t)(analogReadArgs.eana->ival[0] - 1);

    printf("%d\n", DiscreteStandalone::analogRead(eana));

    return 0;
}

void ConsoleDiscrete::_registerAnalogRead(void)
{
    analogReadArgs.eana = arg_int1(NULL, NULL, "<AIN>", "[1-3]");
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

#endif
