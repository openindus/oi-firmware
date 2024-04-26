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

void CoreCLI::init(void)
{
    _registerDigitalWrite();
    _registerDigitalRead();
    _registerAnalogRead();
    _registerAnalogReadMilliVolt();
    _registerDigitalGetOverCurrentStatus();
    _registerDate();
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
    struct arg_end *end;
} analogReadArgs;

static int analogRead(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &analogReadArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, analogReadArgs.end, argv[0]);
        return 1;
    }

    AnalogInput_Num_t ain = (AnalogInput_Num_t)(analogReadArgs.ain->ival[0] - 1);

    printf("%d\n", CoreStandalone::analogRead(ain));

    return 0;
}

void CoreCLI::_registerAnalogRead(void)
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

static int analogReadMilliVolt(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &analogReadArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, analogReadArgs.end, argv[0]);
        return 1;
    }

    AnalogInput_Num_t ain = (AnalogInput_Num_t)(analogReadArgs.ain->ival[0] - 1);

    printf("%imV\n", CoreStandalone::analogReadMilliVolt(ain));

    return 0;
}

void CoreCLI::_registerAnalogReadMilliVolt(void)
{
    analogReadArgs.ain = arg_int1(NULL, NULL, "<AIN>", "[1-2]");
    analogReadArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "analog-read-millivolts",
        .help = "Get analog input value (MilliVolts)",
        .hint = NULL,
        .func = &analogReadMilliVolt,
        .argtable = &analogReadArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
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
        .command = "get-current-level",
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
