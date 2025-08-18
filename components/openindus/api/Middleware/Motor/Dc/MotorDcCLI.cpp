/**
 * @file MotorDcCLI.cpp
 * @brief Command line interface - Motor DC
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "Motor.h"
#include "MotorDc.h"
#include "esp_log.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"

/** 'run' */

static struct {
    struct arg_int *motor;
    struct arg_int *dir;
    struct arg_end *end;
} runArgs;

static int run(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &runArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, runArgs.end, argv[0]);
        return 1;
    }

    MotorNum_t motor = (MotorNum_t)(runArgs.motor->ival[0] - 1);
    MotorDirection_t direction = (MotorDirection_t)(runArgs.dir->ival[0]);

    MotorDc::run(motor, direction, 100);

    return 0;
}

static void _registerRun(void)
{
    runArgs.motor   = arg_int1(NULL, NULL, "MOTOR", "[1-4]");
    runArgs.dir     = arg_int1(NULL, NULL, "DIRECTION", "[1: Forward, 0: Reverse]");
    runArgs.end     = arg_end(4);

    const esp_console_cmd_t cmd = {
        .command = "run",
        .help = "run",
        .hint = NULL,
        .func = &run,
        .argtable = &runArgs,
        .func_w_context = NULL,
        .context = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/** 'stop' */
static struct {
    struct arg_int *motor;
    struct arg_end *end;
} stopArgs;

static int stop(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &stopArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, stopArgs.end, argv[0]);
        return 1;
    }

    MotorNum_t motor = (MotorNum_t)(stopArgs.motor->ival[0] - 1);

    MotorDc::stop(motor);

    return 0;
}

static void _registerStop(void)
{
    stopArgs.motor  = arg_int1(NULL, NULL, "MOTOR", "[1-4]");
    stopArgs.end    = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "stop",
        .help = "stop the motor",
        .hint = NULL,
        .func = &stop,
        .argtable = &stopArgs,
        .func_w_context = NULL,
        .context = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

int MotorDc::_registerCLI(void)
{
    _registerStop();
    _registerRun();

    return 0;
}
