/**
 * @file MotorStepperCmdCLI.cpp
 * @brief Motor stepper command CLI
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "MotorStepperCmd.h"

#if defined(CONFIG_MODULE_MASTER)

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
    {"ocd-th", {OCD_TH, TYPE_FLOAT}},
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
    {"stall-th", {VM_STALL_TH, TYPE_FLOAT}}
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
    struct arg_int *motor;
    struct arg_str *cmd;
    struct arg_str *param;
    struct arg_dbl *value;
    struct arg_end *end;
} advancedParamArgs;

static int advancedParamFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, advancedParamArgs);

    uint16_t id = advancedParamArgs.id->ival[0];
    MotorNum_t motor = (MotorNum_t)(advancedParamArgs.motor->ival[0] - 1);

    if (motor >= MOTOR_MAX_NUM) {
        ESP_LOGE(TAG, "Invalid motor number: %d. Must be between %d and %d", 
                 advancedParamArgs.motor->ival[0], 0, MOTOR_MAX_NUM);
        return 1;
    }

    MotorStepperCmd *stepper = new MotorStepperCmd(id);
    if (stepper == nullptr) {
        ESP_LOGE(TAG, "Failed to create MotorStepperCmd instance");
        return 1;
    }

    if (strcmp(advancedParamArgs.cmd->sval[0], "get") == 0) 
    {
        AdvancedParameterInfo_s AdvancedParameterInfo_s = advancedParamMap.at(advancedParamArgs.param->sval[0]);
        if (AdvancedParameterInfo_s.type == TYPE_FLOAT) {
            float value = 0.0f;
            stepper->getAdvancedParam(motor, AdvancedParameterInfo_s.param, &value);
            printf("%f\n", value);
        } else {
            int value = 0;
            stepper->getAdvancedParam(motor, AdvancedParameterInfo_s.param, &value);
            printf("%d\n", value);
        }
    } 
    else if (strcmp(advancedParamArgs.cmd->sval[0], "set") == 0) 
    {
        AdvancedParameterInfo_s AdvancedParameterInfo_s = advancedParamMap.at(advancedParamArgs.param->sval[0]);
        if (AdvancedParameterInfo_s.type == TYPE_FLOAT) {
            float value = static_cast<float>(advancedParamArgs.value->dval[0]);
            stepper->setAdvancedParam(motor, AdvancedParameterInfo_s.param, &value);
        } else {
            int value = static_cast<int>(advancedParamArgs.value->dval[0]);
            stepper->setAdvancedParam(motor, AdvancedParameterInfo_s.param, &value);
        }
    } 
    else if (strcmp(advancedParamArgs.cmd->sval[0], "reset") == 0) 
    {
        stepper->resetAllAdvancedParam(motor);
    } 
    else {
        ESP_LOGE(TAG, "Unknown command: %s", advancedParamArgs.cmd->sval[0]);
        delete stepper;
        return 1;
    }

    delete stepper;

    return 0;
}

static struct {
    struct arg_int *id;
    struct arg_int *motor;
    struct arg_int *din;
    struct arg_int *logic;
    struct arg_end *end;
} attachLimitSwitchArgs;

static int attachLimitSwitchFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, attachLimitSwitchArgs);

    uint16_t id = attachLimitSwitchArgs.id->ival[0];
    MotorNum_t motor = (MotorNum_t)(attachLimitSwitchArgs.motor->ival[0] - 1);
    DIn_Num_t din = (DIn_Num_t)(attachLimitSwitchArgs.din->ival[0] - 1);

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

    MotorStepperCmd *stepper = new MotorStepperCmd(id);
    if (stepper == nullptr) {
        ESP_LOGE(TAG, "Failed to create MotorStepperCmd instance");
        return 1;
    }

    if (attachLimitSwitchArgs.logic->count > 0) {
        Logic_t logic = (Logic_t)(attachLimitSwitchArgs.logic->ival[0]);
        stepper->attachLimitSwitch(motor, din, logic);
    } else {
        stepper->attachLimitSwitch(motor, din);
    }

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

/** 'set-speed' */

static struct {
    struct arg_int *id;
    struct arg_int *motor;
    struct arg_int *speed;
    struct arg_end *end;
} setMaxSpeedArgs;

static int setMaxSpeedFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, setMaxSpeedArgs);

    uint16_t id = setStepResolutionArgs.id->ival[0];
    MotorNum_t motor = (MotorNum_t)(setMaxSpeedArgs.motor->ival[0] - 1);
    float speed = (float)(setMaxSpeedArgs.speed->ival[0]);

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
    struct arg_int *acc;
    struct arg_end *end;
} setAccelerationArgs;

static int setAccelerationFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, setAccelerationArgs);

    uint16_t id = setAccelerationArgs.id->ival[0];
    MotorNum_t motor = (MotorNum_t)(setAccelerationArgs.motor->ival[0] - 1);
    float acc = (float)setAccelerationArgs.acc->ival[0];

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
    struct arg_int *dec;
    struct arg_end *end;
} setDecelerationArgs;

static int setDecelerationFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, setDecelerationArgs);

    uint16_t id = setDecelerationArgs.id->ival[0];
    MotorNum_t motor = (MotorNum_t)(setDecelerationArgs.motor->ival[0] - 1);
    float dec = (float)setDecelerationArgs.dec->ival[0];

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
    struct arg_int *speed;
    struct arg_end *end;
} setMinSpeedArgs;

static int setMinSpeedFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, setMinSpeedArgs);

    uint16_t id = setMinSpeedArgs.id->ival[0];
    MotorNum_t motor = (MotorNum_t)(setMinSpeedArgs.motor->ival[0] - 1);
    float speed = (float)setMinSpeedArgs.speed->ival[0];

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
    struct arg_int *speed;
    struct arg_end *end;
} setFullStepSpeedArgs;

static int setFullStepSpeedFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, setFullStepSpeedArgs);

    uint16_t id = setFullStepSpeedArgs.id->ival[0];
    MotorNum_t motor = (MotorNum_t)(setFullStepSpeedArgs.motor->ival[0] - 1);
    float speed = (float)setFullStepSpeedArgs.speed->ival[0];

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
    printf("%" PRId32 "\n", position);

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
    struct arg_lit *raw;
    struct arg_end *end;
} getStatusArgs;

static int getStatusFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, getStatusArgs);

    uint16_t id = getStatusArgs.id->ival[0];
    MotorNum_t motor = (MotorNum_t)(getStatusArgs.motor->ival[0] - 1);

    if (motor >= MOTOR_MAX_NUM) {
        ESP_LOGE(TAG, "Invalid motor number: %d. Must be between %d and %d", 
                 getStatusArgs.motor->ival[0], 0, MOTOR_MAX_NUM);
        return 1;
    }

    MotorStepperCmd *stepper = new MotorStepperCmd(id);
    if (stepper == nullptr) {
        ESP_LOGE(TAG, "Failed to create MotorStepperCmd instance");
        return 1;
    }

    MotorStepperStatus_t status = stepper->getStatus(motor);
    
    delete stepper;

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

static struct {
    struct arg_int *id;
    struct arg_int *motor;
    struct arg_end *end;
} clearStatusArgs;

static int clearStatusFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, clearStatusArgs);

    uint16_t id = clearStatusArgs.id->ival[0];
    MotorNum_t motor = (MotorNum_t)(clearStatusArgs.motor->ival[0] - 1);

    if (motor >= MOTOR_MAX_NUM) {
        ESP_LOGE(TAG, "Invalid motor number: %d. Must be between %d and %d", 
                 clearStatusArgs.motor->ival[0], 0, MOTOR_MAX_NUM);
        return 1;
    }

    MotorStepperCmd *stepper = new MotorStepperCmd(id);
    if (stepper == nullptr) {
        ESP_LOGE(TAG, "Failed to create MotorStepperCmd instance");
        return 1;
    }

    stepper->clearStatus(motor);

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
    struct arg_end *end;
} moveAbsoluteArgs;

static int moveAbsoluteFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, moveAbsoluteArgs);

    uint16_t id = moveAbsoluteArgs.id->ival[0];
    MotorNum_t motor = (MotorNum_t)(moveAbsoluteArgs.motor->ival[0] - 1);
    uint32_t position = (uint32_t)moveAbsoluteArgs.position->ival[0];

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

    stepper->moveAbsolute(motor, position);

    delete stepper;

    return 0;
}

static struct {
    struct arg_int *id;
    struct arg_int *motor;
    struct arg_int *position;
    struct arg_int *dir;
    struct arg_end *end;
} moveRelativeArgs;

static int moveRelativeFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, moveRelativeArgs);

    uint16_t id = moveRelativeArgs.id->ival[0];
    MotorNum_t motor = (MotorNum_t)(moveRelativeArgs.motor->ival[0] - 1);
    int32_t position = moveRelativeArgs.position->ival[0];

    if (motor >= MOTOR_MAX_NUM) {
        ESP_LOGE(TAG, "Invalid motor number: %d. Must be between %d and %d", 
                 moveRelativeArgs.motor->ival[0], 0, MOTOR_MAX_NUM);
        return 1;
    }

    // Handle optional direction argument
    if (moveRelativeArgs.dir->count > 0) {
        int direction = moveRelativeArgs.dir->ival[0];
        if (direction == 0) {
            // Reverse direction: make position negative
            position = -abs(position);
        } else {
            // Forward direction: make position positive
            position = abs(position);
        }
    }

    MotorStepperCmd *stepper = new MotorStepperCmd(id);
    if (stepper == nullptr) {
        ESP_LOGE(TAG, "Failed to create MotorStepperCmd instance");
        return 1;
    }

    stepper->moveRelative(motor, position);

    delete stepper;

    return 0;
}

static struct {
    struct arg_int *id;
    struct arg_int *motor;
    struct arg_int *direction;
    struct arg_int *speed;
    struct arg_end *end;
} runArgs;

static int runFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, runArgs);

    uint16_t id = runArgs.id->ival[0];
    MotorNum_t motor = (MotorNum_t)(runArgs.motor->ival[0] - 1);
    MotorDirection_t direction = (MotorDirection_t)(runArgs.direction->ival[0]);
    float speed = (float)runArgs.speed->ival[0];

    if (motor >= MOTOR_MAX_NUM) {
        ESP_LOGE(TAG, "Invalid motor number: %d. Must be between %d and %d", 
                 runArgs.motor->ival[0], 0, MOTOR_MAX_NUM);
        return 1;
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
    struct arg_int *speed;
    struct arg_end *end;
} homingArgs;

static int homingFunc(int argc, char **argv)
{
    PARSE_ARGS_OR_RETURN(argc, argv, homingArgs);

    uint16_t id = homingArgs.id->ival[0];
    MotorNum_t motor = (MotorNum_t)(homingArgs.motor->ival[0] - 1);
    float speed = (float)homingArgs.speed->ival[0];

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
    advancedParamArgs.motor = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    advancedParamArgs.cmd   = arg_str1(NULL, NULL, "COMMAND", "<get/set/reset>");
    advancedParamArgs.param = arg_str0(NULL, NULL, "PARAMETERS", "Parameter to set/get");
    advancedParamArgs.value = arg_dbl0(NULL, NULL, "VALUE", "Arguments");
    advancedParamArgs.end   = arg_end(20);

    const esp_console_cmd_t setCmd = {.command  = "stepper-advanced-param",
                                      .help     = "Get/set stepper motor advanced parameters",
                                      .hint     = "MOTOR <get/set/reset> PARAMETER ARGUMENT",
                                      .func     = &advancedParamFunc,
                                      .argtable = &advancedParamArgs,
                                      .func_w_context = NULL,
                                      .context = NULL};
    esp_console_cmd_register(&setCmd);

    /* Attach limit switch command */
    attachLimitSwitchArgs.id    = arg_int1("i", "id", "<id>", "ModuleControl ID");
    attachLimitSwitchArgs.motor = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    attachLimitSwitchArgs.din   = arg_int1(NULL, NULL, "DIN", "[1-4]");
    attachLimitSwitchArgs.logic = arg_int0(NULL, NULL, "logic", "[0: Active low, 1: Active high]");
    attachLimitSwitchArgs.end   = arg_end(4);
    const esp_console_cmd_t attachCmd = {
        .command  = "stepper-attach-limit-switch",
        .help     = "Attach a limit switch to the specified motor",
        .hint     = NULL,
        .func     = &attachLimitSwitchFunc,
        .argtable = &attachLimitSwitchArgs,
        .func_w_context = NULL,
        .context = NULL
    };
    esp_console_cmd_register(&attachCmd);

    /* Detach limit switch command */
    detachLimitSwitchArgs.id    = arg_int1("i", "id", "<id>", "ModuleControl ID");
    detachLimitSwitchArgs.motor = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    detachLimitSwitchArgs.din   = arg_int1(NULL, NULL, "DIN", "[1-10]");
    detachLimitSwitchArgs.end   = arg_end(3);
    const esp_console_cmd_t detachCmd = {
        .command  = "stepper-detach-limit-switch",
        .help     = "Detach a limit switch from the specified motor",
        .hint     = NULL,
        .func     = &detachLimitSwitchFunc,
        .argtable = &detachLimitSwitchArgs,
        .func_w_context = NULL,
        .context = NULL
    };
    esp_console_cmd_register(&detachCmd);

    /* Set step resolution command */
    setStepResolutionArgs.id         = arg_int1("i", "id", "<id>", "ModuleControl ID");
    setStepResolutionArgs.motor      = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    setStepResolutionArgs.resolution = arg_int1(NULL, NULL, "RESOLUTION", "[0: Full step, 1: Half step, 2: 1/4 step, 3: 1/8 step, 4: 1/16 step, 5: 1/32 step, 6: 1/64 step, 7: 1/128 step]");
    setStepResolutionArgs.end        = arg_end(3);

    const esp_console_cmd_t setResCmd = {
        .command  = "stepper-step-resolution",
        .help     = "Set step resolution for the specified motor",
        .hint     = NULL,
        .func     = &setStepResolutionFunc,
        .argtable = &setStepResolutionArgs,
        .func_w_context = NULL,
        .context = NULL
    };
    esp_console_cmd_register(&setResCmd);

    /* Set max speed */
    setMaxSpeedArgs.id    = arg_int1("i", "id", "<id>", "ModuleControl ID");
    setMaxSpeedArgs.motor       = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    setMaxSpeedArgs.speed       = arg_int1(NULL, NULL, "SPEED", "motor max speed in step/s");
    setMaxSpeedArgs.end         = arg_end(3);

    const esp_console_cmd_t cmd = {
        .command = "stepper-set-speed",
        .help = "set motor speed",
        .hint = NULL,
        .func = &setMaxSpeedFunc,
        .argtable = &setMaxSpeedArgs,
        .func_w_context = NULL,
        .context = NULL
    };
    esp_console_cmd_register(&cmd);

    /* Set acceleration command */
    setAccelerationArgs.id    = arg_int1("i", "id", "<id>", "ModuleControl ID");
    setAccelerationArgs.motor = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    setAccelerationArgs.acc   = arg_int1(NULL, NULL, "ACCELERATION", "motor acceleration in step/s²");
    setAccelerationArgs.end   = arg_end(3);
    const esp_console_cmd_t setAccCmd = {
        .command  = "stepper-set-acceleration",
        .help     = "Set motor acceleration",
        .hint     = NULL,
        .func     = &setAccelerationFunc,
        .argtable = &setAccelerationArgs,
        .func_w_context = NULL,
        .context = NULL
    };
    esp_console_cmd_register(&setAccCmd);

    /* Set deceleration command */
    setDecelerationArgs.id    = arg_int1("i", "id", "<id>", "ModuleControl ID");
    setDecelerationArgs.motor = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    setDecelerationArgs.dec   = arg_int1(NULL, NULL, "DECELERATION", "motor deceleration speed in step/s²");
    setDecelerationArgs.end   = arg_end(3);
    const esp_console_cmd_t setDecCmd = {
        .command  = "stepper-set-deceleration",
        .help     = "Set motor deceleration",
        .hint     = NULL,
        .func     = &setDecelerationFunc,
        .argtable = &setDecelerationArgs,
        .func_w_context = NULL,
        .context = NULL
    };
    esp_console_cmd_register(&setDecCmd);

    /* Set min speed command */
    setMinSpeedArgs.id    = arg_int1("i", "id", "<id>", "ModuleControl ID");
    setMinSpeedArgs.motor = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    setMinSpeedArgs.speed = arg_int1(NULL, NULL, "MIN_SPEED", "motor min speed in step/s");
    setMinSpeedArgs.end   = arg_end(3);
    const esp_console_cmd_t setMinSpdCmd = {
        .command  = "stepper-set-min-speed",
        .help     = "Set motor minimum speed",
        .hint     = NULL,
        .func     = &setMinSpeedFunc,
        .argtable = &setMinSpeedArgs,
        .func_w_context = NULL,
        .context = NULL
    };
    esp_console_cmd_register(&setMinSpdCmd);

    /* Set full step speed command */
    setFullStepSpeedArgs.id    = arg_int1("i", "id", "<id>", "ModuleControl ID");
    setFullStepSpeedArgs.motor = arg_int1("m", "motor", "<motor>", "Motor number");
    setFullStepSpeedArgs.speed = arg_int1(NULL, NULL, "FULL_STEP_SPEED", "motor full step speed in step/s");
    setFullStepSpeedArgs.end   = arg_end(3);
    const esp_console_cmd_t setFullSpdCmd = {
        .command  = "stepper-set-full-step-speed",
        .help     = "Set motor full step speed",
        .hint     = NULL,
        .func     = &setFullStepSpeedFunc,
        .argtable = &setFullStepSpeedArgs,
        .func_w_context = NULL,
        .context = NULL
    };
    esp_console_cmd_register(&setFullSpdCmd);

    /* Get position command */
    getPositionArgs.id    = arg_int1("i", "id", "<id>", "ModuleControl ID");
    getPositionArgs.motor = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    getPositionArgs.end   = arg_end(2);
    const esp_console_cmd_t getPosCmd = {
        .command  = "stepper-get-position",
        .help     = "Get motor position",
        .hint     = NULL,
        .func     = &getPositionFunc,
        .argtable = &getPositionArgs,
        .func_w_context = NULL,
        .context = NULL
    };
    esp_console_cmd_register(&getPosCmd);

    /* Get speed command */
    getSpeedArgs.id    = arg_int1("i", "id", "<id>", "ModuleControl ID");
    getSpeedArgs.motor = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    getSpeedArgs.end   = arg_end(2);
    const esp_console_cmd_t getSpdCmd = {
        .command  = "stepper-get-speed",
        .help     = "Get motor speed",
        .hint     = NULL,
        .func     = &getSpeedFunc,
        .argtable = &getSpeedArgs,
        .func_w_context = NULL,
        .context = NULL
    };
    esp_console_cmd_register(&getSpdCmd);

    /* Get status command */
    getStatusArgs.id    = arg_int1("i", "id", "<id>", "ModuleControl ID");
    getStatusArgs.motor = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    getStatusArgs.raw   = arg_lit0("r", "raw", "Output raw register value as uint16_t");
    getStatusArgs.end   = arg_end(3);
    const esp_console_cmd_t getStatusCmd = {
        .command  = "stepper-get-status",
        .help     = "Get motor status information",
        .hint     = NULL,
        .func     = &getStatusFunc,
        .argtable = &getStatusArgs,
        .func_w_context = NULL,
        .context = NULL
    };
    esp_console_cmd_register(&getStatusCmd);

    /* Clear status command */
    clearStatusArgs.id    = arg_int1("i", "id", "<id>", "ModuleControl ID");
    clearStatusArgs.motor = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    clearStatusArgs.end   = arg_end(2);
    const esp_console_cmd_t clearStatusCmd = {
        .command  = "stepper-clear-status",
        .help     = "Clear motor status information",
        .hint     = NULL,
        .func     = &clearStatusFunc,
        .argtable = &clearStatusArgs,
        .func_w_context = NULL,
        .context = NULL
    };
    esp_console_cmd_register(&clearStatusCmd);

    /* Reset home position command */
    resetHomePositionArgs.id    = arg_int1("i", "id", "<id>", "ModuleControl ID");
    resetHomePositionArgs.motor = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    resetHomePositionArgs.end   = arg_end(2);
    const esp_console_cmd_t resetHomePosCmd = {
        .command  = "stepper-reset-home-position",
        .help     = "Reset motor home position",
        .hint     = NULL,
        .func     = &resetHomePositionFunc,
        .argtable = &resetHomePositionArgs,
        .func_w_context = NULL,
        .context = NULL
    };
    esp_console_cmd_register(&resetHomePosCmd);

    /* Set position command */
    setPositionArgs.id    = arg_int1("i", "id", "<id>", "ModuleControl ID");
    setPositionArgs.motor = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    setPositionArgs.position = arg_int1(NULL, NULL, "<position>", "Position (int)");
    setPositionArgs.end   = arg_end(3);
    const esp_console_cmd_t setPosCmd = {
        .command  = "stepper-set-position",
        .help     = "Set motor position",
        .hint     = NULL,
        .func     = &setPositionFunc,
        .argtable = &setPositionArgs,
        .func_w_context = NULL,
        .context = NULL
    };
    esp_console_cmd_register(&setPosCmd);

    /* Stop command */
    stopArgs.id    = arg_int1("i", "id", "<id>", "ModuleControl ID");
    stopArgs.motor = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    stopArgs.mode  = arg_str0(NULL, NULL, "MODE", "[soft-stop, hard-stop, soft-hiz, hard-hiz]");
    stopArgs.end   = arg_end(3);
    const esp_console_cmd_t stopCmd = {
        .command  = "stepper-stop",
        .help     = "Stop the motor",
        .hint     = NULL,
        .func     = &stopFunc,
        .argtable = &stopArgs,
        .func_w_context = NULL,
        .context = NULL
    };
    esp_console_cmd_register(&stopCmd);

    /* Move absolute command */
    moveAbsoluteArgs.id    = arg_int1("i", "id", "<id>", "ModuleControl ID");
    moveAbsoluteArgs.motor = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    moveAbsoluteArgs.position = arg_int1(NULL, NULL, "POSITION", "position in step");
    moveAbsoluteArgs.end   = arg_end(3);
    const esp_console_cmd_t moveAbsCmd = {
        .command  = "stepper-move-absolute",
        .help     = "Move motor to absolute position",
        .hint     = NULL,
        .func     = &moveAbsoluteFunc,
        .argtable = &moveAbsoluteArgs,
        .func_w_context = NULL,
        .context = NULL
    };
    esp_console_cmd_register(&moveAbsCmd);

    /* Move relative command */
    moveRelativeArgs.id    = arg_int1("i", "id", "<id>", "ModuleControl ID");
    moveRelativeArgs.motor = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    moveRelativeArgs.position = arg_int1(NULL, NULL, "POSITION", "position in step");
    moveRelativeArgs.dir   =arg_int0(NULL, NULL, "DIRECTION", "[1: Forward, 0: Reverse] (optional, position sign)");
    moveRelativeArgs.end   = arg_end(4);
    const esp_console_cmd_t moveRelCmd = {
        .command  = "stepper-move-relative",
        .help     = "Move motor to relative position (optional direction sign)",
        .hint     = NULL,
        .func     = &moveRelativeFunc,
        .argtable = &moveRelativeArgs,
        .func_w_context = NULL,
        .context = NULL
    };
    esp_console_cmd_register(&moveRelCmd);

    /* Run command */
    runArgs.id    = arg_int1("i", "id", "<id>", "ModuleControl ID");
    runArgs.motor = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    runArgs.direction = arg_int1(NULL, NULL, "DIRECTION", "[1: Forward, 0: Reverse]");
    runArgs.speed  = arg_int1(NULL, NULL, "SPEED", "speed in step/s");
    runArgs.end   = arg_end(4);
    const esp_console_cmd_t runCmd = {
        .command  = "stepper-run",
        .help     = "Run the motor",
        .hint     = NULL,
        .func     = &runFunc,
        .argtable = &runArgs,
        .func_w_context = NULL,
        .context = NULL
    };
    esp_console_cmd_register(&runCmd);

    /* Homing command */
    homingArgs.id    = arg_int1("i", "id", "<id>", "ModuleControl ID");
    homingArgs.motor = arg_int1(NULL, NULL, "MOTOR", "[1-2]");
    homingArgs.speed = arg_int1(NULL, NULL, "SPEED", "speed in step/s");
    homingArgs.end   = arg_end(3);
    const esp_console_cmd_t homeCmd = {
        .command  = "stepper-homing",
        .help     = "Homing the motor",
        .hint     = NULL,
        .func     = &homingFunc,
        .argtable = &homingArgs,
        .func_w_context = NULL,
        .context = NULL
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
        .argtable = &getSupplyVoltageArgs,
        .func_w_context = NULL,
        .context = NULL
    };
    esp_console_cmd_register(&getVoltCmd);
}

#endif