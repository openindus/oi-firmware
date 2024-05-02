/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file AnalogInputsLVCLI.h
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "AnalogInputsLVCLI.h"

int AnalogInputsLVCLI::init() {
    int err = 0;

    err |= _registerAnalogInputMode();
    err |= _registerAnalogInputVoltageRange();
    err |= _registerAnalogRead();

    return err;
}

static struct {
    struct arg_int *ain;
    struct arg_int *mode;
    struct arg_end *end;
} _analogInputModeArgs;

int _analogInputMode(int argc, char **argv)
{
    int err = arg_parse(argc, argv, (void **) &_analogInputModeArgs);
    if (err != 0) {
        arg_print_errors(stderr, _analogInputModeArgs.end, argv[0]);
        return -1;
    }

    AnalogInput_Num_t ain = (AnalogInput_Num_t)(_analogInputModeArgs.ain->ival[0] - 1);
    AnalogInput_Mode_t mode = (AnalogInput_Mode_t)(_analogInputModeArgs.mode->ival[0]);

    AnalogInputsLV::analogInputMode(ain, mode);

    return 0;
}

int AnalogInputsLVCLI::_registerAnalogInputMode()
{
    _analogInputModeArgs.ain = arg_int1(NULL, NULL, "<AIN>", "[1-4]");
    _analogInputModeArgs.mode = arg_int1(NULL, NULL, "<MODE>", "0 = Voltage, 1 = Current");
    _analogInputModeArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "analog-input-mode",
        .help = "Set Ain mode between Voltage and Current",
        .hint = NULL,
        .func = &_analogInputMode,
        .argtable = &_analogInputModeArgs
    };
    return esp_console_cmd_register(&cmd);
}

static struct {
    struct arg_int *ain;
    struct arg_int *range;
    struct arg_end *end;
} _analogInputVoltageRangeArgs;

int _analogInputVoltageRange(int argc, char **argv)
{
    int err = arg_parse(argc, argv, (void **) &_analogInputVoltageRangeArgs);
    if (err != 0) {
        arg_print_errors(stderr, _analogInputVoltageRangeArgs.end, argv[0]);
        return -1;
    }

    AnalogInput_Num_t ain = (AnalogInput_Num_t)(_analogInputVoltageRangeArgs.ain->ival[0] - 1);
    int range_int = _analogInputVoltageRangeArgs.range->ival[0];
    AnalogInput_VoltageRange_t range;

    switch (range_int)
    {
    case 6:
        range = AIN_VOLTAGE_RANGE_0_5V12;
        break;
    case 7:
        range = AIN_VOLTAGE_RANGE_0_2V56;
        break;
    case 8:
        range = AIN_VOLTAGE_RANGE_0_1V28;
        break;
    default:
        range = AIN_VOLTAGE_RANGE_0_10V24;
        break;
    }

    AnalogInputsLV::analogInputVoltageRange(ain, range);

    return 0;
}

int AnalogInputsLVCLI::_registerAnalogInputVoltageRange()
{
    _analogInputVoltageRangeArgs.ain = arg_int1(NULL, NULL, "<AIN>", "[1-4]");
    _analogInputVoltageRangeArgs.range = arg_int1(NULL, NULL, "<RANGE>", "5 = 0-10.24V, 6 = 0-5.12V, 7 = 0-2.56V, 8 = 0-1.28V");
    _analogInputVoltageRangeArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "analog-input-voltage-range",
        .help = "Set AIN voltage range",
        .hint = NULL,
        .func = &_analogInputVoltageRange,
        .argtable = &_analogInputVoltageRangeArgs
    };
    return esp_console_cmd_register(&cmd);
}

static struct {
    struct arg_int *ain;
    struct arg_int *unit;
    struct arg_end *end;
} _analogReadInputReadArgs;

int _AnalogRead(int argc, char **argv)
{
    int err = arg_parse(argc, argv, (void **) &_analogReadInputReadArgs);
    if (err != 0) {
        arg_print_errors(stderr, _analogReadInputReadArgs.end, argv[0]);
        return -1;
    }

    AnalogInput_Num_t ain = (AnalogInput_Num_t)(_analogReadInputReadArgs.ain->ival[0] - 1);
    AnalogInput_Unit_t unit = AIN_UNIT_MILLIVOLT;
    if (_analogReadInputReadArgs.unit->count == 1) {
        unit = (AnalogInput_Unit_t)(_analogReadInputReadArgs.unit->ival[0]);
    }

    switch (unit)
    {
    case AIN_UNIT_VOLT:
        printf("%.3f\n", AnalogInputsLV::analogReadVolt(ain));
        break;
    case AIN_UNIT_MILLIVOLT:
        printf("%.1f\n", AnalogInputsLV::analogReadMilliVolt(ain));
        break;
    case AIN_UNIT_AMP:
        printf("%.3f\n", AnalogInputsLV::analogReadAmp(ain));
        break;
    case AIN_UNIT_MILLIAMP:
        printf("%.3f\n", AnalogInputsLV::analogReadMilliAmp(ain));
        break;

    default:
        printf("Wrong unit\n");
        break;
    }

    return 0;
}

int AnalogInputsLVCLI::_registerAnalogRead()
{
    _analogReadInputReadArgs.ain = arg_int1(NULL, NULL, "<AIN>", "[1-4]");
    _analogReadInputReadArgs.unit = arg_int1(NULL, NULL, "<UNIT>", "0 = Raw, 1 = mV, 2 = mA, 3 = V, 4 = A");
    _analogReadInputReadArgs.end = arg_end(2);

    const esp_console_cmd_t read_cmd = {
        .command = "analog-read",
        .help = "Read AIN voltage or current in desired unit (default to mV)",
        .hint = NULL,
        .func = &_AnalogRead,
        .argtable = &_analogReadInputReadArgs
    };
    return esp_console_cmd_register(&read_cmd);
}