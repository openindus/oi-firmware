/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file CoreCLI::h
 * @brief OpenIndus console
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "CoreCLI.h"

#if defined(OI_CORE)

int CoreCLI::init(void)
{
    _registerDigitalWrite();
    _registerDigitalRead();
    _registerDigitalGetOverCurrentStatus();
    _registerAnalogInputRead();
    _registerSetAnalogCoeffs();
    _registerDate();

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

    CoreStandalone::digitalWrite(dout, level);

    return 0;
}

void CoreCLI::_registerDigitalWrite(void)
{
    digitalWriteArgs.dout = arg_int1(NULL, NULL, "<DOUT>", "[1-4]");
    digitalWriteArgs.level = arg_int1(NULL, NULL, "<LEVEL>", "0 = LOW, 1 = HIGH");
    digitalWriteArgs.end = arg_end(3);

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

    printf("%d\n", CoreStandalone::digitalRead(din));

    return 0;
}

void CoreCLI::_registerDigitalRead(void)
{
    digitalReadArgs.din = arg_int1(NULL, NULL, "<DIN>", "[1-4]");
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

/** 'analog-read' */

static struct {
    struct arg_int *ain;
    struct arg_int *unit;
    struct arg_end *end;
} _analogReadInputReadArgs;

/** 'analog-read-millivolts' */

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
        printf("%i\n", CoreStandalone::analogRead(ain));
        break;
    case AIN_UNIT_VOLT:
        printf("%.3f\n", CoreStandalone::analogReadVolt(ain));
        break;
    case AIN_UNIT_MILLIVOLT:
    default:
        printf("%.1f\n", CoreStandalone::analogReadMilliVolt(ain));
        break;
    }

    return 0;
}

void CoreCLI::_registerAnalogInputRead(void)
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

    return CoreStandalone::setAnalogCoeffs(a, b);
}

void CoreCLI::_registerSetAnalogCoeffs(void)
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

/** 'digital-read-overcurrent' */

static struct {
    struct arg_int *dout;
    struct arg_end *end;
} digitalReadOverCurrentArgs;

static int digitalGetOverCurrentStatus(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &digitalReadOverCurrentArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, digitalReadOverCurrentArgs.end, argv[0]);
        return 1;
    }

    DigitalOutputNum_t dout = (DigitalOutputNum_t)(digitalReadOverCurrentArgs.dout->ival[0] - 1);
    printf("%u\n", CoreStandalone::digitalGetOverCurrentStatus(dout));
    return 0;
}

void CoreCLI::_registerDigitalGetOverCurrentStatus(void)
{
    digitalReadOverCurrentArgs.dout = arg_int1(NULL, NULL, "<DOUT>", "[1-4]");
    digitalReadOverCurrentArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "get-current-status",
        .help = "Read the overcurrent status for the given output",
        .hint = NULL,
        .func = &digitalGetOverCurrentStatus,
        .argtable = &digitalReadOverCurrentArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/** 'date' */

static struct {
    struct arg_date *date;
    struct arg_end *end;
} dateArgs;

static int date(int argc, char **argv) 
{
    int nerrors = arg_parse(argc, argv, (void **) &dateArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, dateArgs.end, argv[0]);
        return 1;
    }

    if (dateArgs.date->count == 1) {
        DateTime* setDate =  new DateTime(dateArgs.date->tmval->tm_year + 1900, \
                                          dateArgs.date->tmval->tm_mon + 1,     \
                                          dateArgs.date->tmval->tm_mday,        \
                                          dateArgs.date->tmval->tm_hour,        \
                                          dateArgs.date->tmval->tm_min,         \
                                          dateArgs.date->tmval->tm_sec);
        printf("Setting RTC date to %04u-%02u-%02u %02u:%02u:%02u\n", setDate->year(), setDate->month(), setDate->day(), setDate->hour(), setDate->minute(), setDate->second());
        RTC.setTime(*setDate);
    }
    else {
        // Get date
        DateTime currTime;
        currTime = RTC.now();
        printf("%04u-%02u-%02u %02u:%02u:%02u\n", currTime.year(), currTime.month(), currTime.day(), currTime.hour(), currTime.minute(), currTime.second());
    }

    return 0;
}

void CoreCLI::_registerDate(void)
{
    dateArgs.date = arg_date0("s", "set", "%Y-%m-%d_%H:%M:%S", NULL, NULL);
    dateArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "date",
        .help = "Get/Set RTC Date",
        .hint = NULL,
        .func = &date,
        .argtable = &dateArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

#endif
