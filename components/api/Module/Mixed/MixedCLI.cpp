/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file MixedCLI.h
 * @brief OpenIndus console
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "MixedCLI.h"

#if defined(OI_MIXED)

MixedStandalone* MixedCLI::_mixed = new MixedStandalone();

int MixedCLI::init(void)
{
    _registerDigitalWrite();
    _registerDigitalRead();
    _registerGetCurrent();
    _registerSetAnalogVoltageRange();
    _registerAnalogReadVoltage();
    return 0;
}

/******* Digital I/O CLI Commands ********/

static struct {
    struct arg_int *dout;
    struct arg_int *level;
    struct arg_end *end;
} _digitalWriteArgs;

int MixedCLI::_digitalWrite(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &_digitalWriteArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, _digitalWriteArgs.end, argv[0]);
        return 1;
    }

    DigitalOutputNum_t dout = (DigitalOutputNum_t)(_digitalWriteArgs.dout->ival[0] - 1);
    uint8_t level = (uint8_t)_digitalWriteArgs.level->ival[0];

    _mixed->digitalWrite(dout, level);

    return 0;
}

void MixedCLI::_registerDigitalWrite(void)
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
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

static struct {
    struct arg_int *din;
    struct arg_end *end;
} _digitalReadArgs;

int MixedCLI::_digitalRead(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &_digitalReadArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, _digitalReadArgs.end, argv[0]);
        return 1;
    }

    DigitalInputNum_t din = (DigitalInputNum_t)(_digitalReadArgs.din->ival[0] - 1);

    printf("%d\n", _mixed->digitalRead(din));

    return 0;
}

void MixedCLI::_registerDigitalRead(void)
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
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

static struct {
    struct arg_int *dout;
    struct arg_end *end;
} _getCurrentArgs;

int MixedCLI::_getCurrent(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &_getCurrentArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, _getCurrentArgs.end, argv[0]);
        return 1;
    }

    DigitalOutputNum_t dout = (DigitalOutputNum_t)(_getCurrentArgs.dout->ival[0] - 1);

    printf("%.3f\n", _mixed->getCurrent(dout));

    return 0;
}

void MixedCLI::_registerGetCurrent(void)
{
    _getCurrentArgs.dout = arg_int1(NULL, NULL, "<DOUT>", "[1-4]");
    _getCurrentArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "get-current",
        .help = "Get Dout current",
        .hint = NULL,
        .func = &_getCurrent,
        .argtable = &_getCurrentArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/******* Analog Input CLI Commands ************/

static struct {
    struct arg_int *ain;
    struct arg_int *range;
    struct arg_end *end;
} _setAnalogVoltageRangeArgs;

int MixedCLI::_setAnalogVoltageRange(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &_setAnalogVoltageRangeArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, _setAnalogVoltageRangeArgs.end, argv[0]);
        return 1;
    }

    AnalogInput_Num_t ain = (AnalogInput_Num_t)(_setAnalogVoltageRangeArgs.ain->ival[0] - 1);
    int range_int = _setAnalogVoltageRangeArgs.range->ival[0];
    AnalogInput_VoltageRange_t range;

    switch (range_int)
    {
    case 2:
        range = AIN_VOLTAGE_RANGE_0_5V12;
        break;
    case 3:
        range = AIN_VOLTAGE_RANGE_0_2V56;
        break;
    case 4:
        range = AIN_VOLTAGE_RANGE_0_1V28;
        break;
    default:
        range = AIN_VOLTAGE_RANGE_0_10V24;
        break;
    }

    _mixed->analogInputVoltageRange(ain, range);

    return 0;
}

void MixedCLI::_registerSetAnalogVoltageRange(void)
{
    _setAnalogVoltageRangeArgs.ain = arg_int1(NULL, NULL, "<AIN>", "[1-4]");
    _setAnalogVoltageRangeArgs.range = arg_int1(NULL, NULL, "<RANGE>", "1 = 0-10.24V, 2 = 0-5.12V, 3 = 0-2.56V, 4 = 0-1.28V");
    _setAnalogVoltageRangeArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "set-analog-voltage-range",
        .help = "Set Ain voltage range",
        .hint = NULL,
        .func = &_setAnalogVoltageRange,
        .argtable = &_setAnalogVoltageRangeArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

static struct {
    struct arg_int *ain;
    struct arg_end *end;
} _analogReadVoltageArgs;

int MixedCLI::_analogReadVoltage(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &_analogReadVoltageArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, _analogReadVoltageArgs.end, argv[0]);
        return 1;
    }

    AnalogInput_Num_t ain = (AnalogInput_Num_t)(_analogReadVoltageArgs.ain->ival[0] - 1);

    printf("%.3f\n", _mixed->analogReadMilliVolt(ain));

    return 0;
}

void MixedCLI::_registerAnalogReadVoltage(void)
{
    _analogReadVoltageArgs.ain = arg_int1(NULL, NULL, "<AIN>", "[1-4]");
    _analogReadVoltageArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "analog-read-voltage",
        .help = "Read Ain voltage",
        .hint = NULL,
        .func = &_analogReadVoltage,
        .argtable = &_analogReadVoltageArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

#endif