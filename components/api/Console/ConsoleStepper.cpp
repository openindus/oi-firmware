/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file ConsoleStepper.h
 * @brief OpenIndus console
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "ConsoleStepper.h"
#include "argtable3/argtable3.h"

#if defined(CONFIG_STEPPER) || defined(CONFIG_STEPPER_VE)

/* declare structures for argtable3 */
static struct {
    struct arg_int *motor;
    struct arg_rex *cmd;
    struct arg_end *end;
} resetArgtable;

static struct {
    struct arg_int *motor;
    struct arg_rex *cmd;
    struct arg_str *parameter;
    struct arg_dbl *arguments;
    struct arg_end *end;
} setAdvancedParametersArgtable;

static struct {
    struct arg_int *motor;
    struct arg_rex *cmd;
    struct arg_str *parameter;
    struct arg_end *end;
} getAdvancedParametersArgtable;

void ConsoleStepper::registerCli(void)
{
    _registerDigitalRead();
    _registerAttachInterrupt();
    _registerDetachInterrupt();
    _registerLimitSwitch();
    _registerStepResolution();
    _registerSetSpeed();
    _registerGetPosition();
    _registerGetSpeed();
    _registerStop();
    _registerMoveAbsolute();
    _registerMoveRelative();
    _registerRun();
    _registerHoming();
    _registerDevice();
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

    printf("%d\n", StepperStandalone::digitalRead(din));

    return 0;
}

void ConsoleStepper::_registerDigitalRead(void)
{
    digitalReadArgs.din    = arg_int1(NULL, NULL, "DIN", "[1-10]");
    digitalReadArgs.end     = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "digital-read",
        .help = "Get DIN level",
        .hint = NULL,
        .func = &dRead,
        .argtable = &digitalReadArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/** 'attach-interrupt' */

static struct {
    struct arg_int *din;
    struct arg_int *mode;
    struct arg_end *end;
} attachInterruptArgs;

static void callback(void* arg)
{
    printf("isr\n");
}

static int attachInterrupt(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &attachInterruptArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, attachInterruptArgs.end, argv[0]);
        return 1;
    }

    DigitalInputNum_t din = (DigitalInputNum_t)(attachInterruptArgs.din->ival[0] - 1);
    InterruptMode_t mode = (InterruptMode_t)(attachInterruptArgs.mode->ival[0]);

    StepperStandalone::attachInterrupt(din, callback, mode, NULL);

    return 0;
}

void ConsoleStepper::_registerAttachInterrupt(void)
{
    attachInterruptArgs.din    = arg_int1(NULL, NULL, "DIN", "[1-10]");
    attachInterruptArgs.mode    = arg_int1(NULL, NULL, "MODE", "[1: rising, 2: falling, 3: change]");
    attachInterruptArgs.end     = arg_end(4);

    const esp_console_cmd_t cmd = {
        .command = "attach-interrupt",
        .help = "attach interrupt",
        .hint = NULL,
        .func = &attachInterrupt,
        .argtable = &attachInterruptArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/** 'detach-interrupt' */

static struct {
    struct arg_int *din;
    struct arg_end *end;
} detachInterruptArgs;

static int detachInterrupt(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &detachInterruptArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, detachInterruptArgs.end, argv[0]);
        return 1;
    }

    DigitalInputNum_t din = (DigitalInputNum_t)(detachInterruptArgs.din->ival[0] - 1);

    StepperStandalone::detachInterrupt(din);

    return 0;
}

void ConsoleStepper::_registerDetachInterrupt(void)
{
    detachInterruptArgs.din    = arg_int1(NULL, NULL, "DIN", "[1-10]");
    detachInterruptArgs.end     = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "detach-interrupt",
        .help = "detach interrupt",
        .hint = NULL,
        .func = &detachInterrupt,
        .argtable = &detachInterruptArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/** 'limit-switch' */

static struct {
    struct arg_int *motor;
    struct arg_int *din;
    struct arg_int *logic;
    struct arg_end *end;
} limitSwitchArgs;

static int limitSwitch(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &limitSwitchArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, limitSwitchArgs.end, argv[0]);
        return 1;
    }

    MotorNum_t motor = (MotorNum_t)(limitSwitchArgs.motor->ival[0] - 1);
    DigitalInputNum_t din = (DigitalInputNum_t)(limitSwitchArgs.din->ival[0] - 1);

    if (limitSwitchArgs.logic->count > 0) {
        DigitalInputLogic_t logic = (DigitalInputLogic_t)(limitSwitchArgs.logic->ival[0]);
        StepperStandalone::setLimitSwitch(motor, din, logic);
    } else {
        StepperStandalone::setLimitSwitch(motor, din);
    }

    return 0;
}

void ConsoleStepper::_registerLimitSwitch(void)
{
    limitSwitchArgs.motor   = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    limitSwitchArgs.din    = arg_int1(NULL, NULL, "DIN", "[1-10]");
    limitSwitchArgs.logic   = arg_int0(NULL, NULL, "logic", "[0: Active low, 1: Active high]");
    limitSwitchArgs.end     = arg_end(4);

    const esp_console_cmd_t cmd = {
        .command = "limit-switch",
        .help = "set limit switch",
        .hint = NULL,
        .func = &limitSwitch,
        .argtable = &limitSwitchArgs
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

    StepperStandalone::setStepResolution(motor, res);

    return 0;
}

void ConsoleStepper::_registerStepResolution(void)
{
    stepResolutionArgs.motor    = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    stepResolutionArgs.res      = arg_int1(NULL, NULL, "RESOLUTION", "[0: Full step, 1: Half step, 2: 1/4 step, 3: 1/8 step, 4: 1/16 step, 5: 1/32 step, 6: 1/64 step, 7: 1/128 step]");
    stepResolutionArgs.end      = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "step-resolution",
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
} setSpeedArgs;

static int setSpeed(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &setSpeedArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, setSpeedArgs.end, argv[0]);
        return 1;
    }

    MotorNum_t motor = (MotorNum_t)(setSpeedArgs.motor->ival[0] - 1);
    float speed = (float)(setSpeedArgs.speed->ival[0]);

    StepperStandalone::setSpeed(motor, speed);

    return 0;
}

void ConsoleStepper::_registerSetSpeed(void)
{
    setSpeedArgs.motor  = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    setSpeedArgs.speed  = arg_int1(NULL, NULL, "SPEED", "motor speed in step/s");
    setSpeedArgs.end    = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "set-speed",
        .help = "set motor speed",
        .hint = NULL,
        .func = &setSpeed,
        .argtable = &setSpeedArgs
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

    printf("%d\n", StepperStandalone::getPosition(motor));

    return 0;
}

void ConsoleStepper::_registerGetPosition(void)
{
    getPositionArgs.motor  = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    getPositionArgs.end    = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "get-position",
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

    printf("%f\n", StepperStandalone::getSpeed(motor));

    return 0;
}

void ConsoleStepper::_registerGetSpeed(void)
{
    getSpeedArgs.motor  = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    getSpeedArgs.end    = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "get-speed",
        .help = "get current motor speed",
        .hint = NULL,
        .func = &getSpeed,
        .argtable = &getSpeedArgs
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

    StepperStandalone::stop(motor);

    return 0;
}

void ConsoleStepper::_registerStop(void)
{
    stopArgs.motor  = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    stopArgs.end    = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "stop",
        .help = "stop the motor",
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

    StepperStandalone::moveAbsolute(motor, position);

    return 0;
}

void ConsoleStepper::_registerMoveAbsolute(void)
{
    moveAbsoluteArgs.motor  = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    moveAbsoluteArgs.pos    = arg_int1(NULL, NULL, "POSITION", "position in step");
    moveAbsoluteArgs.end    = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "move-absolute",
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
    uint32_t position = (uint32_t)(moveRelativeArgs.pos->ival[0]);

    StepperStandalone::moveRelative(motor, position);

    return 0;
}

void ConsoleStepper::_registerMoveRelative(void)
{
    moveRelativeArgs.motor  = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    moveRelativeArgs.pos    = arg_int1(NULL, NULL, "POSITION", "position in step");
    moveRelativeArgs.end    = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "move-relative",
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

    StepperStandalone::run(motor, direction, speed);

    return 0;
}

void ConsoleStepper::_registerRun(void)
{
    runArgs.motor   = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    runArgs.dir     = arg_int1(NULL, NULL, "DIRECTION", "[0: Forward, 1: Reverse]");
    runArgs.speed   = arg_int1(NULL, NULL, "SPEED", "speed in step/s");
    runArgs.end     = arg_end(4);

    const esp_console_cmd_t cmd = {
        .command = "run",
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

    StepperStandalone::homing(motor, speed);

    return 0;
}

void ConsoleStepper::_registerHoming(void)
{
    homingArgs.motor    = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    homingArgs.speed    = arg_int1(NULL, NULL, "SPEED", "speed in step/s");
    homingArgs.end      = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "homing",
        .help = "homing",
        .hint = NULL,
        .func = &homing,
        .argtable = &homingArgs
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

static bool eq(const char * reference, const char * to_compare) {
    size_t length = strlen(reference);
    if (length != strlen(to_compare)) return false;
    return strncmp(reference, to_compare, length) == 0;
}

static void deviceReset(uint8_t motor)
{
    uint8_t device = motor - 1;
    PS01_Hal_Reset(device);
    vTaskDelay(10/portTICK_PERIOD_MS);
    PS01_Hal_ReleaseReset(device);
    vTaskDelay(10/portTICK_PERIOD_MS);
    int errorCode = MotorStepper::setAllAdvancedParamPS01((MotorNum_t) device, defaultParameters);
    if (errorCode != 0) {
        printf("Error while setting default parameters\n");
    } else {
        MotorStepper::setNVSParam((MotorNum_t) device, defaultParameters);
    }
}

int deviceSetAdvancedParameter(uint8_t motorId, const char* parameter, double *args, int nargs) {
    uint8_t deviceId = motorId - 1;
    /* set given parameter */
    if (eq(parameter, POWERSTEP01_PARAM_ABS_POS) && nargs == 1) {
        printf("set abs pos\n");
        int32_t absPos = (int32_t) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, ABS_POS, &absPos);

    } else if (eq(parameter, POWERSTEP01_PARAM_EL_POS_MICROSTEP) && nargs == 1) {
        printf("set el pos microstep\n");
        int32_t elPosMicrostep = (int32_t) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, EL_POS_MICROSTEP, &elPosMicrostep);
    
    } else if (eq(parameter, POWERSTEP01_PARAM_EL_POS_STEP) && nargs == 1) {
        printf("set el pos step\n");
        int32_t elPosStep = (int32_t) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, EL_POS_STEP, &elPosStep);

    } else if (eq(parameter, POWERSTEP01_PARAM_MARK) && nargs == 1) {
        printf("set mark\n");
        int32_t mark = (int32_t) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, MARK, &mark);

    } else if (eq(parameter, POWERSTEP01_PARAM_ACC) && nargs == 1) {
        printf("set acc\n");
        float acc = (float) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, ACC, &acc);

    } else if (eq(parameter, POWERSTEP01_PARAM_DEC) && nargs == 1) {
        printf("set dec\n");
        float dec = (float) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, DEC, &dec);

    } else if (eq(parameter, POWERSTEP01_PARAM_MAX_SPEED) && nargs == 1) {
        printf("set max speed\n");
        float maxSpeed = (float) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, MAX_SPEED, &maxSpeed);

    } else if (eq(parameter, POWERSTEP01_PARAM_MIN_SPEED) && nargs == 1) {
        printf("set min speed\n");
        float minSpeed = (float) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, MIN_SPEED, &minSpeed);

    } else if (eq(parameter, POWERSTEP01_PARAM_MIN_SPEED_LSPD_OPT) && nargs == 1) {
        printf("set min speed lsdp opt\n");
        bool minSpeedLspdOpt = (bool) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, MIN_SPEED_LSPD_OPT, &minSpeedLspdOpt);

    } else if (eq(parameter, POWERSTEP01_PARAM_FS_SPD) && nargs == 1) {
        printf("set fs spd\n");
        float fsSpd = (float) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, FS_SPD, &fsSpd);
    
    } else if (eq(parameter, POWERSTEP01_PARAM_FS_SPD_BOOST_MODE) && nargs == 1) {
        printf("set fs spd boost mode\n");
        bool fsSpdBoostMode = (bool) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, FS_SPD_BOOST_MODE, &fsSpdBoostMode);

    } else if (eq(parameter, POWERSTEP01_PARAM_OCD_TH) && nargs == 1) {
        printf("set ocd th\n");
        float ocdTh = (float) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, OCD_TH, &ocdTh);
    
    } else if (eq(parameter, POWERSTEP01_PARAM_STEP_MODE_STEP_SEL) && nargs == 1) {
        printf("set step mode step sel\n");
        uint8_t stepModeStepSel = (uint8_t) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, STEP_MODE_STEP_SEL, &stepModeStepSel);

    } else if (eq(parameter, POWERSTEP01_PARAM_STEP_MODE_CM_VM) && nargs == 1) {
        printf("set step mode cm vm\n");
        uint8_t stepModeCmVm = (uint8_t) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, STEP_MODE_CM_VM, &stepModeCmVm);

    } else if (eq(parameter, POWERSTEP01_PARAM_STEP_MODE_SYNC_SEL) && nargs == 1) {
        printf("set step mode sync sel\n");
        uint8_t stepModeSyncSel = (uint8_t) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, STEP_MODE_SYNC_SEL, &stepModeSyncSel);

    } else if (eq(parameter, POWERSTEP01_PARAM_STEP_MODE_SYNC_EN) && nargs == 1) {
        printf("set step mode sync en\n");
        uint8_t stepModeSyncEn = (uint8_t) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, STEP_MODE_SYNC_EN, &stepModeSyncEn);

    } else if (eq(parameter, POWERSTEP01_PARAM_ALARM_EN_OVERCURRENT) && nargs == 1) {
        printf("set alarm en overcurrent\n");
        bool alarmEnOvercurrent = (bool) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, ALARM_EN_OVERCURRENT, &alarmEnOvercurrent);

    } else if (eq(parameter, POWERSTEP01_PARAM_ALARM_EN_THERMAL_SHUTDOWN) && nargs == 1) {
        printf("set alarm en thermal shutdown\n");
        bool alarmEnThermalShutdown = (bool) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, ALARM_EN_THERMAL_SHUTDOWN, &alarmEnThermalShutdown);
        
    } else if (eq(parameter, POWERSTEP01_PARAM_ALARM_EN_THERMAL_WARNING) && nargs == 1) {
        printf("set alarm en thermal warning\n");
        bool alarmEnThermalWarning = (bool) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, ALARM_EN_THERMAL_WARNING, &alarmEnThermalWarning);

    } else if (eq(parameter, POWERSTEP01_PARAM_ALARM_EN_UVLO) && nargs == 1) {
        printf("set alarm en uvlo\n");
        bool alarmEnUvlo = (bool) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, ALARM_EN_UVLO, &alarmEnUvlo);

    } else if (eq(parameter, POWERSTEP01_PARAM_ALARM_EN_ADC_UVLO) && nargs == 1) {
        printf("set alarm en adc uvlo\n");
        bool alarmEnAdcUvlo = (bool) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, ALARM_EN_ADC_UVLO, &alarmEnAdcUvlo);
    
    } else if (eq(parameter, POWERSTEP01_PARAM_ALARM_EN_STALL_DETECT) && nargs == 1) {
        printf("set alarm en stall detect\n");
        bool alarmEnStallDetect = (bool) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, ALARM_EN_STALL_DETECTION, &alarmEnStallDetect);

    } else if (eq(parameter, POWERSTEP01_PARAM_ALARM_EN_SW_TURN_ON) && nargs == 1) {
        printf("set alarm en sw turn on\n");
        bool alarmEnSwTurnOn = (bool) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, ALARM_EN_SW_TURN_ON, &alarmEnSwTurnOn);

    } else if (eq(parameter, POWERSTEP01_PARAM_ALARM_EN_COMMAND_ERROR) && nargs == 1) {
        printf("set alarm en command error\n");
        bool alarmEnCommandError = (bool) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, ALARM_EN_COMMAND_ERROR, &alarmEnCommandError);

    } else if (eq(parameter, POWERSTEP01_PARAM_GATECFG1_TCC) && nargs == 1) {
        printf("set gatecfg1 tcc\n");
        uint8_t gatecfg1Tcc = (uint8_t) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, GATECFG1_TCC, &gatecfg1Tcc);

    } else if (eq(parameter, POWERSTEP01_PARAM_GATECFG1_IGATE) && nargs == 1) {
        printf("set gatecfg1 igate\n");
        uint8_t gatecfg1Igate = (uint8_t) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, GATECFG1_IGATE, &gatecfg1Igate);

    } else if (eq(parameter, POWERSTEP01_PARAM_GATECFG1_TBOOST) && nargs == 1) {
        printf("set gatecfg1 tboost\n");
        uint8_t gatecfg1Tboost = (uint8_t) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, GATECFG1_TBOOST, &gatecfg1Tboost);

    } else if (eq(parameter, POWERSTEP01_PARAM_GATECFG1_WD_EN) && nargs == 1) {
        printf("set gatecfg1 wd en\n");
        uint8_t gatecfg1WdEn = (uint8_t) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, GATECFG1_WD_EN, &gatecfg1WdEn);

    } else if (eq(parameter, POWERSTEP01_PARAM_GATECFG2_TDT) && nargs == 1) {
        printf("set gatecfg2 tdt\n");
        uint8_t gatecfg2Tdt = (uint8_t) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, GATECFG2_TDT, &gatecfg2Tdt);

    } else if (eq(parameter, POWERSTEP01_PARAM_GATECFG2_TBLANK) && nargs == 1) {
        printf("set gatecfg2 tblank\n");
        uint8_t gatecfg2Tblank = (uint8_t) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, GATECFG2_TBLANK, &gatecfg2Tblank);

    } else if (eq(parameter, POWERSTEP01_PARAM_CONFIG_OSC_SEL) && nargs == 1) {
        printf("set config osc sel\n");
        uint8_t configOscSel = (uint8_t) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, CONFIG_OSC_SEL, &configOscSel);
    
    } else if (eq(parameter, POWERSTEP01_PARAM_CONFIG_EXT_CLK) && nargs == 1) {
        printf("set config ext clk\n");
        uint8_t configExtClk = (uint8_t) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, CONFIG_EXT_CLK, &configExtClk);

    } else if (eq(parameter, POWERSTEP01_PARAM_CONFIG_SW_MODE) && nargs == 1) {
        printf("set config sw mode\n");
        uint8_t configSwMode = (uint8_t) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, CONFIG_SW_MODE, &configSwMode);

    } else if (eq(parameter, POWERSTEP01_PARAM_CONFIG_OC_SD) && nargs == 1) {
        printf("set config oc sd\n");
        uint8_t configOcSd = (uint8_t) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, CONFIG_OC_SD, &configOcSd);

    } else if (eq(parameter, POWERSTEP01_PARAM_CONFIG_UVLOVAL) && nargs == 1) {
        printf("set config uvloval\n");
        uint8_t configUvloval = (uint8_t) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, CONFIG_UVLOVAL, &configUvloval);

    } else if (eq(parameter, POWERSTEP01_PARAM_CONFIG_VCCVAL) && nargs == 1) {
        printf("set config vccval\n");
        uint8_t configVccval = (uint8_t) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, CONFIG_VCCVAL, &configVccval);

#ifdef CONFIG_VOLTAGE_MODE

    } else if (eq(parameter, POWERSTEP01_PARAM_CONFIG_EN_VSCOMP) && nargs == 1) {
        printf("set config en vscomp\n");
        uint8_t configEnVscomp = (uint8_t) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, VM_CONFIG_EN_VSCOMP, &configEnVscomp);

    } else if (eq(parameter, POWERSTEP01_PARAM_CONFIG_F_PWM_DEC) && nargs == 1) {
        printf("set config f pwm dec\n");
        uint8_t configFPwmDec = (uint8_t) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, VM_CONFIG_F_PWM_DEC, &configFPwmDec);

    } else if (eq(parameter, POWERSTEP01_PARAM_CONFIG_F_PWM_INT) && nargs == 1) {
        printf("set config f pwm int\n");
        uint8_t configFPwmInt = (uint8_t) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, VM_CONFIG_F_PWM_INT, &configFPwmInt);

    } else if (eq(parameter, POWERSTEP01_PARAM_KVAL_HOLD) && nargs == 1) {
        printf("set kval hold\n");
        float kvalHold = (float) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, VM_KVAL_HOLD, &kvalHold);

    } else if (eq(parameter, POWERSTEP01_PARAM_KVAL_RUN) && nargs == 1) {
        printf("set kval run\n");
        float kvalRun = (float) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, VM_KVAL_RUN, &kvalRun);

    } else if (eq(parameter, POWERSTEP01_PARAM_KVAL_ACC) && nargs == 1) {
        printf("set kval acc\n");
        float kvalAcc = (float) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, VM_KVAL_ACC, &kvalAcc);

    } else if (eq(parameter, POWERSTEP01_PARAM_KVAL_DEC) && nargs == 1) {
        printf("set kval dec\n");
        float kvalDec = (float) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, VM_KVAL_DEC, &kvalDec);

    } else if (eq(parameter, POWERSTEP01_PARAM_INT_SPEED) && nargs == 1) {
        printf("set int speed\n");
        float intSpeed = (float) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, VM_INT_SPEED, &intSpeed);

    } else if (eq(parameter, POWERSTEP01_PARAM_ST_SLP) && nargs == 1) {
        printf("set st slp\n");
        float stSlp = (float) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, VM_ST_SLP, &stSlp);

    } else if (eq(parameter, POWERSTEP01_PARAM_FN_SLP_ACC) && nargs == 1) {
        printf("set fn slp acc\n");
        float fnSlpAcc = (float) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, VM_FN_SLP_ACC, &fnSlpAcc);

    } else if (eq(parameter, POWERSTEP01_PARAM_FN_SLP_DEC) && nargs == 1) {
        printf("set fn slp dec\n");
        float fnSlpDec = (float) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, VM_FN_SLP_DEC, &fnSlpDec);
    
    } else if (eq(parameter, POWERSTEP01_PARAM_K_THERM) && nargs == 1) {
        printf("set k therm\n");
        float kTherm = (float) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, VM_K_THERM, &kTherm);

    } else if (eq(parameter, POWERSTEP01_PARAM_STALL_TH) && nargs == 1) {
        printf("set stall th\n");
        float stallTh = (float) args[0];
        MotorStepper::setAdvancedParam((MotorNum_t) deviceId, VM_STALL_TH, &stallTh);

#else //config current mode
    //TODO implement current mode
#endif
    
    } else {
        printf("command not recognized: set device [%d] param [%s] nargs [%d]\n", deviceId, parameter, nargs);
        return 1;
    }
    return 0;
}

int deviceGetAdvancedParameter(uint8_t motorId, const char* parameter) {
    uint8_t deviceId = motorId - 1;
    int errorCode = 0;
    if (eq(parameter, POWERSTEP01_PARAM_ABS_POS)) {
        int32_t absPos = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, ABS_POS, &absPos);
        printf("%d\n", absPos);
    
    } else if (eq(parameter, POWERSTEP01_PARAM_EL_POS_MICROSTEP)) {
        errorCode = 1;
        printf("not implemented\n");
    
    } else if (eq(parameter, POWERSTEP01_PARAM_EL_POS_STEP)) {
        errorCode = 1;
        printf("not implemented\n");
    
    } else if (eq(parameter, POWERSTEP01_PARAM_MARK)) {
        int32_t mark = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, MARK, &mark);
        printf("%d\n", mark);

    } else if (eq(parameter, POWERSTEP01_PARAM_SPEED)) {
        float speed = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, SPEED, &speed);
        printf("%f\n", speed);

    } else if (eq(parameter, POWERSTEP01_PARAM_ACC)) {
        float acc = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, ACC, &acc);
        printf("%f\n", acc);

    } else if (eq(parameter, POWERSTEP01_PARAM_DEC)) {
        float dec = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, DEC, &dec);
        printf("%f\n", dec);

    } else if (eq(parameter, POWERSTEP01_PARAM_MAX_SPEED)) {
        float maxSpeed = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, MAX_SPEED, &maxSpeed);
        printf("%f\n", maxSpeed);

    } else if (eq(parameter, POWERSTEP01_PARAM_MIN_SPEED)) {
        float minSpeed = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, MIN_SPEED, &minSpeed);
        printf("%f\n", minSpeed);

    } else if (eq(parameter, POWERSTEP01_PARAM_MIN_SPEED_LSPD_OPT)) {
        bool minSpeedLspdOpt = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, MIN_SPEED_LSPD_OPT, &minSpeedLspdOpt);
        printf("%d\n", minSpeedLspdOpt);

    } else if (eq(parameter, POWERSTEP01_PARAM_ADC_OUT)) {
        errorCode = 1;
        printf("not implemented\n");

    } else if (eq(parameter, POWERSTEP01_PARAM_OCD_TH)) {
        errorCode = 1;
        printf("not implemented\n");

    } else if (eq(parameter, POWERSTEP01_PARAM_FS_SPD)) {
        float fsSpd = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, FS_SPD, &fsSpd);
        printf("%f\n", fsSpd);

    } else if (eq(parameter, POWERSTEP01_PARAM_FS_SPD_BOOST_MODE)) {
        bool fsSpdBoostMode = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, FS_SPD_BOOST_MODE, &fsSpdBoostMode);
        printf("%d\n", fsSpdBoostMode);

    } else if (eq(parameter, POWERSTEP01_PARAM_STEP_MODE_STEP_SEL)) {
        uint8_t stepModeStepSel = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, STEP_MODE_STEP_SEL, &stepModeStepSel);
        printf("%d\n", stepModeStepSel);

    } else if (eq(parameter, POWERSTEP01_PARAM_STEP_MODE_CM_VM)) {
        uint8_t stepModeCmVm = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, STEP_MODE_CM_VM, &stepModeCmVm);
        printf("%d\n", stepModeCmVm);

    } else if (eq(parameter, POWERSTEP01_PARAM_STEP_MODE_SYNC_SEL)) {
        uint8_t stepModeSyncSel = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, STEP_MODE_SYNC_SEL, &stepModeSyncSel);
        printf("%d\n", stepModeSyncSel);

    } else if (eq(parameter, POWERSTEP01_PARAM_STEP_MODE_SYNC_EN)) {
        uint8_t stepModeSyncEn = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, STEP_MODE_SYNC_EN, &stepModeSyncEn);
        printf("%d\n", stepModeSyncEn);

    } else if (eq(parameter, POWERSTEP01_PARAM_ALARM_EN_OVERCURRENT)) {
        uint8_t alarmEnOvercurrent = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, ALARM_EN_OVERCURRENT, &alarmEnOvercurrent);
        printf("%d\n", alarmEnOvercurrent);

    } else if (eq(parameter, POWERSTEP01_PARAM_ALARM_EN_THERMAL_SHUTDOWN)) {
        uint8_t alarmEnThermalShutdown = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, ALARM_EN_THERMAL_SHUTDOWN, &alarmEnThermalShutdown);
        printf("%d\n", alarmEnThermalShutdown);

    } else if (eq(parameter, POWERSTEP01_PARAM_ALARM_EN_THERMAL_WARNING)) {
        uint8_t alarmEnThermalWarning = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, ALARM_EN_THERMAL_WARNING, &alarmEnThermalWarning);
        printf("%d\n", alarmEnThermalWarning);

    } else if (eq(parameter, POWERSTEP01_PARAM_ALARM_EN_UVLO)) {
        uint8_t alarmEnUvlo = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, ALARM_EN_UVLO, &alarmEnUvlo);
        printf("%d\n", alarmEnUvlo);

    } else if (eq(parameter, POWERSTEP01_PARAM_ALARM_EN_ADC_UVLO)) {
        uint8_t alarmEnAdcUvlo = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, ALARM_EN_ADC_UVLO, &alarmEnAdcUvlo);
        printf("%d\n", alarmEnAdcUvlo);

    } else if (eq(parameter, POWERSTEP01_PARAM_ALARM_EN_STALL_DETECT)) {
        uint8_t alarmEnStallDetect = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, ALARM_EN_STALL_DETECTION, &alarmEnStallDetect);
        printf("%d\n", alarmEnStallDetect);

    } else if (eq(parameter, POWERSTEP01_PARAM_ALARM_EN_SW_TURN_ON)) {
        uint8_t alarmEnSwTurnOn = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, ALARM_EN_SW_TURN_ON, &alarmEnSwTurnOn);
        printf("%d\n", alarmEnSwTurnOn);

    } else if (eq(parameter, POWERSTEP01_PARAM_ALARM_EN_COMMAND_ERROR)) {
        uint8_t alarmEnCommandError = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, ALARM_EN_COMMAND_ERROR, &alarmEnCommandError);
        printf("%d\n", alarmEnCommandError);

    } else if (eq(parameter, POWERSTEP01_PARAM_GATECFG1_TCC)) {
        uint8_t gateCfg1Tcc = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, GATECFG1_TCC, &gateCfg1Tcc);
        printf("%d\n", gateCfg1Tcc);

    } else if (eq(parameter, POWERSTEP01_PARAM_GATECFG1_IGATE)) {
        uint8_t gateCfg1Igate = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, GATECFG1_IGATE, &gateCfg1Igate);
        printf("%d\n", gateCfg1Igate);

    } else if (eq(parameter, POWERSTEP01_PARAM_GATECFG1_TBOOST)) {
        uint8_t gateCfg1Tboost = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, GATECFG1_TBOOST, &gateCfg1Tboost);
        printf("%d\n", gateCfg1Tboost);

    } else if (eq(parameter, POWERSTEP01_PARAM_GATECFG1_WD_EN)) {
        uint8_t gateCfg1WdEn = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, GATECFG1_WD_EN, &gateCfg1WdEn);
        printf("%d\n", gateCfg1WdEn);

    } else if (eq(parameter, POWERSTEP01_PARAM_GATECFG2_TDT)) {
        uint8_t gateCfg2Tdt = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, GATECFG2_TDT, &gateCfg2Tdt);
        printf("%d\n", gateCfg2Tdt);

    } else if (eq(parameter, POWERSTEP01_PARAM_GATECFG2_TBLANK)) {
        uint8_t gateCfg2Tblank = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, GATECFG2_TBLANK, &gateCfg2Tblank);
        printf("%d\n", gateCfg2Tblank);

#ifdef CONFIG_VOLTAGE_MODE
    } else if (eq(parameter, POWERSTEP01_PARAM_CONFIG_OSC_SEL)) {
        uint8_t oscSel = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, CONFIG_OSC_SEL, &oscSel);
        printf("%d\n", oscSel);
    } else if (eq(parameter, POWERSTEP01_PARAM_CONFIG_EXT_CLK)) {
        uint8_t extClk = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, CONFIG_EXT_CLK, &extClk);
        printf("%d\n", extClk);

    } else if (eq(parameter, POWERSTEP01_PARAM_CONFIG_SW_MODE)) {
        uint8_t swMode = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, CONFIG_SW_MODE, &swMode);
        printf("%d\n", swMode);

    } else if (eq(parameter, POWERSTEP01_PARAM_CONFIG_OC_SD)) {
        uint8_t ocSd = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, CONFIG_OC_SD, &ocSd);
        printf("%d\n", ocSd);

    } else if (eq(parameter, POWERSTEP01_PARAM_CONFIG_UVLOVAL)) {
        uint8_t uvloVal = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, CONFIG_UVLOVAL, &uvloVal);
        printf("%d\n", uvloVal);

    } else if (eq(parameter, POWERSTEP01_PARAM_CONFIG_VCCVAL)) {
        uint8_t vccVal = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, CONFIG_VCCVAL, &vccVal);
        printf("%d\n", vccVal);

    } else if (eq(parameter, POWERSTEP01_PARAM_CONFIG_EN_VSCOMP)) {
        uint8_t enVscomp = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, VM_CONFIG_EN_VSCOMP, &enVscomp);
        printf("%d\n", enVscomp);

    } else if (eq(parameter, POWERSTEP01_PARAM_CONFIG_F_PWM_DEC)) {
        uint8_t fPwmDec = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, VM_CONFIG_F_PWM_DEC, &fPwmDec);
        printf("%d\n", fPwmDec);

    } else if (eq(parameter, POWERSTEP01_PARAM_CONFIG_F_PWM_INT)) {
        uint8_t fPwmInt = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, VM_CONFIG_F_PWM_INT, &fPwmInt);
        printf("%d\n", fPwmInt);

    } else if (eq(parameter, POWERSTEP01_PARAM_KVAL_HOLD)) {
        float kvalHold = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, VM_KVAL_HOLD, &kvalHold);
        printf("%f\n", kvalHold);

    } else if (eq(parameter, POWERSTEP01_PARAM_KVAL_RUN)) {
        float kvalRun = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, VM_KVAL_RUN, &kvalRun);
        printf("%f\n", kvalRun);

    } else if (eq(parameter, POWERSTEP01_PARAM_KVAL_ACC)) {
        float kvalAcc = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, VM_KVAL_ACC, &kvalAcc);
        printf("%f\n", kvalAcc);

    } else if (eq(parameter, POWERSTEP01_PARAM_KVAL_DEC)) {
        float kvalDec = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, VM_KVAL_DEC, &kvalDec);
        printf("%f\n", kvalDec);

    } else if (eq(parameter, POWERSTEP01_PARAM_INT_SPEED)) {
        float intSpeed = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, VM_INT_SPEED, &intSpeed);
        printf("%f\n", intSpeed);

    } else if (eq(parameter, POWERSTEP01_PARAM_ST_SLP)) {
        float stSlp = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, VM_ST_SLP, &stSlp);
        printf("%f\n", stSlp);

    } else if (eq(parameter, POWERSTEP01_PARAM_FN_SLP_ACC)) {
        float fnSlpAcc = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, VM_FN_SLP_ACC, &fnSlpAcc);
        printf("%f\n", fnSlpAcc);

    } else if (eq(parameter, POWERSTEP01_PARAM_FN_SLP_DEC)) {
        float fnSlpDec = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, VM_FN_SLP_DEC, &fnSlpDec);
        printf("%f\n", fnSlpDec);

    } else if (eq(parameter, POWERSTEP01_PARAM_K_THERM)) {
        float kTherm = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, VM_K_THERM, &kTherm);
        printf("%f\n", kTherm);

    } else if (eq(parameter, POWERSTEP01_PARAM_STALL_TH)) {
        float stallTh = 0;
        errorCode = MotorStepper::getAdvancedParam((MotorNum_t) deviceId, VM_STALL_TH, &stallTh);
        printf("%f\n", stallTh);

#else
    // TODO current mode
#endif

    } else {
        printf("command not recognized: get device [%d] param [%s]\n", deviceId, parameter);
        return 1;
    }
    return errorCode;
}

static void initArgtables(void) {

    /* reset */
    resetArgtable.motor = arg_int1(NULL, NULL, "ID", "motor id [1-2]");
    resetArgtable.cmd   = arg_rex1(NULL, NULL, "reset", NULL, REG_ICASE, NULL);
    resetArgtable.end   = arg_end(20);

    /* set-advanced-parameters */
    setAdvancedParametersArgtable.motor     = arg_int1(NULL, NULL, "ID", "motor id [1-2]");
    setAdvancedParametersArgtable.cmd       = arg_rex1(NULL, NULL, "set", NULL, REG_ICASE, NULL);
    setAdvancedParametersArgtable.parameter = arg_str0(NULL, NULL, "<str>", "parameter");
    setAdvancedParametersArgtable.arguments = arg_dbln(NULL, NULL, "<double>", 1, 10, "arguments");
    setAdvancedParametersArgtable.end       = arg_end(20);

    /* get-advanced-parameters */
    getAdvancedParametersArgtable.motor     = arg_int1(NULL, NULL, "ID", "motor id [1-2]");
    getAdvancedParametersArgtable.cmd       = arg_rex1(NULL, NULL, "get", NULL, REG_ICASE, NULL);
    getAdvancedParametersArgtable.parameter = arg_str0(NULL, NULL, "<str>", "parameter");
    getAdvancedParametersArgtable.end       = arg_end(20);

}

static bool nullCheckArgtables(void) {
    bool thereAreNullEntries = false;
    thereAreNullEntries = thereAreNullEntries || arg_nullcheck((void **) &resetArgtable)!=0;
    thereAreNullEntries = thereAreNullEntries || arg_nullcheck((void **) &setAdvancedParametersArgtable)!=0;
    thereAreNullEntries = thereAreNullEntries || arg_nullcheck((void **) &getAdvancedParametersArgtable)!=0;
    return thereAreNullEntries;
}

static void freeArgtables(void) {
    arg_freetable((void **) &resetArgtable, sizeof(resetArgtable)/sizeof(resetArgtable.cmd));
    arg_freetable((void **) &setAdvancedParametersArgtable,sizeof(setAdvancedParametersArgtable)/sizeof(setAdvancedParametersArgtable.cmd));
    arg_freetable((void **) &getAdvancedParametersArgtable,sizeof(getAdvancedParametersArgtable)/sizeof(getAdvancedParametersArgtable.cmd));

}

static int stepperParser(int argc, char **argv) {

    /* allocate argtables */
    initArgtables();
    int exitcode = 0;

    /* verify all argtables were allocated successfully*/
    if (nullCheckArgtables()) {
        printf("Stepper console: insufficient memory\n");
        freeArgtables();
        return 1;
    }

    /* we parse each argtable to see if any of them are successful*/
    int resetErrors                 = arg_parse(argc, argv, (void **) &resetArgtable);
    int setAdvancedParametersErrors = arg_parse(argc, argv, (void **) &setAdvancedParametersArgtable);
    int getAdvancedParametersErrors = arg_parse(argc, argv, (void **) &getAdvancedParametersArgtable);

    if (resetErrors == 0) {
        deviceReset(resetArgtable.motor->ival[0]);
        exitcode = 0;

    } else if (setAdvancedParametersErrors == 0) {
        exitcode = deviceSetAdvancedParameter(
                setAdvancedParametersArgtable.motor->ival[0],
                setAdvancedParametersArgtable.parameter->sval[0],
                setAdvancedParametersArgtable.arguments->dval,
                setAdvancedParametersArgtable.arguments->count);

    } else if (getAdvancedParametersErrors == 0) {
        exitcode = deviceGetAdvancedParameter(
                getAdvancedParametersArgtable.motor->ival[0],
                getAdvancedParametersArgtable.parameter->sval[0]);
    } else {
        exitcode = 1;
    }
    freeArgtables();
    return exitcode;
}

void ConsoleStepper::_registerDevice(void)
{
    const esp_console_cmd_t cmd = {
        .command = "advanced-param",
        .help = "powerSTEP01 advanced param commands",
        .hint = "MOTOR <get/set/reset> PARAMETER ARGUMENT",
        .func = &stepperParser,
        .argtable = NULL
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

#endif
