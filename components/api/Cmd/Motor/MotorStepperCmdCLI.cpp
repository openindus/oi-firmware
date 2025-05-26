/**
 * @file MotorStepperCmdCLI.cpp
 * @brief Motor stepper command CLI
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "MotorStepperCmd.h"

#if defined(MODULE_MASTER)

#define MOTOR_MAX_NUM   2
#define DIN_MAX_NUM     4

static const char TAG[] = "MotorStepperCmdCLI";

enum AdvancedParameterType_e { TYPE_INT, TYPE_FLOAT };

struct AdvancedParameterInfo_s {
    AdvancedParameter_t param;
    AdvancedParameterType_e type;
};

/* Advanced parameters */
const std::map<std::string, AdvancedParameterInfo_s> advancedParamMap = {
    {"abs-pos", {ABS_POS, TYPE_INT}},
    {"el-pos-microstep", {EL_POS_MICROSTEP, TYPE_INT}},
    {"el-pos-step", {EL_POS_STEP, TYPE_INT}},
    {"mark", {MARK, TYPE_INT}},
    {"speed", {SPEED, TYPE_FLOAT}},
    {"acc", {ACCELERATION, TYPE_FLOAT}},
    {"dec", {DECELERATION, TYPE_FLOAT}},
    {"max-speed", {MAX_SPEED, TYPE_FLOAT}},
    {"min-speed", {MIN_SPEED, TYPE_FLOAT}},
    {"min-speed-lspd-opt", {MIN_SPEED_LSPD_OPT, TYPE_INT}},
    {"adc-out", {ADC_OUT, TYPE_INT}},
    {"ocd-th", {OCD_TH, TYPE_INT}},
    {"fs-spd", {FS_SPD, TYPE_FLOAT}},
    {"fs-spd-boost-mode", {FS_SPD_BOOST_MODE, TYPE_INT}},
    {"step-mode-step-sel", {STEP_MODE_STEP_SEL, TYPE_INT}},
    {"step-mode-cm-vm", {STEP_MODE_CM_VM, TYPE_INT}},
    {"step-mode-sync-sel", {STEP_MODE_SYNC_SEL, TYPE_INT}},
    {"step-mode-sync-en", {STEP_MODE_SYNC_EN, TYPE_INT}},
    {"alarm-en-overcurrent", {ALARM_EN_OVERCURRENT, TYPE_INT}},
    {"alarm-en-thermal-shutdown", {ALARM_EN_THERMAL_SHUTDOWN, TYPE_INT}},
    {"alarm-en-thermal-warning", {ALARM_EN_THERMAL_WARNING, TYPE_INT}},
    {"alarm-en-uvlo", {ALARM_EN_UVLO, TYPE_INT}},
    {"alarm-en-adc-uvlo", {ALARM_EN_ADC_UVLO, TYPE_INT}},
    {"alarm-en-stall-detect", {ALARM_EN_STALL_DETECTION, TYPE_INT}},
    {"alarm-en-sw-turn-on", {ALARM_EN_SW_TURN_ON, TYPE_INT}},
    {"alarm-en-command-error", {ALARM_EN_COMMAND_ERROR, TYPE_INT}},
    {"gatecfg1-tcc", {GATECFG1_TCC, TYPE_INT}},
    {"gatecfg1-igate", {GATECFG1_IGATE, TYPE_INT}},
    {"gatecfg1-tboost", {GATECFG1_TBOOST, TYPE_INT}},
    {"gatecfg1-wd-en", {GATECFG1_WD_EN, TYPE_INT}},
    {"gatecfg2-tdt", {GATECFG2_TDT, TYPE_INT}},
    {"gatecfg2-tblank", {GATECFG2_TBLANK, TYPE_INT}},
    {"config-osc-sel", {CONFIG_OSC_SEL, TYPE_INT}},
    {"config-ext-clk", {CONFIG_EXT_CLK, TYPE_INT}},
    {"config-sw-mode", {CONFIG_SW_MODE, TYPE_INT}},
    {"config-oc-sd", {CONFIG_OC_SD, TYPE_INT}},
    {"config-uvloval", {CONFIG_UVLOVAL, TYPE_INT}},
    {"config-vccval", {CONFIG_VCCVAL, TYPE_INT}},
#ifdef CONFIG_VOLTAGE_MODE
    {"config-en-vscomp", {VM_CONFIG_EN_VSCOMP, TYPE_INT}},
    {"config-f-pwm-dec", {VM_CONFIG_F_PWM_DEC, TYPE_INT}},
    {"config-f-pwm-int", {VM_CONFIG_F_PWM_INT, TYPE_INT}},
    {"kval-hold", {VM_KVAL_HOLD, TYPE_FLOAT}},
    {"kval-run", {VM_KVAL_RUN, TYPE_FLOAT}},
    {"kval-acc", {VM_KVAL_ACC, TYPE_FLOAT}},
    {"kval-dec", {VM_KVAL_DEC, TYPE_FLOAT}},
    {"int-speed", {VM_INT_SPEED, TYPE_FLOAT}},
    {"st-slp", {VM_ST_SLP, TYPE_FLOAT}},
    {"fn-slp-acc", {VM_FN_SLP_ACC, TYPE_FLOAT}},
    {"fn-slp-dec", {VM_FN_SLP_DEC, TYPE_FLOAT}},
    {"k-therm", {VM_K_THERM, TYPE_FLOAT}},
    {"stall-th", {VM_STALL_TH, TYPE_INT}}
#else
    {"config-en-tqreg", {CM_CONFIG_EN_TQREG, TYPE_INT}},
    {"config-tsw", {CM_CONFIG_TSW, TYPE_INT}},
    {"config-pred-en", {CM_CONFIG_PRED_EN, TYPE_INT}},
    {"tval-hold", {CM_TVAL_HOLD, TYPE_FLOAT}},
    {"tval-run", {CM_TVAL_RUN, TYPE_FLOAT}},
    {"tval-acc", {CM_TVAL_ACC, TYPE_FLOAT}},
    {"tval-dec", {CM_TVAL_DEC, TYPE_FLOAT}},
    {"t-fast-fast-step", {CM_TFAST_FAST_STEP, TYPE_INT}},
    {"t-fast-toff-fast", {CM_TFAST_TOFF_FAST, TYPE_INT}},
    {"ton-min", {CM_TON_MIN, TYPE_FLOAT}},
    {"toff-min", {CM_TOFF_MIN, TYPE_FLOAT}}
#endif
};

static struct {
    struct arg_int *id;
    struct arg_str *cmd;
    struct arg_int *motor;
    struct arg_str *param;
    struct arg_rex *value;
    struct arg_end *end;
} advancedParamArgs;

static int advancedParamFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, advancedParamArgs);

    uint16_t id      = advancedParamArgs.id->ival[0];
    MotorNum_t motor = (MotorNum_t)(advancedParamArgs.motor->ival[0] - 1);

    if (motor >= MOTOR_MAX_NUM) {
        ESP_LOGE(TAG, "Invalid motor number: %d. Must be between %d and %d", 
                 advancedParamArgs.motor->ival[0], 0, MOTOR_MAX_NUM);
        return 1;
    }
    
    AdvancedParameterInfo_s AdvancedParameterInfo_s =
        advancedParamMap.at(advancedParamArgs.param->sval[0]);

    MotorStepperCmd *stepper = new MotorStepperCmd(id);
    if (stepper == nullptr) {
        ESP_LOGE(TAG, "Failed to create MotorStepperCmd instance");
        return 1;
    }

    if (strcmp(advancedParamArgs.cmd->sval[0], "get") == 0) {
        if (AdvancedParameterInfo_s.type == TYPE_FLOAT) {
            float value = 0.0f;
            stepper->getAdvancedParam(motor, AdvancedParameterInfo_s.param, &value);
            printf("%f\n", value);
        } else {
            int value = 0;
            stepper->getAdvancedParam(motor, AdvancedParameterInfo_s.param, &value);
            printf("%d\n", value);
        }
    } else if (strcmp(advancedParamArgs.cmd->sval[0], "set") == 0) {
        const char *str_value = advancedParamArgs.value->sval[0];
        if (AdvancedParameterInfo_s.type == TYPE_FLOAT) {
            float value = strtof(str_value, NULL);
            stepper->setAdvancedParam(motor, AdvancedParameterInfo_s.param, &value);
        } else {
            int value = strtoul(str_value, NULL, 10);
            stepper->setAdvancedParam(motor, AdvancedParameterInfo_s.param, &value);
        }
    }

    delete stepper;

    return 0;
}

static struct {
    struct arg_int *id;
    struct arg_int *motor;
    struct arg_int *din;
    struct arg_str *logic;
    struct arg_end *end;
} attachLimitSwitchArgs;

static int attachLimitSwitchFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, attachLimitSwitchArgs);

    uint16_t id = attachLimitSwitchArgs.id->ival[0];
    MotorNum_t motor = (MotorNum_t)(attachLimitSwitchArgs.motor->ival[0] - 1);
    DIn_Num_t din = (DIn_Num_t)(attachLimitSwitchArgs.din->ival[0] - 1);
    Logic_t logic = ACTIVE_HIGH;

    if (motor >= MOTOR_MAX_NUM) {
        ESP_LOGE(TAG, "Invalid motor number: %d. Must be between %d and %d", 
                 advancedParamArgs.motor->ival[0], 0, MOTOR_MAX_NUM);
        return 1;
    }

    if (din >= DIN_MAX_NUM) {
        ESP_LOGE(TAG, "Invalid DIN number: %d. Must be between %d and %d", 
                 attachLimitSwitchArgs.din->ival[0], 0, DIN_MAX_NUM);
        return 1;
    }
    
    if (attachLimitSwitchArgs.logic->count > 0) {
        if (strcmp(attachLimitSwitchArgs.logic->sval[0], "low") == 0) {
            logic = ACTIVE_LOW;
        }
    }

    MotorStepperCmd *stepper = new MotorStepperCmd(id);
    if (stepper == nullptr) {
        ESP_LOGE(TAG, "Failed to create MotorStepperCmd instance");
        return 1;
    }

    stepper->attachLimitSwitch(motor, din, logic);

    delete stepper;

    return 0;
}

static struct {
    struct arg_int *id;
    struct arg_int *motor;
    struct arg_int *din;
    struct arg_end *end;
} detachLimitSwitchArgs;

static int detachLimitSwitchFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, detachLimitSwitchArgs);

    uint16_t id = detachLimitSwitchArgs.id->ival[0];
    MotorNum_t motor = (MotorNum_t)(detachLimitSwitchArgs.motor->ival[0] - 1);
    DIn_Num_t din = (DIn_Num_t)(detachLimitSwitchArgs.din->ival[0] - 1);

    if (motor >= MOTOR_MAX_NUM) {
        ESP_LOGE(TAG, "Invalid motor number: %d. Must be between %d and %d", 
                 detachLimitSwitchArgs.motor->ival[0], 0, MOTOR_MAX_NUM);
        return 1;
    }

    if (din >= DIN_MAX_NUM) {
        ESP_LOGE(TAG, "Invalid DIN number: %d. Must be between %d and %d", 
                 detachLimitSwitchArgs.din->ival[0], 0, DIN_MAX_NUM);
        return 1;
    }

    MotorStepperCmd *stepper = new MotorStepperCmd(id);
    if (stepper == nullptr) {
        ESP_LOGE(TAG, "Failed to create MotorStepperCmd instance");
        return 1;
    }

    stepper->detachLimitSwitch(motor, din);

    delete stepper;

    return 0;
}

static struct {
    struct arg_int *id;
    struct arg_int *motor;
    struct arg_int *resolution;
    struct arg_end *end;
} setStepResolutionArgs;

static int setStepResolutionFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, setStepResolutionArgs);

    uint16_t id = setStepResolutionArgs.id->ival[0];
    MotorNum_t motor = (MotorNum_t)(setStepResolutionArgs.motor->ival[0] - 1);
    MotorStepResolution_t resolution = (MotorStepResolution_t)setStepResolutionArgs.resolution->ival[0];

    if (motor >= MOTOR_MAX_NUM) {
        ESP_LOGE(TAG, "Invalid motor number: %d. Must be between %d and %d", 
                 setStepResolutionArgs.motor->ival[0], 0, MOTOR_MAX_NUM);
        return 1;
    }

    MotorStepperCmd *stepper = new MotorStepperCmd(id);
    if (stepper == nullptr) {
        ESP_LOGE(TAG, "Failed to create MotorStepperCmd instance");
        return 1;
    }

    stepper->setStepResolution(motor, resolution);

    delete stepper;

    return 0;
}

static struct {
    struct arg_int *id;
    struct arg_int *motor;
    struct arg_dbl *acc;
    struct arg_end *end;
} setAccelerationArgs;

static int setAccelerationFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, setAccelerationArgs);

    uint16_t id = setAccelerationArgs.id->ival[0];
    MotorNum_t motor = (MotorNum_t)(setAccelerationArgs.motor->ival[0] - 1);
    float acc = (float)setAccelerationArgs.acc->dval[0];

    if (motor >= MOTOR_MAX_NUM) {
        ESP_LOGE(TAG, "Invalid motor number: %d. Must be between %d and %d", 
                 setAccelerationArgs.motor->ival[0], 0, MOTOR_MAX_NUM);
        return 1;
    }

    MotorStepperCmd *stepper = new MotorStepperCmd(id);
    if (stepper == nullptr) {
        ESP_LOGE(TAG, "Failed to create MotorStepperCmd instance");
        return 1;
    }

    stepper->setAcceleration(motor, acc);

    delete stepper;

    return 0;
}

static struct {
    struct arg_int *id;
    struct arg_int *motor;
    struct arg_dbl *dec;
    struct arg_end *end;
} setDecelerationArgs;

static int setDecelerationFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, setDecelerationArgs);

    uint16_t id = setDecelerationArgs.id->ival[0];
    MotorNum_t motor = (MotorNum_t)(setDecelerationArgs.motor->ival[0] - 1);
    float dec = (float)setDecelerationArgs.dec->dval[0];

    if (motor >= MOTOR_MAX_NUM) {
        ESP_LOGE(TAG, "Invalid motor number: %d. Must be between %d and %d", 
                 setDecelerationArgs.motor->ival[0], 0, MOTOR_MAX_NUM);
        return 1;
    }

    MotorStepperCmd *stepper = new MotorStepperCmd(id);
    if (stepper == nullptr) {
        ESP_LOGE(TAG, "Failed to create MotorStepperCmd instance");
        return 1;
    }

    stepper->setDeceleration(motor, dec);

    delete stepper;

    return 0;
}

static struct {
    struct arg_int *id;
    struct arg_int *motor;
    struct arg_dbl *speed;
    struct arg_end *end;
} setMaxSpeedArgs;

static int setMaxSpeedFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, setMaxSpeedArgs);

    uint16_t id = setMaxSpeedArgs.id->ival[0];
    MotorNum_t motor = (MotorNum_t)(setMaxSpeedArgs.motor->ival[0] - 1);
    float speed = (float)setMaxSpeedArgs.speed->dval[0];

    if (motor >= MOTOR_MAX_NUM) {
        ESP_LOGE(TAG, "Invalid motor number: %d. Must be between %d and %d", 
                 setMaxSpeedArgs.motor->ival[0], 0, MOTOR_MAX_NUM);
        return 1;
    }

    MotorStepperCmd *stepper = new MotorStepperCmd(id);
    if (stepper == nullptr) {
        ESP_LOGE(TAG, "Failed to create MotorStepperCmd instance");
        return 1;
    }

    stepper->setMaxSpeed(motor, speed);

    delete stepper;

    return 0;
}

static struct {
    struct arg_int *id;
    struct arg_int *motor;
    struct arg_dbl *speed;
    struct arg_end *end;
} setMinSpeedArgs;

static int setMinSpeedFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, setMinSpeedArgs);

    uint16_t id = setMinSpeedArgs.id->ival[0];
    MotorNum_t motor = (MotorNum_t)(setMinSpeedArgs.motor->ival[0] - 1);
    float speed = (float)setMinSpeedArgs.speed->dval[0];

    if (motor >= MOTOR_MAX_NUM) {
        ESP_LOGE(TAG, "Invalid motor number: %d. Must be between %d and %d", 
                 setMinSpeedArgs.motor->ival[0], 0, MOTOR_MAX_NUM);
        return 1;
    }

    MotorStepperCmd *stepper = new MotorStepperCmd(id);
    if (stepper == nullptr) {
        ESP_LOGE(TAG, "Failed to create MotorStepperCmd instance");
        return 1;
    }

    stepper->setMinSpeed(motor, speed);

    delete stepper;

    return 0;
}

static struct {
    struct arg_int *id;
    struct arg_int *motor;
    struct arg_dbl *speed;
    struct arg_end *end;
} setFullStepSpeedArgs;

static int setFullStepSpeedFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, setFullStepSpeedArgs);

    uint16_t id = setFullStepSpeedArgs.id->ival[0];
    MotorNum_t motor = (MotorNum_t)(setFullStepSpeedArgs.motor->ival[0] - 1);
    float speed = (float)setFullStepSpeedArgs.speed->dval[0];

    if  (motor >= MOTOR_MAX_NUM) {
        ESP_LOGE(TAG, "Invalid motor number: %d. Must be between %d and %d", 
                 setFullStepSpeedArgs.motor->ival[0], 0, MOTOR_MAX_NUM);
        return 1;
    }

    MotorStepperCmd *stepper = new MotorStepperCmd(id);
    if (stepper == nullptr) {
        ESP_LOGE(TAG, "Failed to create MotorStepperCmd instance");
        return 1;
    }

    stepper->setFullStepSpeed(motor, speed);

    delete stepper;

    return 0;
}

static struct {
    struct arg_int *id;
    struct arg_int *motor;
    struct arg_end *end;
} getPositionArgs;

static int getPositionFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, getPositionArgs);

    uint16_t id = getPositionArgs.id->ival[0];
    MotorNum_t motor = (MotorNum_t)(getPositionArgs.motor->ival[0] - 1);

    if (motor >= MOTOR_MAX_NUM) {
        ESP_LOGE(TAG, "Invalid motor number: %d. Must be between %d and %d", 
                 getPositionArgs.motor->ival[0], 0, MOTOR_MAX_NUM);
        return 1;
    }

    MotorStepperCmd *stepper = new MotorStepperCmd(id);
    if (stepper == nullptr) {
        ESP_LOGE(TAG, "Failed to create MotorStepperCmd instance");
        return 1;
    }

    int32_t position = stepper->getPosition(motor);
    printf("%d\n", position);

    delete stepper;

    return 0;
}

static struct {
    struct arg_int *id;
    struct arg_int *motor;
    struct arg_end *end;
} getSpeedArgs;

static int getSpeedFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, getSpeedArgs);

    uint16_t id = getSpeedArgs.id->ival[0];
    MotorNum_t motor = (MotorNum_t)(getSpeedArgs.motor->ival[0] - 1);

    if (motor >= MOTOR_MAX_NUM) {
        ESP_LOGE(TAG, "Invalid motor number: %d. Must be between %d and %d", 
                 getSpeedArgs.motor->ival[0], 0, MOTOR_MAX_NUM);
        return 1;
    }

    MotorStepperCmd *stepper = new MotorStepperCmd(id);
    if (stepper == nullptr) {
        ESP_LOGE(TAG, "Failed to create MotorStepperCmd instance");
        return 1;
    }

    float speed = stepper->getSpeed(motor);
    printf("%f\n", speed);

    delete stepper;

    return 0;
}

static struct {
    struct arg_int *id;
    struct arg_int *motor;
    struct arg_end *end;
} resetHomePositionArgs;

static int resetHomePositionFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, resetHomePositionArgs);

    uint16_t id = resetHomePositionArgs.id->ival[0];
    MotorNum_t motor = (MotorNum_t)(resetHomePositionArgs.motor->ival[0] - 1);

    if (motor >= MOTOR_MAX_NUM) {
        ESP_LOGE(TAG, "Invalid motor number: %d. Must be between %d and %d", 
                 resetHomePositionArgs.motor->ival[0], 0, MOTOR_MAX_NUM);
        return 1;
    }

    MotorStepperCmd *stepper = new MotorStepperCmd(id);
    if (stepper == nullptr) {
        ESP_LOGE(TAG, "Failed to create MotorStepperCmd instance");
        return 1;
    }

    stepper->resetHomePosition(motor);

    delete stepper;

    return 0;
}

static struct {
    struct arg_int *id;
    struct arg_int *motor;
    struct arg_int *position;
    struct arg_end *end;
} setPositionArgs;

static int setPositionFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, setPositionArgs);

    uint16_t id = setPositionArgs.id->ival[0];
    MotorNum_t motor = (MotorNum_t)(setPositionArgs.motor->ival[0] - 1);
    int32_t position = setPositionArgs.position->ival[0];

    if (motor >= MOTOR_MAX_NUM) {
        ESP_LOGE(TAG, "Invalid motor number: %d. Must be between %d and %d", 
                 setPositionArgs.motor->ival[0], 0, MOTOR_MAX_NUM);
        return 1;
    }

    MotorStepperCmd *stepper = new MotorStepperCmd(id);
    if (stepper == nullptr) {
        ESP_LOGE(TAG, "Failed to create MotorStepperCmd instance");
        return 1;
    }

    stepper->setPosition(motor, position);

    delete stepper;

    return 0;
}

static struct {
    struct arg_int *id;
    struct arg_int *motor;
    struct arg_str *mode;
    struct arg_end *end;
} stopArgs;

static int stopFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, stopArgs);

    uint16_t id = stopArgs.id->ival[0];
    MotorNum_t motor = (MotorNum_t)(stopArgs.motor->ival[0] - 1);
    MotorStopMode_t stopMode = SOFT_HIZ; // Default mode

    if (motor >= MOTOR_MAX_NUM) {
        ESP_LOGE(TAG, "Invalid motor number: %d. Must be between %d and %d", 
                 stopArgs.motor->ival[0], 0, MOTOR_MAX_NUM);
        return 1;
    }

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
        }
    }

    MotorStepperCmd *stepper = new MotorStepperCmd(id);
    if (stepper == nullptr) {
        ESP_LOGE(TAG, "Failed to create MotorStepperCmd instance");
        return 1;
    }

    stepper->stop(motor, stopMode);

    delete stepper;

    return 0;
}

static struct {
    struct arg_int *id;
    struct arg_int *motor;
    struct arg_int *position;
    struct arg_lit *microstep;
    struct arg_end *end;
} moveAbsoluteArgs;

static int moveAbsoluteFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, moveAbsoluteArgs);

    uint16_t id = moveAbsoluteArgs.id->ival[0];
    MotorNum_t motor = (MotorNum_t)(moveAbsoluteArgs.motor->ival[0] - 1);
    uint32_t position = (uint32_t)moveAbsoluteArgs.position->ival[0];
    bool microStep = moveAbsoluteArgs.microstep->count > 0;

    if (motor >= MOTOR_MAX_NUM) {
        ESP_LOGE(TAG, "Invalid motor number: %d. Must be between %d and %d", 
                 moveAbsoluteArgs.motor->ival[0], 0, MOTOR_MAX_NUM);
        return 1;
    }

    MotorStepperCmd *stepper = new MotorStepperCmd(id);
    if (stepper == nullptr) {
        ESP_LOGE(TAG, "Failed to create MotorStepperCmd instance");
        return 1;
    }

    stepper->moveAbsolute(motor, position, microStep);

    delete stepper;

    return 0;
}

static struct {
    struct arg_int *id;
    struct arg_int *motor;
    struct arg_int *position;
    struct arg_lit *microstep;
    struct arg_end *end;
} moveRelativeArgs;

static int moveRelativeFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, moveRelativeArgs);

    uint16_t id = moveRelativeArgs.id->ival[0];
    MotorNum_t motor = (MotorNum_t)(moveRelativeArgs.motor->ival[0] - 1);
    int32_t position = moveRelativeArgs.position->ival[0];
    bool microStep = moveRelativeArgs.microstep->count > 0;

    if (motor >= MOTOR_MAX_NUM) {
        ESP_LOGE(TAG, "Invalid motor number: %d. Must be between %d and %d", 
                 moveRelativeArgs.motor->ival[0], 0, MOTOR_MAX_NUM);
        return 1;
    }

    MotorStepperCmd *stepper = new MotorStepperCmd(id);
    if (stepper == nullptr) {
        ESP_LOGE(TAG, "Failed to create MotorStepperCmd instance");
        return 1;
    }

    stepper->moveRelative(motor, position, microStep);

    delete stepper;

    return 0;
}

static struct {
    struct arg_int *id;
    struct arg_int *motor;
    struct arg_str *direction;
    struct arg_dbl *speed;
    struct arg_end *end;
} runArgs;

static int runFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, runArgs);

    uint16_t id = runArgs.id->ival[0];
    MotorNum_t motor = (MotorNum_t)(runArgs.motor->ival[0] - 1);
    float speed = (float)runArgs.speed->dval[0];
    MotorDirection_t direction = FORWARD; // Default direction

    if (motor >= MOTOR_MAX_NUM) {
        ESP_LOGE(TAG, "Invalid motor number: %d. Must be between %d and %d", 
                 runArgs.motor->ival[0], 0, MOTOR_MAX_NUM);
        return 1;
    }

    if (runArgs.direction->count > 0) {
        const char *dir_str = runArgs.direction->sval[0];
        if (strcmp(dir_str, "forward") == 0 || strcmp(dir_str, "fwd") == 0) {
            direction = FORWARD;
        } else if (strcmp(dir_str, "reverse") == 0 || strcmp(dir_str, "rev") == 0) {
            direction = REVERSE;
        }
    }

    MotorStepperCmd *stepper = new MotorStepperCmd(id);
    if (stepper == nullptr) {
        ESP_LOGE(TAG, "Failed to create MotorStepperCmd instance");
        return 1;
    }

    stepper->run(motor, direction, speed);

    delete stepper;

    return 0;
}

static struct {
    struct arg_int *id;
    struct arg_int *motor;
    struct arg_dbl *speed;
    struct arg_end *end;
} homingArgs;

static int homingFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, homingArgs);

    uint16_t id = homingArgs.id->ival[0];
    MotorNum_t motor = (MotorNum_t)(homingArgs.motor->ival[0] - 1);
    float speed = (float)homingArgs.speed->dval[0];

    if (motor >= MOTOR_MAX_NUM) {
        ESP_LOGE(TAG, "Invalid motor number: %d. Must be between %d and %d", 
                 homingArgs.motor->ival[0], 0, MOTOR_MAX_NUM);
        return 1;
    }

    MotorStepperCmd *stepper = new MotorStepperCmd(id);
    if (stepper == nullptr) {
        ESP_LOGE(TAG, "Failed to create MotorStepperCmd instance");
        return 1;
    }

    stepper->homing(motor, speed);

    delete stepper;

    return 0;
}

static struct {
    struct arg_int *id;
    struct arg_end *end;
} getSupplyVoltageArgs;

static int getSupplyVoltageFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, getSupplyVoltageArgs);

    uint16_t id = getSupplyVoltageArgs.id->ival[0];

    MotorStepperCmd *stepper = new MotorStepperCmd(id);
    if (stepper == nullptr) {
        ESP_LOGE(TAG, "Failed to create MotorStepperCmd instance");
        return 1;
    }

    float voltage = stepper->getSupplyVoltage();
    printf("%f\n", voltage);

    delete stepper;

    return 0;
}

void MotorStepperCmd::_registerCLI(void)
{
    /* Advanced parameters command */
    advancedParamArgs.id    = arg_int1("i", "id", "<id>", "ModuleControl ID");
    advancedParamArgs.cmd   = arg_str1(NULL, NULL, "<cmd>", "Command: get/set");
    advancedParamArgs.motor = arg_int1(NULL, NULL, "<motor>", "Motor number");
    advancedParamArgs.param = arg_str0(NULL, NULL, "<param>", "Parameter to set/get");
    advancedParamArgs.value = arg_rex0(NULL, NULL, "^-?([0-9]+\\.?[0-9]*|\\.[0-9]+)$", "<value>",
                                       REG_EXTENDED, "Value to set (integer or float)");
    advancedParamArgs.end   = arg_end(5);
    const esp_console_cmd_t setCmd = {.command  = "stepper-advanced-param",
                                      .help     = "Get/set stepper motor advanced parameters",
                                      .hint     = NULL,
                                      .func     = &advancedParamFunc,
                                      .argtable = &advancedParamArgs};
    esp_console_cmd_register(&setCmd);

    /* Attach limit switch command */
    attachLimitSwitchArgs.id    = arg_int1("i", "id", "<id>", "ModuleControl ID");
    attachLimitSwitchArgs.motor = arg_int1("m", "motor", "<motor>", "Motor number");
    attachLimitSwitchArgs.din   = arg_int1("d", "din", "<din>", "Digital input number");
    attachLimitSwitchArgs.logic = arg_str0("l", "logic", "<logic>", "Logic level (high/low)");
    attachLimitSwitchArgs.end   = arg_end(4);
    const esp_console_cmd_t attachCmd = {
        .command  = "stepper-attach-limit-switch",
        .help     = "Attach a limit switch to the specified motor",
        .hint     = NULL,
        .func     = &attachLimitSwitchFunc,
        .argtable = &attachLimitSwitchArgs
    };
    esp_console_cmd_register(&attachCmd);

    /* Detach limit switch command */
    detachLimitSwitchArgs.id    = arg_int1("i", "id", "<id>", "ModuleControl ID");
    detachLimitSwitchArgs.motor = arg_int1("m", "motor", "<motor>", "Motor number");
    detachLimitSwitchArgs.din   = arg_int1("d", "din", "<din>", "Digital input number");
    detachLimitSwitchArgs.end   = arg_end(3);
    const esp_console_cmd_t detachCmd = {
        .command  = "stepper-detach-limit-switch",
        .help     = "Detach a limit switch from the specified motor",
        .hint     = NULL,
        .func     = &detachLimitSwitchFunc,
        .argtable = &detachLimitSwitchArgs
    };
    esp_console_cmd_register(&detachCmd);

    /* Set step resolution command */
    setStepResolutionArgs.id         = arg_int1("i", "id", "<id>", "ModuleControl ID");
    setStepResolutionArgs.motor      = arg_int1("m", "motor", "<motor>", "Motor number");
    setStepResolutionArgs.resolution = arg_int1("r", "resolution", "<resolution>",
                                                "Step resolution (0: 1/1, 1: 1/2, 2: 1/4, "
                                                "3: 1/8, 4: 1/16, 5: 1/32, 6: 1/64, "
                                                "7: 1/128)");
    setStepResolutionArgs.end        = arg_end(3);
    const esp_console_cmd_t setResCmd = {
        .command  = "stepper-set-step-resolution",
        .help     = "Set step resolution for the specified motor",
        .hint     = NULL,
        .func     = &setStepResolutionFunc,
        .argtable = &setStepResolutionArgs
    };
    esp_console_cmd_register(&setResCmd);

    /* Set acceleration command */
    setAccelerationArgs.id    = arg_int1("i", "id", "<id>", "ModuleControl ID");
    setAccelerationArgs.motor = arg_int1("m", "motor", "<motor>", "Motor number");
    setAccelerationArgs.acc   = arg_dbl1("a", "acceleration", "<acc>", "Acceleration (float)");
    setAccelerationArgs.end   = arg_end(3);
    const esp_console_cmd_t setAccCmd = {
        .command  = "stepper-set-acceleration",
        .help     = "Set motor acceleration",
        .hint     = NULL,
        .func     = &setAccelerationFunc,
        .argtable = &setAccelerationArgs
    };
    esp_console_cmd_register(&setAccCmd);

    /* Set deceleration command */
    setDecelerationArgs.id    = arg_int1("i", "id", "<id>", "ModuleControl ID");
    setDecelerationArgs.motor = arg_int1("m", "motor", "<motor>", "Motor number");
    setDecelerationArgs.dec   = arg_dbl1("d", "deceleration", "<dec>", "Deceleration (float)");
    setDecelerationArgs.end   = arg_end(3);
    const esp_console_cmd_t setDecCmd = {
        .command  = "stepper-set-deceleration",
        .help     = "Set motor deceleration",
        .hint     = NULL,
        .func     = &setDecelerationFunc,
        .argtable = &setDecelerationArgs
    };
    esp_console_cmd_register(&setDecCmd);

    /* Set max speed command */
    setMaxSpeedArgs.id    = arg_int1("i", "id", "<id>", "ModuleControl ID");
    setMaxSpeedArgs.motor = arg_int1("m", "motor", "<motor>", "Motor number");
    setMaxSpeedArgs.speed  = arg_dbl1("s", "speed", "<speed>", "Max speed (float)");
    setMaxSpeedArgs.end    = arg_end(3);
    const esp_console_cmd_t setMaxSpdCmd = {
        .command  = "stepper-set-max-speed",
        .help     = "Set motor maximum speed",
        .hint     = NULL,
        .func     = &setMaxSpeedFunc,
        .argtable = &setMaxSpeedArgs
    };
    esp_console_cmd_register(&setMaxSpdCmd);

    /* Set min speed command */
    setMinSpeedArgs.id    = arg_int1("i", "id", "<id>", "ModuleControl ID");
    setMinSpeedArgs.motor = arg_int1("m", "motor", "<motor>", "Motor number");
    setMinSpeedArgs.speed  = arg_dbl1("s", "speed", "<speed>", "Min speed (float)");
    setMinSpeedArgs.end    = arg_end(3);
    const esp_console_cmd_t setMinSpdCmd = {
        .command  = "stepper-set-min-speed",
        .help     = "Set motor minimum speed",
        .hint     = NULL,
        .func     = &setMinSpeedFunc,
        .argtable = &setMinSpeedArgs
    };
    esp_console_cmd_register(&setMinSpdCmd);

    /* Set full step speed command */
    setFullStepSpeedArgs.id    = arg_int1("i", "id", "<id>", "ModuleControl ID");
    setFullStepSpeedArgs.motor = arg_int1("m", "motor", "<motor>", "Motor number");
    setFullStepSpeedArgs.speed  = arg_dbl1("s", "speed", "<speed>", "Full step speed (float)");
    setFullStepSpeedArgs.end    = arg_end(3);
    const esp_console_cmd_t setFullSpdCmd = {
        .command  = "stepper-set-full-step-speed",
        .help     = "Set motor full step speed",
        .hint     = NULL,
        .func     = &setFullStepSpeedFunc,
        .argtable = &setFullStepSpeedArgs
    };
    esp_console_cmd_register(&setFullSpdCmd);

    /* Get position command */
    getPositionArgs.id    = arg_int1("i", "id", "<id>", "ModuleControl ID");
    getPositionArgs.motor = arg_int1("m", "motor", "<motor>", "Motor number");
    getPositionArgs.end   = arg_end(2);
    const esp_console_cmd_t getPosCmd = {
        .command  = "stepper-get-position",
        .help     = "Get motor position",
        .hint     = NULL,
        .func     = &getPositionFunc,
        .argtable = &getPositionArgs
    };
    esp_console_cmd_register(&getPosCmd);

    /* Get speed command */
    getSpeedArgs.id    = arg_int1("i", "id", "<id>", "ModuleControl ID");
    getSpeedArgs.motor = arg_int1("m", "motor", "<motor>", "Motor number");
    getSpeedArgs.end   = arg_end(2);
    const esp_console_cmd_t getSpdCmd = {
        .command  = "stepper-get-speed",
        .help     = "Get motor speed",
        .hint     = NULL,
        .func     = &getSpeedFunc,
        .argtable = &getSpeedArgs
    };
    esp_console_cmd_register(&getSpdCmd);

    /* Reset home position command */
    resetHomePositionArgs.id    = arg_int1("i", "id", "<id>", "ModuleControl ID");
    resetHomePositionArgs.motor = arg_int1("m", "motor", "<motor>", "Motor number");
    resetHomePositionArgs.end   = arg_end(2);
    const esp_console_cmd_t resetHomePosCmd = {
        .command  = "stepper-reset-home-position",
        .help     = "Reset motor home position",
        .hint     = NULL,
        .func     = &resetHomePositionFunc,
        .argtable = &resetHomePositionArgs
    };
    esp_console_cmd_register(&resetHomePosCmd);

    /* Set position command */
    setPositionArgs.id    = arg_int1("i", "id", "<id>", "ModuleControl ID");
    setPositionArgs.motor = arg_int1("m", "motor", "<motor>", "Motor number");
    setPositionArgs.position = arg_int1("p", "position", "<position>", "Position (int)");
    setPositionArgs.end   = arg_end(3);
    const esp_console_cmd_t setPosCmd = {
        .command  = "stepper-set-position",
        .help     = "Set motor position",
        .hint     = NULL,
        .func     = &setPositionFunc,
        .argtable = &setPositionArgs
    };
    esp_console_cmd_register(&setPosCmd);

    /* Stop command */
    stopArgs.id    = arg_int1("i", "id", "<id>", "ModuleControl ID");
    stopArgs.motor = arg_int1("m", "motor", "<motor>", "Motor number");
    stopArgs.mode  = arg_str0("mo", "mode", "<mode>", "Stop mode (soft-stop, hard-stop, soft-hiz, hard-hiz)");
    stopArgs.end   = arg_end(3);
    const esp_console_cmd_t stopCmd = {
        .command  = "stepper-stop",
        .help     = "Stop the motor",
        .hint     = NULL,
        .func     = &stopFunc,
        .argtable = &stopArgs
    };
    esp_console_cmd_register(&stopCmd);

    /* Move absolute command */
    moveAbsoluteArgs.id    = arg_int1("i", "id", "<id>", "ModuleControl ID");
    moveAbsoluteArgs.motor = arg_int1("m", "motor", "<motor>", "Motor number");
    moveAbsoluteArgs.position = arg_int1("p", "position", "<position>", "Position (uint)");
    moveAbsoluteArgs.microstep = arg_lit0("ms", "microstep", "Enable microstepping");
    moveAbsoluteArgs.end   = arg_end(4);
    const esp_console_cmd_t moveAbsCmd = {
        .command  = "stepper-move-absolute",
        .help     = "Move motor to absolute position",
        .hint     = NULL,
        .func     = &moveAbsoluteFunc,
        .argtable = &moveAbsoluteArgs
    };
    esp_console_cmd_register(&moveAbsCmd);

    /* Move relative command */
    moveRelativeArgs.id    = arg_int1("i", "id", "<id>", "ModuleControl ID");
    moveRelativeArgs.motor = arg_int1("m", "motor", "<motor>", "Motor number");
    moveRelativeArgs.position = arg_int1("p", "position", "<position>", "Position (int)");
    moveRelativeArgs.microstep = arg_lit0("ms", "microstep", "Enable microstepping");
    moveRelativeArgs.end   = arg_end(4);
    const esp_console_cmd_t moveRelCmd = {
        .command  = "stepper-move-relative",
        .help     = "Move motor to relative position",
        .hint     = NULL,
        .func     = &moveRelativeFunc,
        .argtable = &moveRelativeArgs
    };
    esp_console_cmd_register(&moveRelCmd);

    /* Run command */
    runArgs.id    = arg_int1("i", "id", "<id>", "ModuleControl ID");
    runArgs.motor = arg_int1("m", "motor", "<motor>", "Motor number");
    runArgs.direction = arg_str0("d", "direction", "<direction>", "Direction (forward/reverse)");
    runArgs.speed  = arg_dbl1("s", "speed", "<speed>", "Speed (float)");
    runArgs.end   = arg_end(4);
    const esp_console_cmd_t runCmd = {
        .command  = "stepper-run",
        .help     = "Run the motor",
        .hint     = NULL,
        .func     = &runFunc,
        .argtable = &runArgs
    };
    esp_console_cmd_register(&runCmd);

    /* Homing command */
    homingArgs.id    = arg_int1("i", "id", "<id>", "ModuleControl ID");
    homingArgs.motor = arg_int1("m", "motor", "<motor>", "Motor number");
    homingArgs.speed  = arg_dbl1("s", "speed", "<speed>", "Homing speed (float)");
    homingArgs.end   = arg_end(3);
    const esp_console_cmd_t homeCmd = {
        .command  = "stepper-homing",
        .help     = "Homing the motor",
        .hint     = NULL,
        .func     = &homingFunc,
        .argtable = &homingArgs
    };
    esp_console_cmd_register(&homeCmd);

    /* Get supply voltage command */
    getSupplyVoltageArgs.id    = arg_int1("i", "id", "<id>", "ModuleControl ID");
    getSupplyVoltageArgs.end   = arg_end(1);
    const esp_console_cmd_t getVoltCmd = {
        .command  = "stepper-get-supply-voltage",
        .help     = "Get supply voltage",
        .hint     = NULL,
        .func     = &getSupplyVoltageFunc,
        .argtable = &getSupplyVoltageArgs
    };
    esp_console_cmd_register(&getVoltCmd);
}

#endif