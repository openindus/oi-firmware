/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file AnalogInputsHVCLI.h
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "AnalogInputsHVCLI.h"

int AnalogInputsHVCLI::init() {
    int err = 0;

    err |= _registerAnalogRead();
    err |= _registerSetAnalogCoeffs();
    err |= _registerGetAnalogCoeffs();

    return err;
}

static struct {
    struct arg_int *ain;
    struct arg_int *unit;
    struct arg_end *end;
} _analogReadArgs;

static int _analogRead(int argc, char **argv)
{
    int err = arg_parse(argc, argv, (void **) &_analogReadArgs);
    if (err != 0) {
        arg_print_errors(stderr, _analogReadArgs.end, argv[0]);
        return -1;
    }

    AnalogInput_Num_t ain = (AnalogInput_Num_t)(_analogReadArgs.ain->ival[0] - 1);
    AnalogInput_Unit_t unit = AIN_UNIT_MILLIVOLT;
    if (_analogReadArgs.unit->count == 1) {
        unit = (AnalogInput_Unit_t)(_analogReadArgs.unit->ival[0]);
    }

    switch (unit)
    {
    case AIN_UNIT_RAW:
        printf("%i\n", AnalogInputsHV::analogRead(ain));
        break;
    case AIN_UNIT_VOLT:
        printf("%.3f\n", AnalogInputsHV::analogReadVolt(ain));
        break;
    case AIN_UNIT_MILLIVOLT:
    default:
        printf("%.1f\n", AnalogInputsHV::analogReadMilliVolt(ain));
        break;
    }

    return 0;
}

int AnalogInputsHVCLI::_registerAnalogRead()
{
    _analogReadArgs.ain = arg_int1(NULL, NULL, "<AIN>", "[1-4]");
    _analogReadArgs.unit = arg_int0(NULL, NULL, "<UNIT>", "0 = Raw, 1 = mV, 3 = V");
    _analogReadArgs.end = arg_end(2);

    const esp_console_cmd_t read_cmd = {
        .command = "analog-read",
        .help = "Read AIN voltage or current in desired unit (default to mV)",
        .hint = NULL,
        .func = &_analogRead,
        .argtable = &_analogReadArgs
    };
    return esp_console_cmd_register(&read_cmd);
}


/** 'set-analog-coeffs' */

static struct {
    struct arg_dbl *a1;
    struct arg_dbl *a2;
    struct arg_dbl *b1;
    struct arg_dbl *b2;
    struct arg_end *end;
} _setAnalogCoeffsArgs;

static int _setAnalogCoeffs(int argc, char **argv)
{
    int err = arg_parse(argc, argv, (void **) &_setAnalogCoeffsArgs);
    if (err != 0) {
        arg_print_errors(stderr, _setAnalogCoeffsArgs.end, argv[0]);
        return -1;
    }

    float a[2] = {(float)_setAnalogCoeffsArgs.a1->dval[0], (float)_setAnalogCoeffsArgs.a2->dval[0]};
    float b[2] = {(float)_setAnalogCoeffsArgs.b1->dval[0], (float)_setAnalogCoeffsArgs.b2->dval[0]};

    return AnalogInputsHV::setAnalogCoeffs(a, b);
}

int AnalogInputsHVCLI::_registerSetAnalogCoeffs()
{
    _setAnalogCoeffsArgs.a1 = arg_dbl1(NULL, "a1", "<A1>", "[-100;100]");
    _setAnalogCoeffsArgs.a2 = arg_dbl1(NULL, "a2", "<A2>", "[-100;100]");    
    _setAnalogCoeffsArgs.b1 = arg_dbl1(NULL, "b1", "<B1>", "[-1000;1000]");
    _setAnalogCoeffsArgs.b2 = arg_dbl1(NULL, "b2", "<B2>", "[-1000;1000]");
    _setAnalogCoeffsArgs.end = arg_end(4);

    const esp_console_cmd_t read_cmd = {
        .command = "set-analog-coeffs",
        .help = "Set coefficients of each AIN",
        .hint = NULL,
        .func = &_setAnalogCoeffs,
        .argtable = &_setAnalogCoeffsArgs
    };
    return esp_console_cmd_register(&read_cmd);
}

/** 'get-analog-coeffs' */

static struct {
    struct arg_end *end;
} _getAnalogCoeffsArgs;

static int _getAnalogCoeffs(int argc, char **argv)
{
    int err = arg_parse(argc, argv, (void **) &_getAnalogCoeffsArgs);
    if (err != 0) {
        arg_print_errors(stderr, _getAnalogCoeffsArgs.end, argv[0]);
        return -1;
    }

    float as[2], bs[2];
    AnalogInputsHV::getAnalogCoeffs(as, bs);

    printf("a1 %.3f\n", as[1]);
    printf("a2 %.3f\n", as[2]);
    printf("b1 %.3f\n", bs[1]);
    printf("b2 %.3f\n", bs[2]);

    return 0;
}

int AnalogInputsHVCLI::_registerGetAnalogCoeffs()
{
    _setAnalogCoeffsArgs.end = arg_end(1);

    const esp_console_cmd_t read_cmd = {
        .command = "get-analog-coeffs",
        .help = "Get coefficients of each AIN",
        .hint = NULL,
        .func = &_getAnalogCoeffs,
        .argtable = &_getAnalogCoeffsArgs
    };
    return esp_console_cmd_register(&read_cmd);
}