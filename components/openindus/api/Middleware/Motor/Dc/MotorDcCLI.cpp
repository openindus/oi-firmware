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

    if (runArgs.motor->ival[0] < 1 || runArgs.motor->ival[0] > 4) {
        fprintf(stderr, "Error: MOTOR must be in range [1-4].\n");
        return 1;
    }
    if (runArgs.dir->ival[0] < 0 || runArgs.dir->ival[0] > 1) {
        fprintf(stderr, "Error: DIRECTION must be 0 (Reverse) or 1 (Forward).\n");
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

    if (stopArgs.motor->ival[0] < 1 || stopArgs.motor->ival[0] > 4) {
        fprintf(stderr, "Error: MOTOR must be in range [1-4].\n");
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

/** 'getcurrent' */
static struct {
    struct arg_int *motor;
    struct arg_end *end;
} getCurrentArgs;

static int getCurrent(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &getCurrentArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, getCurrentArgs.end, argv[0]);
        return 1;
    }

    if (getCurrentArgs.motor->ival[0] < 1 || getCurrentArgs.motor->ival[0] > 4) {
        fprintf(stderr, "Error: MOTOR must be in range [1-4].\n");
        return 1;
    }

    MotorNum_t motor = (MotorNum_t)(getCurrentArgs.motor->ival[0] - 1);

    float current = MotorDc::getCurrent(motor);

    printf("Motor %d current: %.3f A\n", getCurrentArgs.motor->ival[0], current);

    return 0;
}

static void _registerGetCurrent(void)
{
    getCurrentArgs.motor  = arg_int1(NULL, NULL, "MOTOR", "[1-4]");
    getCurrentArgs.end    = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "getcurrent",
        .help = "get motor current",
        .hint = NULL,
        .func = &getCurrent,
        .argtable = &getCurrentArgs,
        .func_w_context = NULL,
        .context = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/** 'getfault' */
static struct {
    struct arg_int *motor;
    struct arg_end *end;
} getFaultArgs;

static int getFault(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &getFaultArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, getFaultArgs.end, argv[0]);
        return 1;
    }

    if (getFaultArgs.motor->ival[0] < 1 || getFaultArgs.motor->ival[0] > 4) {
        fprintf(stderr, "Error: MOTOR must be in range [1-4].\n");
        return 1;
    }

    MotorNum_t motor = (MotorNum_t)(getFaultArgs.motor->ival[0] - 1);
    uint16_t fault_status = MotorDc::getFault(motor);

    printf("Motor %d fault status: 0x%04X\n", getFaultArgs.motor->ival[0], fault_status);

    // Provide human-readable interpretation
    if (fault_status & DRV8873_FAULT_UVLO_MASK) {
        printf("  - Undervoltage Lockout (UVLO)\n");
    }
    if (fault_status & DRV8873_FAULT_CPUV_MASK) {
        printf("  - Charge Pump Undervoltage (CPUV)\n");
    }
    if (fault_status & DRV8873_FAULT_OCP_MASK) {
        printf("  - Overcurrent Protection (OCP)\n");
    }
    if (fault_status & DRV8873_FAULT_TSD_MASK) {
        printf("  - Thermal Shutdown (TSD)\n");
    }
    if (fault_status & DRV8873_FAULT_OLD_MASK) {
        printf("  - Open-Load Detection (OLD)\n");
    }
    if (fault_status & DRV8873_FAULT_OTW_MASK) {
        printf("  - Overtemperature Warning (OTW)\n");
    }
    if (fault_status & DRV8873_FAULT_FAULT_MASK) {
        printf("  - Global Fault\n");
    }
    if (fault_status == 0) {
        printf("  - No faults detected\n");
    }

    return 0;
}

static void _registerGetFault(void)
{
    getFaultArgs.motor  = arg_int1(NULL, NULL, "MOTOR", "[1-4]");
    getFaultArgs.end    = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "getfault",
        .help = "get motor fault status",
        .hint = NULL,
        .func = &getFault,
        .argtable = &getFaultArgs,
        .func_w_context = NULL,
        .context = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/** 'clearfault' */
static struct {
    struct arg_int *motor;
    struct arg_end *end;
} clearFaultArgs;

static int clearFault(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &clearFaultArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, clearFaultArgs.end, argv[0]);
        return 1;
    }

    if (clearFaultArgs.motor->ival[0] < 1 || clearFaultArgs.motor->ival[0] > 4) {
        fprintf(stderr, "Error: MOTOR must be in range [1-4].\n");
        return 1;
    }

    MotorNum_t motor = (MotorNum_t)(clearFaultArgs.motor->ival[0] - 1);

    esp_err_t result = MotorDc::clear_fault(motor);

    if (result == ESP_OK) {
        printf("Motor %d faults cleared successfully\n", clearFaultArgs.motor->ival[0]);
    } else {
        printf("Failed to clear faults for motor %d (error: %d)\n", clearFaultArgs.motor->ival[0], result);
    }

    return 0;
}

static void _registerClearFault(void)
{
    clearFaultArgs.motor  = arg_int1(NULL, NULL, "MOTOR", "[1-4]");
    clearFaultArgs.end    = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "clearfault",
        .help = "clear motor faults",
        .hint = NULL,
        .func = &clearFault,
        .argtable = &clearFaultArgs,
        .func_w_context = NULL,
        .context = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/** 'setmode' */
static struct {
    struct arg_int *motor;
    struct arg_int *mode;
    struct arg_end *end;
} setModeArgs;

static int setMode(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &setModeArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, setModeArgs.end, argv[0]);
        return 1;
    }

    if (setModeArgs.motor->ival[0] < 1 || setModeArgs.motor->ival[0] > 4) {
        fprintf(stderr, "Error: MOTOR must be in range [1-4].\n");
        return 1;
    }

    if (setModeArgs.mode->ival[0] < 0 || setModeArgs.mode->ival[0] > 3) {
        fprintf(stderr, "Error: MODE must be 0 (PH/EN), 1 (PWM), 2 (Independent), or 3 (Disabled).\n");
        return 1;
    }

    MotorNum_t motor = (MotorNum_t)(setModeArgs.motor->ival[0] - 1);
    drv8873_mode_t mode = (drv8873_mode_t)(setModeArgs.mode->ival[0]);

    esp_err_t result = MotorDc::setMode(mode, motor);

    if (result == ESP_OK) {
        const char* mode_str[] = {"PH/EN", "PWM", "Independent", "Disabled"};
        printf("Motor %d mode set to: %s\n", setModeArgs.motor->ival[0], mode_str[mode]);
    } else {
        printf("Failed to set mode for motor %d (error: %d)\n", setModeArgs.motor->ival[0], result);
    }

    return 0;
}

static void _registerSetMode(void)
{
    setModeArgs.motor  = arg_int1(NULL, NULL, "MOTOR", "[1-4]");
    setModeArgs.mode   = arg_int1(NULL, NULL, "MODE", "[0: PH/EN, 1: PWM, 2: Independent, 3: Disabled]");
    setModeArgs.end    = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "setmode",
        .help = "set DRV8873 control mode",
        .hint = NULL,
        .func = &setMode,
        .argtable = &setModeArgs,
        .func_w_context = NULL,
        .context = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

int MotorDc::_registerCLI(void)
{
    _registerRun();
    _registerStop();
    _registerGetCurrent();
    _registerGetFault();
    _registerClearFault();
    _registerSetMode();

    return 0;
}
