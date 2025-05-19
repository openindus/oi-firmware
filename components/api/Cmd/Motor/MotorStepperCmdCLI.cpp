/**
 * @file MotorStepperCmdCLI.cpp
 * @brief Motor stepper command CLI
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "MotorStepperCmd.h"

#if defined(MODULE_MASTER)

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
    int nerrors = arg_parse(argc, argv, (void **)&advancedParamArgs);
    if (nerrors != 0) {
        arg_print_errors(stderr, advancedParamArgs.end, argv[0]);
        return 1;
    }

    uint16_t id      = advancedParamArgs.id->ival[0];
    MotorNum_t motor = (MotorNum_t)advancedParamArgs.motor->ival[0];
    AdvancedParameterInfo_s AdvancedParameterInfo_s =
        advancedParamMap.at(advancedParamArgs.param->sval[0]);

    ModuleControl *module = new ModuleControl(id);
    if (module == nullptr) {
        ESP_LOGE(TAG, "Failed to create ModuleControl instance");
        return 1;
    }

    MotorStepperCmd *stepper = new MotorStepperCmd(module);
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
    delete module;

    return 0;
}

void MotorStepperCmd::_registerCLI(void)
{
    advancedParamArgs.id    = arg_int1("i", "id", "<id>", "ModuleControl ID");
    advancedParamArgs.cmd   = arg_str1(NULL, NULL, "<cmd>", "Command: get/set");
    advancedParamArgs.motor = arg_int1(NULL, NULL, "<motor>", "Motor number");
    advancedParamArgs.param = arg_str0(NULL, NULL, "<param>", "Parameter to set/get");
    advancedParamArgs.value = arg_rex0(NULL, NULL, "^-?([0-9]+\\.?[0-9]*|\\.[0-9]+)$", "<value>",
                                       REG_EXTENDED, "Value to set (integer or float)");
    advancedParamArgs.end   = arg_end(5);

    const esp_console_cmd_t setCmd = {.command  = "advanced-param",
                                      .help     = "Get/set stepper motor advanced parameters",
                                      .hint     = NULL,
                                      .func     = &advancedParamFunc,
                                      .argtable = &advancedParamArgs};
    esp_console_cmd_register(&setCmd);
}

#endif