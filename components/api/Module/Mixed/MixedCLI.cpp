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

int MixedCLI::init(void)
{
    _registerDigitalWrite();
    _registerDigitalRead();
    _registerDigitalGetCurrent();
    _registerSetAnalogInputMode();
    _registerSetAnalogInputVoltageRange();
    _registerAnalogInputRead();
    _registerSetAnalogOutputMode();
    _registerAnalogOutputWrite();
    return 0;
}

/******* Digital I/O CLI Commands ********/

static struct {
    struct arg_int *dout;
    struct arg_int *level;
    struct arg_end *end;
} _digitalWriteArgs;

int _digitalWrite(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &_digitalWriteArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, _digitalWriteArgs.end, argv[0]);
        return 1;
    }

    DigitalOutputNum_t dout = (DigitalOutputNum_t)(_digitalWriteArgs.dout->ival[0] - 1);
    uint8_t level = (uint8_t)_digitalWriteArgs.level->ival[0];

    MixedStandalone::digitalWrite(dout, level);

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

int _digitalRead(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &_digitalReadArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, _digitalReadArgs.end, argv[0]);
        return 1;
    }

    DigitalInputNum_t din = (DigitalInputNum_t)(_digitalReadArgs.din->ival[0] - 1);

    printf("%d\n", MixedStandalone::digitalRead(din));

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
} _digitalGetCurrentArgs;

int _digitalGetCurrent(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &_digitalGetCurrentArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, _digitalGetCurrentArgs.end, argv[0]);
        return 1;
    }

    DigitalOutputNum_t dout = (DigitalOutputNum_t)(_digitalGetCurrentArgs.dout->ival[0] - 1);

    printf("%.3f\n", MixedStandalone::digitalGetCurrent(dout));

    return 0;
}

void MixedCLI::_registerDigitalGetCurrent(void)
{
    _digitalGetCurrentArgs.dout = arg_int1(NULL, NULL, "<DOUT>", "[1-4]");
    _digitalGetCurrentArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "get-current",
        .help = "Get Dout current",
        .hint = NULL,
        .func = &_digitalGetCurrent,
        .argtable = &_digitalGetCurrentArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}


/******* Analog Input CLI Commands ************/

static struct {
    struct arg_int *ain;
    struct arg_int *mode;
    struct arg_end *end;
} _setAnalogInputModeArgs;

int _setAnalogInputMode(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &_setAnalogInputModeArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, _setAnalogInputModeArgs.end, argv[0]);
        return 1;
    }

    AnalogInput_Num_t ain = (AnalogInput_Num_t)(_setAnalogInputModeArgs.ain->ival[0] - 1);
    AnalogInput_Mode_t mode = (AnalogInput_Mode_t)(_setAnalogInputModeArgs.mode->ival[0]);

    MixedStandalone::analogInputMode(ain, mode);

    return 0;
}

void MixedCLI::_registerSetAnalogInputMode(void)
{
    _setAnalogInputModeArgs.ain = arg_int1(NULL, NULL, "<AIN>", "[1-4]");
    _setAnalogInputModeArgs.mode = arg_int1(NULL, NULL, "<MODE>", "0 = Voltage, 1 = Current");
    _setAnalogInputModeArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "set-analog-input-mode",
        .help = "Set Ain mode between Voltage and Current",
        .hint = NULL,
        .func = &_setAnalogInputMode,
        .argtable = &_setAnalogInputModeArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

static struct {
    struct arg_int *ain;
    struct arg_int *range;
    struct arg_end *end;
} _setAnalogInputVoltageRangeArgs;

int _setAnalogInputVoltageRange(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &_setAnalogInputVoltageRangeArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, _setAnalogInputVoltageRangeArgs.end, argv[0]);
        return 1;
    }

    AnalogInput_Num_t ain = (AnalogInput_Num_t)(_setAnalogInputVoltageRangeArgs.ain->ival[0] - 1);
    int range_int = _setAnalogInputVoltageRangeArgs.range->ival[0];
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

    MixedStandalone::analogInputVoltageRange(ain, range);

    return 0;
}

void MixedCLI::_registerSetAnalogInputVoltageRange(void)
{
    _setAnalogInputVoltageRangeArgs.ain = arg_int1(NULL, NULL, "<AIN>", "[1-4]");
    _setAnalogInputVoltageRangeArgs.range = arg_int1(NULL, NULL, "<RANGE>", "5 = 0-10.24V, 6 = 0-5.12V, 7 = 0-2.56V, 8 = 0-1.28V");
    _setAnalogInputVoltageRangeArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "set-analog-input-voltage-range",
        .help = "Set Ain voltage range",
        .hint = NULL,
        .func = &_setAnalogInputVoltageRange,
        .argtable = &_setAnalogInputVoltageRangeArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

static struct {
    struct arg_int *ain;
    struct arg_int *unit;
    struct arg_end *end;
} _analogReadInputReadArgs;

int _analogInputRead(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &_analogReadInputReadArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, _analogReadInputReadArgs.end, argv[0]);
        return 1;
    }

    AnalogInput_Num_t ain = (AnalogInput_Num_t)(_analogReadInputReadArgs.ain->ival[0] - 1);
    AnalogInput_Unit_t unit = (AnalogInput_Unit_t)(_analogReadInputReadArgs.unit->ival[0]);

    switch (unit)
    {
    case AIN_UNIT_VOLT:
        printf("%.3f\n", MixedStandalone::analogReadVolt(ain));
        break;
    case AIN_UNIT_MILLIVOLT:
        printf("%.1f\n", MixedStandalone::analogReadMilliVolt(ain));
        break;
    case AIN_UNIT_AMP:
        printf("%.3f\n", MixedStandalone::analogReadAmp(ain));
        break;
    case AIN_UNIT_MILLIAMP:
        printf("%.3f\n", MixedStandalone::analogReadMilliAmp(ain));
        break;

    default:
        printf("%d\n", MixedStandalone::analogRead(ain));
        break;
    }

    return 0;
}

void MixedCLI::_registerAnalogInputRead(void)
{
    _analogReadInputReadArgs.ain = arg_int1(NULL, NULL, "<AIN>", "[1-4]");
    _analogReadInputReadArgs.unit = arg_int1(NULL, NULL, "<UNIT>", "0 = Raw, 1 = mV, 2 = mA, 3 = V, 4 = A");
    _analogReadInputReadArgs.end = arg_end(2);

    const esp_console_cmd_t read_cmd = {
        .command = "analog-read",
        .help = "Read Ain voltage or current in desired unit",
        .hint = NULL,
        .func = &_analogInputRead,
        .argtable = &_analogReadInputReadArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&read_cmd));
}


/******* Analog Output CLI Commands ************/

static struct {
    struct arg_int *aout;
    struct arg_int *mode;
    struct arg_end *end;
} _setAnalogOutputModeArgs;

int _setAnalogOutputMode(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &_setAnalogOutputModeArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, _setAnalogOutputModeArgs.end, argv[0]);
        return 1;
    }

    AnalogOutput_Num_t ain = (AnalogOutput_Num_t)(_setAnalogOutputModeArgs.aout->ival[0] - 1);
    AnalogOutput_Mode_t mode = (AnalogOutput_Mode_t)(_setAnalogOutputModeArgs.mode->ival[0]);

    MixedStandalone::analogOutputMode(ain, mode);

    return 0;
}

void MixedCLI::_registerSetAnalogOutputMode(void)
{
    _setAnalogOutputModeArgs.aout = arg_int1(NULL, NULL, "<AOUT>", "[1-2]");
    _setAnalogOutputModeArgs.mode = arg_int1(NULL, NULL, "<MODE>", "0 = Voltage, 1 = Current");
    _setAnalogOutputModeArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "set-analog-output-mode",
        .help = "Set Aout mode between Voltage (-10V to 10V) and Current (0mA to 20mA)",
        .hint = NULL,
        .func = &_setAnalogOutputMode,
        .argtable = &_setAnalogOutputModeArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

static struct {
    struct arg_int *aout;
    struct arg_dbl *value;
    struct arg_end *end;
} _analogOutputWriteArgs;

int _analogOutputWrite(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &_analogOutputWriteArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, _analogOutputWriteArgs.end, argv[0]);
        return 1;
    }

    AnalogOutput_Num_t ain = (AnalogOutput_Num_t)(_analogOutputWriteArgs.aout->ival[0] - 1);
    float value = _analogOutputWriteArgs.value->dval[0];

    MixedStandalone::analogWrite(ain, value);

    return 0;
}

void MixedCLI::_registerAnalogOutputWrite(void)
{
    _analogOutputWriteArgs.aout = arg_int1(NULL, NULL, "<AOUT>", "[1-2]");
    _analogOutputWriteArgs.value = arg_dbl1("v", "value", "<VALUE>", "In V for voltage or mA for current");
    _analogOutputWriteArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "analog-write",
        .help = "Set Aout to specified voltage (-10V to 10V) or current (0mA to 20mA)",
        .hint = NULL,
        .func = &_analogOutputWrite,
        .argtable = &_analogOutputWriteArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

#endif