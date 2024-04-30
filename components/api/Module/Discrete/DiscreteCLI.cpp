/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DiscreteCLI::h
 * @brief OpenIndus console
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "DiscreteCLI.h"

#if defined(OI_DISCRETE) || defined(OI_DISCRETE_VE)

int DiscreteCLI::init(void)
{
    _registerDigitalWrite();
    _registerDigitalRead();
    _registerDigitalGetCurrent();
    _registerAnalogInputRead();

    return 0;
}

/** 'digital-write' */

static struct {
    struct arg_int *dout;
    struct arg_int *level;
    struct arg_end *end;
} digitalWriteArgs;

static int dWrite(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &digitalWriteArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, digitalWriteArgs.end, argv[0]);
        return 1;
    }

    DigitalOutputNum_t dout = (DigitalOutputNum_t)(digitalWriteArgs.dout->ival[0] - 1);
    uint8_t level = (uint8_t)digitalWriteArgs.level->ival[0];

    DiscreteStandalone::digitalWrite(dout, level);

    return 0;
}

void DiscreteCLI::_registerDigitalWrite(void)
{
    digitalWriteArgs.dout = arg_int1(NULL, NULL, "<DOUT>", "[1-8]");
    digitalWriteArgs.level = arg_int1(NULL, NULL, "<LEVEL>", "0 = LOW, 1 = HIGH");
    digitalWriteArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "digital-write",
        .help = "Set DOUT level",
        .hint = NULL,
        .func = &dWrite,
        .argtable = &digitalWriteArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/** 'digital-read' */

static struct {
    struct arg_int *din;
    struct arg_end *end;
} digitalReadArgs;

static int dRead(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &digitalReadArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, digitalReadArgs.end, argv[0]);
        return 1;
    }

    DigitalInputNum_t din = (DigitalInputNum_t)(digitalReadArgs.din->ival[0] - 1);

    printf("%d\n", DiscreteStandalone::digitalRead(din));

    return 0;
}

void DiscreteCLI::_registerDigitalRead(void)
{
    digitalReadArgs.din = arg_int1(NULL, NULL, "<DIN>", "[1-10]");
    digitalReadArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "digital-read",
        .help = "Get DIN level",
        .hint = NULL,
        .func = &dRead,
        .argtable = &digitalReadArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/** 'get-current' */

static struct {
    struct arg_int *dout;
    struct arg_end *end;
} digitalGetCurrentArgs;

static int digitalGetCurrent(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &digitalGetCurrentArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, digitalGetCurrentArgs.end, argv[0]);
        return 1;
    }

    DigitalOutputNum_t dout = (DigitalOutputNum_t)(digitalGetCurrentArgs.dout->ival[0] - 1);

    printf("%.3f\n", DiscreteStandalone::digitalGetCurrent(dout));

    return 0;
}

void DiscreteCLI::_registerDigitalGetCurrent(void)
{
    digitalGetCurrentArgs.dout = arg_int1(NULL, NULL, "<DOUT>", "[1-8]");
    digitalGetCurrentArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "get-current",
        .help = "Get Dout current",
        .hint = NULL,
        .func = &digitalGetCurrent,
        .argtable = &digitalGetCurrentArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/** 'analog-read' */

static struct {
    struct arg_int *ain;
    struct arg_int *unit;
    struct arg_end *end;
} _analogReadInputReadArgs;

int analogInputRead(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &_analogReadInputReadArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, _analogReadInputReadArgs.end, argv[0]);
        return 1;
    }

    AnalogInput_Num_t ain = (AnalogInput_Num_t)(_analogReadInputReadArgs.ain->ival[0] - 1);
    AnalogInput_Unit_t unit = AIN_UNIT_MILLIVOLT;
    if (_analogReadInputReadArgs.unit->count == 1) {
        unit = (AnalogInput_Unit_t)(_analogReadInputReadArgs.unit->ival[0]);
    }

    switch (unit)
    {
    case AIN_UNIT_RAW:
        printf("%i\n", DiscreteStandalone::analogRead(ain));
        break;
    case AIN_UNIT_VOLT:
        printf("%.3f\n", DiscreteStandalone::analogReadVolt(ain));
        break;
    case AIN_UNIT_MILLIVOLT:
    default:
        printf("%.1f\n", DiscreteStandalone::analogReadMilliVolt(ain));
        break;
    }

    return 0;
}

void DiscreteCLI::_registerAnalogInputRead(void)
{
    _analogReadInputReadArgs.ain = arg_int1(NULL, NULL, "<AIN>", "[1-4]");
    _analogReadInputReadArgs.unit = arg_int0(NULL, NULL, "<UNIT>", "0 = Raw, 1 = mV, 3 = V");
    _analogReadInputReadArgs.end = arg_end(2);

    const esp_console_cmd_t read_cmd = {
        .command = "analog-read",
        .help = "Read Ain voltage or current in desired unit (default to mV)",
        .hint = NULL,
        .func = &analogInputRead,
        .argtable = &_analogReadInputReadArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&read_cmd));
}


/** 'set-analog-coeffs' */

static struct {
    struct arg_dbl *a1;
    struct arg_dbl *a2;
    struct arg_dbl *b1;
    struct arg_dbl *b2;
    struct arg_end *end;
} _setAnalogCoeffsArgs;

int setAnalogCoeffs(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &_setAnalogCoeffsArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, _setAnalogCoeffsArgs.end, argv[0]);
        return 1;
    }

    float a[2] = {(float)_setAnalogCoeffsArgs.a1->dval[0], (float)_setAnalogCoeffsArgs.a2->dval[0]};
    float b[2] = {(float)_setAnalogCoeffsArgs.b1->dval[0], (float)_setAnalogCoeffsArgs.b2->dval[0]};

    return DiscreteStandalone::setAnalogCoeffs(a, b);
}

void DiscreteCLI::_registerSetAnalogCoeffs(void)
{
    _setAnalogCoeffsArgs.a1 = arg_dbl1(NULL, "a1", "<A1>", "[-100;100]");
    _setAnalogCoeffsArgs.a2 = arg_dbl1(NULL, "a2", "<A2>", "[-100;100]");    
    _setAnalogCoeffsArgs.b1 = arg_dbl1(NULL, "b1", "<B1>", "[-1000;1000]");
    _setAnalogCoeffsArgs.b2 = arg_dbl1(NULL, "b2", "<B2>", "[-1000;1000]");
    _setAnalogCoeffsArgs.end = arg_end(4);

    const esp_console_cmd_t read_cmd = {
        .command = "set-analog-coeffs",
        .help = "Set Ain coefficients",
        .hint = NULL,
        .func = &setAnalogCoeffs,
        .argtable = &_setAnalogCoeffsArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&read_cmd));
}

#endif
