/**
 * @file MotorStepperCLI.cpp
 * @brief Command line interface - Motor Stepper
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "Motor.h"
#include "MotorStepper.h"
#include "DigitalInputs.h"
#include "PS01_Cmd.h"
#include "esp_log.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"

/** 'attach-limit-switch' */

static struct {
    struct arg_int *motor;
    struct arg_int *din;
    struct arg_int *logic;
    struct arg_end *end;
} attachLimitSwitchArgs;

static int attachLimitSwitch(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &attachLimitSwitchArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, attachLimitSwitchArgs.end, argv[0]);
        return 1;
    }

    MotorNum_t motor = (MotorNum_t)(attachLimitSwitchArgs.motor->ival[0] - 1);
    DIn_Num_t din = (DIn_Num_t)(attachLimitSwitchArgs.din->ival[0] - 1);

    if (attachLimitSwitchArgs.logic->count > 0) {
        Logic_t logic = (Logic_t)(attachLimitSwitchArgs.logic->ival[0]);
        MotorStepper::attachLimitSwitch(motor, din, logic);
    } else {
        MotorStepper::attachLimitSwitch(motor, din);
    }

    return 0;
}

static void _registerAttachLimitSwitch(void)
{
    attachLimitSwitchArgs.motor   = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    attachLimitSwitchArgs.din    = arg_int1(NULL, NULL, "DIN", "[1-4]");
    attachLimitSwitchArgs.logic   = arg_int0(NULL, NULL, "logic", "[0: Active low, 1: Active high]");
    attachLimitSwitchArgs.end     = arg_end(4);

    const esp_console_cmd_t cmd = {
        .command = "stepper-attach-limit-switch",
        .help = "set limit switch",
        .hint = NULL,
        .func = &attachLimitSwitch,
        .argtable = &attachLimitSwitchArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/** 'detach-limit-switch' */

static struct {
    struct arg_int *motor;
    struct arg_int *din;
    struct arg_end *end;
} detachLimitSwitchArgs;

static int detachLimitSwitch(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &detachLimitSwitchArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, detachLimitSwitchArgs.end, argv[0]);
        return 1;
    }

    MotorNum_t motor = (MotorNum_t)(detachLimitSwitchArgs.motor->ival[0] - 1);
    DIn_Num_t din = (DIn_Num_t)(detachLimitSwitchArgs.din->ival[0] - 1);

    MotorStepper::detachLimitSwitch(motor, din);

    return 0;
}

static void _registerDetachLimitSwitch(void)
{
    detachLimitSwitchArgs.motor   = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    detachLimitSwitchArgs.din     = arg_int1(NULL, NULL, "DIN", "[1-4]");
    detachLimitSwitchArgs.end     = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "stepper-detach-limit-switch",
        .help = "Detach a limit switch from the specified motor",
        .hint = NULL,
        .func = &detachLimitSwitch,
        .argtable = &detachLimitSwitchArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/** 'step-resolution' */

static struct {
    struct arg_int *motor;
    struct arg_int *res;
    struct arg_end *end;
} stepResolutionArgs;

static int stepResolution(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &stepResolutionArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, stepResolutionArgs.end, argv[0]);
        return 1;
    }

    MotorNum_t motor = (MotorNum_t)(stepResolutionArgs.motor->ival[0] - 1);
    MotorStepResolution_t res = (MotorStepResolution_t)(stepResolutionArgs.res->ival[0]);

    MotorStepper::setStepResolution(motor, res);

    return 0;
}

static void _registerStepResolution(void)
{
    stepResolutionArgs.motor    = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    stepResolutionArgs.res      = arg_int1(NULL, NULL, "RESOLUTION", "[0: Full step, 1: Half step, 2: 1/4 step, 3: 1/8 step, 4: 1/16 step, 5: 1/32 step, 6: 1/64 step, 7: 1/128 step]");
    stepResolutionArgs.end      = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "stepper-step-resolution",
        .help = "set step resolution",
        .hint = NULL,
        .func = &stepResolution,
        .argtable = &stepResolutionArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/** 'set-speed' */

static struct {
    struct arg_int *motor;
    struct arg_int *speed;
    struct arg_end *end;
} setMaxSpeedArgs;

static int setMaxSpeed(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &setMaxSpeedArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, setMaxSpeedArgs.end, argv[0]);
        return 1;
    }

    MotorNum_t motor = (MotorNum_t)(setMaxSpeedArgs.motor->ival[0] - 1);
    float speed = (float)(setMaxSpeedArgs.speed->ival[0]);

    MotorStepper::setMaxSpeed(motor, speed);

    return 0;
}

static void _registerSetMaxSpeed(void)
{
    setMaxSpeedArgs.motor  = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    setMaxSpeedArgs.speed  = arg_int1(NULL, NULL, "SPEED", "motor max speed in step/s");
    setMaxSpeedArgs.end    = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "stepper-set-speed",
        .help = "set motor speed",
        .hint = NULL,
        .func = &setMaxSpeed,
        .argtable = &setMaxSpeedArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/** 'set-acceleration' */

static struct {
    struct arg_int *motor;
    struct arg_int *acceleration;
    struct arg_end *end;
} setAccelerationArgs;
static int setAcceleration(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &setAccelerationArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, setAccelerationArgs.end, argv[0]);
        return 1;
    }
    MotorNum_t motor = (MotorNum_t)(setAccelerationArgs.motor->ival[0] - 1);
    float acceleration = (float)(setAccelerationArgs.acceleration->ival[0]);
    MotorStepper::setAcceleration(motor, acceleration);
    return 0;
}
static void _registerSetAcceleration(void)
{
    setAccelerationArgs.motor = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    setAccelerationArgs.acceleration = arg_int1(NULL, NULL, "ACCELERATION", "motor acceleration in step/s²");
    setAccelerationArgs.end = arg_end(2);
    const esp_console_cmd_t cmd = {
        .command = "stepper-set-acceleration",
        .help = "set motor acceleration",
        .hint = NULL,
        .func = &setAcceleration,
        .argtable = &setAccelerationArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}
/** 'set-deceleration' */
static struct {
    struct arg_int *motor;
    struct arg_int *deceleration;
    struct arg_end *end;
} setDecelerationArgs;

static int setDeceleration(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &setDecelerationArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, setDecelerationArgs.end, argv[0]);
        return 1;
    }
    MotorNum_t motor = (MotorNum_t)(setDecelerationArgs.motor->ival[0] - 1);
    float deceleration = (float)(setDecelerationArgs.deceleration->ival[0]);
    MotorStepper::setDeceleration(motor, deceleration);
    return 0;
}

static void _registerSetDeceleration(void)
{
    setDecelerationArgs.motor = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    setDecelerationArgs.deceleration = arg_int1(NULL, NULL, "DECELERATION", "motor deceleration speed in step/s²");
    setDecelerationArgs.end = arg_end(2);
    const esp_console_cmd_t cmd = {
        .command = "stepper-set-deceleration",
        .help = "set motor deceleration",
        .hint = NULL,
        .func = &setDeceleration,
        .argtable = &setDecelerationArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}
/** 'set-min-speed' */
static struct {
    struct arg_int *motor;
    struct arg_int *speed;
    struct arg_end *end;
} setMinSpeedArgs;

static int setMinSpeed(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &setMinSpeedArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, setMinSpeedArgs.end, argv[0]);
        return 1;
    }
    MotorNum_t motor = (MotorNum_t)(setMinSpeedArgs.motor->ival[0] - 1);
    float speed = (float)(setMinSpeedArgs.speed->ival[0]);
    MotorStepper::setMinSpeed(motor, speed);
    return 0;
}

static void _registerSetMinSpeed(void)
{
    setMinSpeedArgs.motor = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    setMinSpeedArgs.speed = arg_int1(NULL, NULL, "MIN_SPEED", "motor min speed in step/s");
    setMinSpeedArgs.end = arg_end(2);
    const esp_console_cmd_t cmd = {
        .command = "stepper-set-min-speed",
        .help = "set motor minimum speed",
        .hint = NULL,
        .func = &setMinSpeed,
        .argtable = &setMinSpeedArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}
/** 'set-full-step-speed' */
static struct {
    struct arg_int *motor;
    struct arg_int *speed;
    struct arg_end *end;
} setFullStepSpeedArgs;

static int setFullStepSpeed(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &setFullStepSpeedArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, setFullStepSpeedArgs.end, argv[0]);
        return 1;
    }
    MotorNum_t motor = (MotorNum_t)(setFullStepSpeedArgs.motor->ival[0] - 1);
    float speed = (float)(setFullStepSpeedArgs.speed->ival[0]);
    MotorStepper::setFullStepSpeed(motor, speed);
    return 0;
}

static void _registerSetFullStepSpeed(void)
{
    setFullStepSpeedArgs.motor = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    setFullStepSpeedArgs.speed = arg_int1(NULL, NULL, "FULL_STEP_SPEED", "motor full step speed in step/s");
    setFullStepSpeedArgs.end = arg_end(2);
    const esp_console_cmd_t cmd = {
        .command = "stepper-set-full-step-speed",
        .help = "set motor full step speed",
        .hint = NULL,
        .func = &setFullStepSpeed,
        .argtable = &setFullStepSpeedArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/** 'get-position' */

static struct {
    struct arg_int *motor;
    struct arg_end *end;
} getPositionArgs;

static int getPosition(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &getPositionArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, getPositionArgs.end, argv[0]);
        return 1;
    }

    MotorNum_t motor = (MotorNum_t)(getPositionArgs.motor->ival[0] - 1);

    printf("%d\n", MotorStepper::getPosition(motor));

    return 0;
}

static void _registerGetPosition(void)
{
    getPositionArgs.motor  = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    getPositionArgs.end    = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "stepper-get-position",
        .help = "get current motor position",
        .hint = NULL,
        .func = &getPosition,
        .argtable = &getPositionArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/** 'get-speed' */

static struct {
    struct arg_int *motor;
    struct arg_end *end;
} getSpeedArgs;

static int getSpeed(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &getSpeedArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, getSpeedArgs.end, argv[0]);
        return 1;
    }

    MotorNum_t motor = (MotorNum_t)(getSpeedArgs.motor->ival[0] - 1);

    printf("%f\n", MotorStepper::getSpeed(motor));

    return 0;
}

static void _registerGetSpeed(void)
{
    getSpeedArgs.motor  = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    getSpeedArgs.end    = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "stepper-get-speed",
        .help = "get current motor speed",
        .hint = NULL,
        .func = &getSpeed,
        .argtable = &getSpeedArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/** 'reset-home-position' */
static struct {
    struct arg_int *motor;
    struct arg_end *end;
} resetHomePositionArgs;

static int resetHomePosition(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &resetHomePositionArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, resetHomePositionArgs.end, argv[0]);
        return 1;
    }

    MotorNum_t motor = (MotorNum_t)(resetHomePositionArgs.motor->ival[0] - 1);

    MotorStepper::resetHomePosition(motor);

    return 0;
}

static void _registerResetHomePosition(void) 
{
    resetHomePositionArgs.motor = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    resetHomePositionArgs.end   = arg_end(1);

    const esp_console_cmd_t cmd = {
        .command  = "stepper-reset-home-position",
        .help     = "Reset motor home position",
        .hint     = NULL,
        .func     = &resetHomePosition,
        .argtable = &resetHomePositionArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));

}

/** 'set-position' */
static struct {
    struct arg_int *motor;
    struct arg_int *position;
    struct arg_end *end;
} setPositionArgs;

static int setPosition(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &setPositionArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, setPositionArgs.end, argv[0]);
        return 1;
    }

    MotorNum_t motor = (MotorNum_t)(setPositionArgs.motor->ival[0] - 1);
    int32_t position = (int32_t)(setPositionArgs.position->ival[0]);

    MotorStepper::setPosition(motor, position);

    return 0;
}

static void _registerSetPosition(void) 
{
    setPositionArgs.motor = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    setPositionArgs.position = arg_int1(NULL, NULL, "<position>", "Position (int)");
    setPositionArgs.end   = arg_end(2);
    const esp_console_cmd_t setPosCmd = {
        .command  = "stepper-set-position",
        .help     = "Set motor position",
        .hint     = NULL,
        .func     = &setPosition,
        .argtable = &setPositionArgs
    };
    esp_console_cmd_register(&setPosCmd);
}

/** 'stop' */

static struct {
    struct arg_int *motor;
    struct arg_str *mode;
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
    MotorStopMode_t stopMode = SOFT_HIZ; // Default mode

    if (stopArgs.mode->count > 0) {
        const char *mode_str = stopArgs.mode->sval[0];
        if (strcmp(mode_str, "soft-stop") == 0) {
            stopMode = SOFT_STOP;
        } else if (strcmp(mode_str, "hard-stop") == 0) {
            stopMode = HARD_STOP;
        } else if (strcmp(mode_str, "soft-hiz") == 0) {
            stopMode = SOFT_HIZ;
        } else if (strcmp(mode_str, "hard-hiz") == 0) {
            stopMode = HARD_HIZ;
        } else {
            printf("Invalid stop mode: %s. Valid modes are: soft-stop, hard-stop, soft-hiz, hard-hiz\n", mode_str);
            return 1;
        }
    }

    MotorStepper::stop(motor, stopMode);

    return 0;
}

static void _registerStop(void)
{
    stopArgs.motor  = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    stopArgs.mode   = arg_str0(NULL, NULL, "MODE", "[soft-stop, hard-stop, soft-hiz, hard-hiz]");
    stopArgs.end    = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "stepper-stop",
        .help = "stop the motor with specified mode",
        .hint = NULL,
        .func = &stop,
        .argtable = &stopArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/** 'move-absolute' */

static struct {
    struct arg_int *motor;
    struct arg_int *pos;
    struct arg_end *end;
} moveAbsoluteArgs;

static int moveAbsolute(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &moveAbsoluteArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, moveAbsoluteArgs.end, argv[0]);
        return 1;
    }

    MotorNum_t motor = (MotorNum_t)(moveAbsoluteArgs.motor->ival[0] - 1);
    uint32_t position = (uint32_t)(moveAbsoluteArgs.pos->ival[0]);

    MotorStepper::moveAbsolute(motor, position);

    return 0;
}

static void _registerMoveAbsolute(void)
{
    moveAbsoluteArgs.motor  = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    moveAbsoluteArgs.pos    = arg_int1(NULL, NULL, "POSITION", "position in step");
    moveAbsoluteArgs.end    = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "stepper-move-absolute",
        .help = "move absolute",
        .hint = NULL,
        .func = &moveAbsolute,
        .argtable = &moveAbsoluteArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/** 'move-relative' */

static struct {
    struct arg_int *motor;
    struct arg_int *pos;
    struct arg_int *dir;
    struct arg_end *end;
} moveRelativeArgs;

static int moveRelative(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &moveRelativeArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, moveRelativeArgs.end, argv[0]);
        return 1;
    }

    MotorNum_t motor = (MotorNum_t)(moveRelativeArgs.motor->ival[0] - 1);
    int32_t position = (int32_t)(moveRelativeArgs.pos->ival[0]);
    
    if (moveRelativeArgs.dir->count > 0) {
        int direction = moveRelativeArgs.dir->ival[0];
        if (direction == 0) {
            position = -abs(position);
        } else {
            position = abs(position);
        }
    }

    MotorStepper::moveRelative(motor, position);

    return 0;
}

static void _registerMoveRelative(void)
{
    moveRelativeArgs.motor  = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    moveRelativeArgs.pos    = arg_int1(NULL, NULL, "POSITION", "position in step");
    moveRelativeArgs.dir    = arg_int0(NULL, NULL, "DIRECTION", "[1: Forward, 0: Reverse] (optional, position sign)");
    moveRelativeArgs.end    = arg_end(4);

    const esp_console_cmd_t cmd = {
        .command = "stepper-move-relative",
        .help = "move relative",
        .hint = NULL,
        .func = &moveRelative,
        .argtable = &moveRelativeArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/** 'run' */

static struct {
    struct arg_int *motor;
    struct arg_int *dir;
    struct arg_int *speed;
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
    float speed = (float)(runArgs.speed->ival[0]);

    MotorStepper::run(motor, direction, speed);

    return 0;
}

static void _registerRun(void)
{
    runArgs.motor   = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    runArgs.dir     = arg_int1(NULL, NULL, "DIRECTION", "[1: Forward, 0: Reverse]");
    runArgs.speed   = arg_int1(NULL, NULL, "SPEED", "speed in step/s");
    runArgs.end     = arg_end(4);

    const esp_console_cmd_t cmd = {
        .command = "stepper-run",
        .help = "run",
        .hint = NULL,
        .func = &run,
        .argtable = &runArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/** 'homing' */

static struct {
    struct arg_int *motor;
    struct arg_int *speed;
    struct arg_end *end;
} homingArgs;

static int homing(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &homingArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, homingArgs.end, argv[0]);
        return 1;
    }

    MotorNum_t motor = (MotorNum_t)(homingArgs.motor->ival[0] - 1);
    float speed = (float)(homingArgs.speed->ival[0]);

    MotorStepper::homing(motor, speed);

    return 0;
}

static void _registerHoming(void)
{
    homingArgs.motor    = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    homingArgs.speed    = arg_int1(NULL, NULL, "SPEED", "speed in step/s");
    homingArgs.end      = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "stepper-homing",
        .help = "homing",
        .hint = NULL,
        .func = &homing,
        .argtable = &homingArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/** 'getSupplyVoltage' */

static struct {
    struct arg_end *end;
} getSupplyVoltageArgs;

static int getSupplyVoltage(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &getSupplyVoltageArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, getSupplyVoltageArgs.end, argv[0]);
        return 1;
    }

    printf("%f\n", MotorStepper::getSupplyVoltage());

    return 0;
}

static void _registerGetSupplyVoltage(void)
{
    getSupplyVoltageArgs.end = arg_end(0);

    const esp_console_cmd_t cmd = {
        .command = "stepper-get-supply-voltage",
        .help = "get supply voltage",
        .hint = NULL,
        .func = &getSupplyVoltage,
        .argtable = &getSupplyVoltageArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/** 'get-status' */

static struct {
    struct arg_int *motor;
    struct arg_lit *raw;
    struct arg_end *end;
} getStatusArgs;

static int getStatus(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &getStatusArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, getStatusArgs.end, argv[0]);
        return 1;
    }

    MotorNum_t motor = (MotorNum_t)(getStatusArgs.motor->ival[0] - 1);
    MotorStepperStatus_t status = MotorStepper::getStatus(motor);

    if (getStatusArgs.raw->count > 0) {
        uint16_t statusReg = *reinterpret_cast<uint16_t*>(&status);
        printf("0x%04X\n", statusReg);
        return 0;
    }

    printf("Motor %d Status:\n", getStatusArgs.motor->ival[0]);
    printf("  HiZ (High Impedance): %s\n", status.hiz ? "YES" : "NO");
    printf("  Motor busy: %s\n", !status.busy ? "YES" : "NO");
    printf("  Switch input status: %s\n", status.sw_f ? "HIGH" : "LOW");
    printf("  Switch Turn-on Event: %s\n", status.sw_evn ? "YES" : "NO");
    printf("  Direction: %s\n", status.dir ? "FORWARD" : "REVERSE");
    
    const char* mot_status_str[] = {"STOPPED", "ACCELERATION", "DECELERATION", "CONSTANT_SPEED"};
    printf("  Motor Status: %s\n", mot_status_str[status.mot_status & 0x03]);
    
    printf("  Command Error: %s\n", status.cmd_error ? "YES" : "NO");
    printf("  Step-Clock Mode: %s\n", status.stck_mod ? "YES" : "NO");
    printf("  Undervoltage Lockout: %s\n", !status.uvlo ? "YES" : "NO");
    printf("  VS Undervoltage Lockout: %s\n", !status.uvlo_adc ? "YES" : "NO");
    
    const char* th_status_str[] = {"NORMAL", "WARNING", "BRIDGE_SHUTDOWN", "DEVICE_SHUTDOWN"};
    printf("  Thermal Status: %s\n", th_status_str[status.th_status & 0x03]);
    
    printf("  Overcurrent Detection: %s\n", !status.ocd ? "YES" : "NO");
    printf("  Stall Detection A: %s\n", !status.stall_a ? "YES" : "NO");
    printf("  Stall Detection B: %s\n", !status.stall_b ? "YES" : "NO");

    return 0;
}

static void _registerGetStatus(void)
{
    getStatusArgs.motor = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    getStatusArgs.raw   = arg_lit0("r", "raw", "Output raw register value as uint16_t");
    getStatusArgs.end   = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "stepper-get-status",
        .help = "get motor status information",
        .hint = NULL,
        .func = &getStatus,
        .argtable = &getStatusArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/** 'clear-status' */
static struct {
    struct arg_int *motor;
    struct arg_end *end;
} clearStatusArgs;

static int clearStatus(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &clearStatusArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, clearStatusArgs.end, argv[0]);
        return 1;
    }

    MotorNum_t motor = (MotorNum_t)(clearStatusArgs.motor->ival[0] - 1);

    MotorStepper::clearStatus(motor);

    return 0;
}

static void _registerClearStatus(void)
{
    clearStatusArgs.motor = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    clearStatusArgs.end   = arg_end(1);

    const esp_console_cmd_t cmd = {
        .command = "stepper-clear-status",
        .help = "clear motor status",
        .hint = NULL,
        .func = &clearStatus,
        .argtable = &clearStatusArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}


int MotorStepper::_registerCLI(void)
{
    _registerAttachLimitSwitch();
    _registerDetachLimitSwitch();
    _registerStepResolution();
    _registerSetMaxSpeed();
    _registerSetAcceleration();
    _registerSetDeceleration();
    _registerSetMinSpeed();
    _registerSetFullStepSpeed();
    _registerGetPosition();
    _registerGetSpeed();
    _registerGetStatus();
    _registerClearStatus();
    _registerResetHomePosition();
    _registerSetPosition();
    _registerStop();
    _registerMoveAbsolute();
    _registerMoveRelative();
    _registerRun();
    _registerHoming();
    _registerGetSupplyVoltage();

    return 0;
}
