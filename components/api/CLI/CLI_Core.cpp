/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file CLI_Core::h
 * @brief OpenIndus console
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "CLI_Core.h"

#if defined(OI_CORE)

int CLI_Core::init(void)
{
    int err = 0;

    err |= CLI_AInHV::init();
    err |= CLI_DOut::init();
    err |= CLI_DIn::init();
    err |= _registerDate();

    return err;

    return 0;
}

/** 'date' */

static struct {
    struct arg_date *date;
    struct arg_end *end;
} dateArgs;

static int _date(int argc, char **argv) 
{
    int nerrors = arg_parse(argc, argv, (void **) &dateArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, dateArgs.end, argv[0]);
        return 1;
    }

    Core::rtc.begin();
    if (dateArgs.date->count == 1) {
        DateTime* setDate =  new DateTime(dateArgs.date->tmval->tm_year + 1900, \
                                          dateArgs.date->tmval->tm_mon + 1,     \
                                          dateArgs.date->tmval->tm_mday,        \
                                          dateArgs.date->tmval->tm_hour,        \
                                          dateArgs.date->tmval->tm_min,         \
                                          dateArgs.date->tmval->tm_sec);
        printf("Setting RTC date to %04u-%02u-%02u %02u:%02u:%02u\n", setDate->year(), setDate->month(), setDate->day(), setDate->hour(), setDate->minute(), setDate->second());
        Core::rtc.setTime(*setDate);
    } else {
        // Get date
        DateTime currTime;
        currTime = Core::rtc.now();
        printf("%04u-%02u-%02u %02u:%02u:%02u\n", currTime.year(), currTime.month(), currTime.day(), currTime.hour(), currTime.minute(), currTime.second());
    }

    return 0;
}

int CLI_Core::_registerDate(void)
{
    dateArgs.date = arg_date0("s", "set", "%Y-%m-%d_%H:%M:%S", NULL, NULL);
    dateArgs.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "date",
        .help = "Get/Set RTC Date",
        .hint = NULL,
        .func = &_date,
        .argtable = &dateArgs
    };
    return esp_console_cmd_register(&cmd);
}

#endif
