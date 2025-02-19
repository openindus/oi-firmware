/**
 * @file MotorStepperParam_CLI.cpp
 * @brief Command line interface - Motor Stepper
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "MotorStepperParam.h"
#include "esp_log.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"

#define REG_EXTENDED 1
#define REG_ICASE (REG_EXTENDED << 1)
#define POWERSTEP01_PARAM_ABS_POS                   "abs-pos"
#define POWERSTEP01_PARAM_EL_POS_MICROSTEP          "el-pos-microstep"
#define POWERSTEP01_PARAM_EL_POS_STEP               "el-pos-step"
#define POWERSTEP01_PARAM_MARK                      "mark"
#define POWERSTEP01_PARAM_SPEED                     "speed"
#define POWERSTEP01_PARAM_ACC                       "acc"
#define POWERSTEP01_PARAM_DEC                       "dec"
#define POWERSTEP01_PARAM_MAX_SPEED                 "max-speed"
#define POWERSTEP01_PARAM_MIN_SPEED                 "min-speed"
#define POWERSTEP01_PARAM_MIN_SPEED_LSPD_OPT        "min-speed-lspd-opt"
#define POWERSTEP01_PARAM_ADC_OUT                   "adc-out"
#define POWERSTEP01_PARAM_OCD_TH                    "ocd-th"
#define POWERSTEP01_PARAM_FS_SPD                    "fs-spd"
#define POWERSTEP01_PARAM_FS_SPD_BOOST_MODE         "fs-spd-boost-mode"
#define POWERSTEP01_PARAM_STEP_MODE_STEP_SEL        "step-mode-step-sel"
#define POWERSTEP01_PARAM_STEP_MODE_CM_VM           "step-mode-cm-vm"
#define POWERSTEP01_PARAM_STEP_MODE_SYNC_SEL        "step-mode-sync-sel"
#define POWERSTEP01_PARAM_STEP_MODE_SYNC_EN         "step-mode-sync-en"
#define POWERSTEP01_PARAM_ALARM_EN_OVERCURRENT      "alarm-en-overcurrent"
#define POWERSTEP01_PARAM_ALARM_EN_THERMAL_SHUTDOWN "alarm-en-thermal-shutdown"
#define POWERSTEP01_PARAM_ALARM_EN_THERMAL_WARNING  "alarm-en-thermal-warning"
#define POWERSTEP01_PARAM_ALARM_EN_UVLO             "alarm-en-uvlo"
#define POWERSTEP01_PARAM_ALARM_EN_ADC_UVLO         "alarm-en-adc-uvlo"
#define POWERSTEP01_PARAM_ALARM_EN_STALL_DETECT     "alarm-en-stall-detect"
#define POWERSTEP01_PARAM_ALARM_EN_SW_TURN_ON       "alarm-en-sw-turn-on"
#define POWERSTEP01_PARAM_ALARM_EN_COMMAND_ERROR    "alarm-en-command-error"

#define POWERSTEP01_PARAM_GATECFG1_TCC              "gatecfg1-tcc"
#define POWERSTEP01_PARAM_GATECFG1_IGATE            "gatecfg1-igate"
#define POWERSTEP01_PARAM_GATECFG1_TBOOST           "gatecfg1-tboost"
#define POWERSTEP01_PARAM_GATECFG1_WD_EN            "gatecfg1-wd-en"

#define POWERSTEP01_PARAM_GATECFG2_TDT              "gatecfg2-tdt"
#define POWERSTEP01_PARAM_GATECFG2_TBLANK           "gatecfg2-tblank"

#define POWERSTEP01_PARAM_CONFIG_OSC_SEL            "config-osc-sel"
#define POWERSTEP01_PARAM_CONFIG_EXT_CLK            "config-ext-clk"
#define POWERSTEP01_PARAM_CONFIG_SW_MODE            "config-sw-mode"
#define POWERSTEP01_PARAM_CONFIG_OC_SD              "config-oc-sd"
#define POWERSTEP01_PARAM_CONFIG_UVLOVAL            "config-uvloval"
#define POWERSTEP01_PARAM_CONFIG_VCCVAL             "config-vccval"

/* Voltage mode */
#ifdef CONFIG_VOLTAGE_MODE
#define POWERSTEP01_PARAM_CONFIG_EN_VSCOMP          "config-en-vscomp"
#define POWERSTEP01_PARAM_CONFIG_F_PWM_DEC          "config-f-pwm-dec"
#define POWERSTEP01_PARAM_CONFIG_F_PWM_INT          "config-f-pwm-int"

#define POWERSTEP01_PARAM_KVAL_HOLD                 "kval-hold"
#define POWERSTEP01_PARAM_KVAL_RUN                  "kval-run"
#define POWERSTEP01_PARAM_KVAL_ACC                  "kval-acc"
#define POWERSTEP01_PARAM_KVAL_DEC                  "kval-dec"

#define POWERSTEP01_PARAM_INT_SPEED                 "int-speed"
#define POWERSTEP01_PARAM_ST_SLP                    "st-slp"
#define POWERSTEP01_PARAM_FN_SLP_ACC                "fn-slp-acc"
#define POWERSTEP01_PARAM_FN_SLP_DEC                "fn-slp-dec"
#define POWERSTEP01_PARAM_K_THERM                   "k-therm"
#define POWERSTEP01_PARAM_STALL_TH                  "stall-th"

/* Current Mode*/
#else
#define POWERSTEP01_PARAM_CONFIG_EN_TQREG          "config-en-tqreg"
#define POWERSTEP01_PARAM_CONFIG_TSW               "config-tsw"
#define POWERSTEP01_PARAM_CONFIG_PRED_EN           "config-pred-en"

#define POWERSTEP01_PARAM_TVAL_HOLD                "tval-hold"
#define POWERSTEP01_PARAM_TVAL_RUN                 "tval-run"
#define POWERSTEP01_PARAM_TVAL_ACC                 "tval-acc"
#define POWERSTEP01_PARAM_TVAL_DEC                 "tval-dec"

#define POWERSTEP01_PARAM_T_FAST_FAST_STEP         "t-fast-fast-step"
#define POWERSTEP01_PARAM_T_FAST_TOFF_FAST         "t-fast-toff-fast"
#define POWERSTEP01_PARAM_TON_MIN                  "ton-min"
#define POWERSTEP01_PARAM_TOFF_MIN                 "toff-min"

#endif

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
    int errorCode = MotorStepperParam::setAllAdvancedParamPS01((MotorNum_t) device, defaultParameters);
    if (errorCode != 0) {
        printf("Error while setting default parameters\n");
    } else {
        MotorStepperParam::setNVSParam((MotorNum_t) device, defaultParameters);
    }
}

int deviceSetAdvancedParameter(uint8_t motorId, const char* parameter, double *args, int nargs) {
    uint8_t deviceId = motorId - 1;
    /* set given parameter */
    if (eq(parameter, POWERSTEP01_PARAM_ABS_POS) && nargs == 1) {
        printf("set abs pos\n");
        int32_t absPos = (int32_t) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, ABS_POS, &absPos);

    } else if (eq(parameter, POWERSTEP01_PARAM_EL_POS_MICROSTEP) && nargs == 1) {
        printf("set el pos microstep\n");
        int32_t elPosMicrostep = (int32_t) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, EL_POS_MICROSTEP, &elPosMicrostep);
    
    } else if (eq(parameter, POWERSTEP01_PARAM_EL_POS_STEP) && nargs == 1) {
        printf("set el pos step\n");
        int32_t elPosStep = (int32_t) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, EL_POS_STEP, &elPosStep);

    } else if (eq(parameter, POWERSTEP01_PARAM_MARK) && nargs == 1) {
        printf("set mark\n");
        int32_t mark = (int32_t) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, MARK, &mark);

    } else if (eq(parameter, POWERSTEP01_PARAM_ACC) && nargs == 1) {
        printf("set acc\n");
        float acc = (float) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, ACCELERATION, &acc);

    } else if (eq(parameter, POWERSTEP01_PARAM_DEC) && nargs == 1) {
        printf("set dec\n");
        float dec = (float) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, DECELERATION, &dec);

    } else if (eq(parameter, POWERSTEP01_PARAM_MAX_SPEED) && nargs == 1) {
        printf("set max speed\n");
        float maxSpeed = (float) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, MAX_SPEED, &maxSpeed);

    } else if (eq(parameter, POWERSTEP01_PARAM_MIN_SPEED) && nargs == 1) {
        printf("set min speed\n");
        float minSpeed = (float) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, MIN_SPEED, &minSpeed);

    } else if (eq(parameter, POWERSTEP01_PARAM_MIN_SPEED_LSPD_OPT) && nargs == 1) {
        printf("set min speed lsdp opt\n");
        bool minSpeedLspdOpt = (bool) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, MIN_SPEED_LSPD_OPT, &minSpeedLspdOpt);

    } else if (eq(parameter, POWERSTEP01_PARAM_FS_SPD) && nargs == 1) {
        printf("set fs spd\n");
        float fsSpd = (float) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, FS_SPD, &fsSpd);
    
    } else if (eq(parameter, POWERSTEP01_PARAM_FS_SPD_BOOST_MODE) && nargs == 1) {
        printf("set fs spd boost mode\n");
        bool fsSpdBoostMode = (bool) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, FS_SPD_BOOST_MODE, &fsSpdBoostMode);

    } else if (eq(parameter, POWERSTEP01_PARAM_OCD_TH) && nargs == 1) {
        printf("set ocd th\n");
        float ocdTh = (float) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, OCD_TH, &ocdTh);
    
    } else if (eq(parameter, POWERSTEP01_PARAM_STEP_MODE_STEP_SEL) && nargs == 1) {
        printf("set step mode step sel\n");
        uint8_t stepModeStepSel = (uint8_t) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, STEP_MODE_STEP_SEL, &stepModeStepSel);

    } else if (eq(parameter, POWERSTEP01_PARAM_STEP_MODE_CM_VM) && nargs == 1) {
        printf("set step mode cm vm\n");
        uint8_t stepModeCmVm = (uint8_t) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, STEP_MODE_CM_VM, &stepModeCmVm);

    } else if (eq(parameter, POWERSTEP01_PARAM_STEP_MODE_SYNC_SEL) && nargs == 1) {
        printf("set step mode sync sel\n");
        uint8_t stepModeSyncSel = (uint8_t) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, STEP_MODE_SYNC_SEL, &stepModeSyncSel);

    } else if (eq(parameter, POWERSTEP01_PARAM_STEP_MODE_SYNC_EN) && nargs == 1) {
        printf("set step mode sync en\n");
        uint8_t stepModeSyncEn = (uint8_t) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, STEP_MODE_SYNC_EN, &stepModeSyncEn);

    } else if (eq(parameter, POWERSTEP01_PARAM_ALARM_EN_OVERCURRENT) && nargs == 1) {
        printf("set alarm en overcurrent\n");
        bool alarmEnOvercurrent = (bool) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, ALARM_EN_OVERCURRENT, &alarmEnOvercurrent);

    } else if (eq(parameter, POWERSTEP01_PARAM_ALARM_EN_THERMAL_SHUTDOWN) && nargs == 1) {
        printf("set alarm en thermal shutdown\n");
        bool alarmEnThermalShutdown = (bool) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, ALARM_EN_THERMAL_SHUTDOWN, &alarmEnThermalShutdown);
        
    } else if (eq(parameter, POWERSTEP01_PARAM_ALARM_EN_THERMAL_WARNING) && nargs == 1) {
        printf("set alarm en thermal warning\n");
        bool alarmEnThermalWarning = (bool) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, ALARM_EN_THERMAL_WARNING, &alarmEnThermalWarning);

    } else if (eq(parameter, POWERSTEP01_PARAM_ALARM_EN_UVLO) && nargs == 1) {
        printf("set alarm en uvlo\n");
        bool alarmEnUvlo = (bool) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, ALARM_EN_UVLO, &alarmEnUvlo);

    } else if (eq(parameter, POWERSTEP01_PARAM_ALARM_EN_ADC_UVLO) && nargs == 1) {
        printf("set alarm en adc uvlo\n");
        bool alarmEnAdcUvlo = (bool) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, ALARM_EN_ADC_UVLO, &alarmEnAdcUvlo);
    
    } else if (eq(parameter, POWERSTEP01_PARAM_ALARM_EN_STALL_DETECT) && nargs == 1) {
        printf("set alarm en stall detect\n");
        bool alarmEnStallDetect = (bool) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, ALARM_EN_STALL_DETECTION, &alarmEnStallDetect);

    } else if (eq(parameter, POWERSTEP01_PARAM_ALARM_EN_SW_TURN_ON) && nargs == 1) {
        printf("set alarm en sw turn on\n");
        bool alarmEnSwTurnOn = (bool) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, ALARM_EN_SW_TURN_ON, &alarmEnSwTurnOn);

    } else if (eq(parameter, POWERSTEP01_PARAM_ALARM_EN_COMMAND_ERROR) && nargs == 1) {
        printf("set alarm en command error\n");
        bool alarmEnCommandError = (bool) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, ALARM_EN_COMMAND_ERROR, &alarmEnCommandError);

    } else if (eq(parameter, POWERSTEP01_PARAM_GATECFG1_TCC) && nargs == 1) {
        printf("set gatecfg1 tcc\n");
        uint8_t gatecfg1Tcc = (uint8_t) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, GATECFG1_TCC, &gatecfg1Tcc);

    } else if (eq(parameter, POWERSTEP01_PARAM_GATECFG1_IGATE) && nargs == 1) {
        printf("set gatecfg1 igate\n");
        uint8_t gatecfg1Igate = (uint8_t) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, GATECFG1_IGATE, &gatecfg1Igate);

    } else if (eq(parameter, POWERSTEP01_PARAM_GATECFG1_TBOOST) && nargs == 1) {
        printf("set gatecfg1 tboost\n");
        uint8_t gatecfg1Tboost = (uint8_t) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, GATECFG1_TBOOST, &gatecfg1Tboost);

    } else if (eq(parameter, POWERSTEP01_PARAM_GATECFG1_WD_EN) && nargs == 1) {
        printf("set gatecfg1 wd en\n");
        uint8_t gatecfg1WdEn = (uint8_t) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, GATECFG1_WD_EN, &gatecfg1WdEn);

    } else if (eq(parameter, POWERSTEP01_PARAM_GATECFG2_TDT) && nargs == 1) {
        printf("set gatecfg2 tdt\n");
        uint8_t gatecfg2Tdt = (uint8_t) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, GATECFG2_TDT, &gatecfg2Tdt);

    } else if (eq(parameter, POWERSTEP01_PARAM_GATECFG2_TBLANK) && nargs == 1) {
        printf("set gatecfg2 tblank\n");
        uint8_t gatecfg2Tblank = (uint8_t) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, GATECFG2_TBLANK, &gatecfg2Tblank);

    } else if (eq(parameter, POWERSTEP01_PARAM_CONFIG_OSC_SEL) && nargs == 1) {
        printf("set config osc sel\n");
        uint8_t configOscSel = (uint8_t) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, CONFIG_OSC_SEL, &configOscSel);
    
    } else if (eq(parameter, POWERSTEP01_PARAM_CONFIG_EXT_CLK) && nargs == 1) {
        printf("set config ext clk\n");
        uint8_t configExtClk = (uint8_t) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, CONFIG_EXT_CLK, &configExtClk);

    } else if (eq(parameter, POWERSTEP01_PARAM_CONFIG_SW_MODE) && nargs == 1) {
        printf("set config sw mode\n");
        uint8_t configSwMode = (uint8_t) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, CONFIG_SW_MODE, &configSwMode);

    } else if (eq(parameter, POWERSTEP01_PARAM_CONFIG_OC_SD) && nargs == 1) {
        printf("set config oc sd\n");
        uint8_t configOcSd = (uint8_t) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, CONFIG_OC_SD, &configOcSd);

    } else if (eq(parameter, POWERSTEP01_PARAM_CONFIG_UVLOVAL) && nargs == 1) {
        printf("set config uvloval\n");
        uint8_t configUvloval = (uint8_t) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, CONFIG_UVLOVAL, &configUvloval);

    } else if (eq(parameter, POWERSTEP01_PARAM_CONFIG_VCCVAL) && nargs == 1) {
        printf("set config vccval\n");
        uint8_t configVccval = (uint8_t) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, CONFIG_VCCVAL, &configVccval);

#ifdef CONFIG_VOLTAGE_MODE

    } else if (eq(parameter, POWERSTEP01_PARAM_CONFIG_EN_VSCOMP) && nargs == 1) {
        printf("set config en vscomp\n");
        uint8_t configEnVscomp = (uint8_t) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, VM_CONFIG_EN_VSCOMP, &configEnVscomp);

    } else if (eq(parameter, POWERSTEP01_PARAM_CONFIG_F_PWM_DEC) && nargs == 1) {
        printf("set config f pwm dec\n");
        uint8_t configFPwmDec = (uint8_t) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, VM_CONFIG_F_PWM_DEC, &configFPwmDec);

    } else if (eq(parameter, POWERSTEP01_PARAM_CONFIG_F_PWM_INT) && nargs == 1) {
        printf("set config f pwm int\n");
        uint8_t configFPwmInt = (uint8_t) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, VM_CONFIG_F_PWM_INT, &configFPwmInt);

    } else if (eq(parameter, POWERSTEP01_PARAM_KVAL_HOLD) && nargs == 1) {
        printf("set kval hold\n");
        float kvalHold = (float) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, VM_KVAL_HOLD, &kvalHold);

    } else if (eq(parameter, POWERSTEP01_PARAM_KVAL_RUN) && nargs == 1) {
        printf("set kval run\n");
        float kvalRun = (float) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, VM_KVAL_RUN, &kvalRun);

    } else if (eq(parameter, POWERSTEP01_PARAM_KVAL_ACC) && nargs == 1) {
        printf("set kval acc\n");
        float kvalAcc = (float) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, VM_KVAL_ACC, &kvalAcc);

    } else if (eq(parameter, POWERSTEP01_PARAM_KVAL_DEC) && nargs == 1) {
        printf("set kval dec\n");
        float kvalDec = (float) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, VM_KVAL_DEC, &kvalDec);

    } else if (eq(parameter, POWERSTEP01_PARAM_INT_SPEED) && nargs == 1) {
        printf("set int speed\n");
        float intSpeed = (float) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, VM_INT_SPEED, &intSpeed);

    } else if (eq(parameter, POWERSTEP01_PARAM_ST_SLP) && nargs == 1) {
        printf("set st slp\n");
        float stSlp = (float) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, VM_ST_SLP, &stSlp);

    } else if (eq(parameter, POWERSTEP01_PARAM_FN_SLP_ACC) && nargs == 1) {
        printf("set fn slp acc\n");
        float fnSlpAcc = (float) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, VM_FN_SLP_ACC, &fnSlpAcc);

    } else if (eq(parameter, POWERSTEP01_PARAM_FN_SLP_DEC) && nargs == 1) {
        printf("set fn slp dec\n");
        float fnSlpDec = (float) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, VM_FN_SLP_DEC, &fnSlpDec);
    
    } else if (eq(parameter, POWERSTEP01_PARAM_K_THERM) && nargs == 1) {
        printf("set k therm\n");
        float kTherm = (float) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, VM_K_THERM, &kTherm);

    } else if (eq(parameter, POWERSTEP01_PARAM_STALL_TH) && nargs == 1) {
        printf("set stall th\n");
        float stallTh = (float) args[0];
        MotorStepperParam::setAdvancedParam((MotorNum_t) deviceId, VM_STALL_TH, &stallTh);

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
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, ABS_POS, &absPos);
        printf("%d\n", absPos);
    
    } else if (eq(parameter, POWERSTEP01_PARAM_EL_POS_MICROSTEP)) {
        errorCode = 1;
        printf("not implemented\n");
    
    } else if (eq(parameter, POWERSTEP01_PARAM_EL_POS_STEP)) {
        errorCode = 1;
        printf("not implemented\n");
    
    } else if (eq(parameter, POWERSTEP01_PARAM_MARK)) {
        int32_t mark = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, MARK, &mark);
        printf("%d\n", mark);

    } else if (eq(parameter, POWERSTEP01_PARAM_SPEED)) {
        float speed = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, SPEED, &speed);
        printf("%f\n", speed);

    } else if (eq(parameter, POWERSTEP01_PARAM_ACC)) {
        float acc = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, ACCELERATION, &acc);
        printf("%f\n", acc);

    } else if (eq(parameter, POWERSTEP01_PARAM_DEC)) {
        float dec = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, DECELERATION, &dec);
        printf("%f\n", dec);

    } else if (eq(parameter, POWERSTEP01_PARAM_MAX_SPEED)) {
        float maxSpeed = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, MAX_SPEED, &maxSpeed);
        printf("%f\n", maxSpeed);

    } else if (eq(parameter, POWERSTEP01_PARAM_MIN_SPEED)) {
        float minSpeed = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, MIN_SPEED, &minSpeed);
        printf("%f\n", minSpeed);

    } else if (eq(parameter, POWERSTEP01_PARAM_MIN_SPEED_LSPD_OPT)) {
        bool minSpeedLspdOpt = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, MIN_SPEED_LSPD_OPT, &minSpeedLspdOpt);
        printf("%d\n", minSpeedLspdOpt);

    } else if (eq(parameter, POWERSTEP01_PARAM_ADC_OUT)) {
        errorCode = 1;
        printf("not implemented\n");

    } else if (eq(parameter, POWERSTEP01_PARAM_OCD_TH)) {
        errorCode = 1;
        printf("not implemented\n");

    } else if (eq(parameter, POWERSTEP01_PARAM_FS_SPD)) {
        float fsSpd = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, FS_SPD, &fsSpd);
        printf("%f\n", fsSpd);

    } else if (eq(parameter, POWERSTEP01_PARAM_FS_SPD_BOOST_MODE)) {
        bool fsSpdBoostMode = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, FS_SPD_BOOST_MODE, &fsSpdBoostMode);
        printf("%d\n", fsSpdBoostMode);

    } else if (eq(parameter, POWERSTEP01_PARAM_STEP_MODE_STEP_SEL)) {
        uint8_t stepModeStepSel = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, STEP_MODE_STEP_SEL, &stepModeStepSel);
        printf("%d\n", stepModeStepSel);

    } else if (eq(parameter, POWERSTEP01_PARAM_STEP_MODE_CM_VM)) {
        uint8_t stepModeCmVm = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, STEP_MODE_CM_VM, &stepModeCmVm);
        printf("%d\n", stepModeCmVm);

    } else if (eq(parameter, POWERSTEP01_PARAM_STEP_MODE_SYNC_SEL)) {
        uint8_t stepModeSyncSel = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, STEP_MODE_SYNC_SEL, &stepModeSyncSel);
        printf("%d\n", stepModeSyncSel);

    } else if (eq(parameter, POWERSTEP01_PARAM_STEP_MODE_SYNC_EN)) {
        uint8_t stepModeSyncEn = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, STEP_MODE_SYNC_EN, &stepModeSyncEn);
        printf("%d\n", stepModeSyncEn);

    } else if (eq(parameter, POWERSTEP01_PARAM_ALARM_EN_OVERCURRENT)) {
        uint8_t alarmEnOvercurrent = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, ALARM_EN_OVERCURRENT, &alarmEnOvercurrent);
        printf("%d\n", alarmEnOvercurrent);

    } else if (eq(parameter, POWERSTEP01_PARAM_ALARM_EN_THERMAL_SHUTDOWN)) {
        uint8_t alarmEnThermalShutdown = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, ALARM_EN_THERMAL_SHUTDOWN, &alarmEnThermalShutdown);
        printf("%d\n", alarmEnThermalShutdown);

    } else if (eq(parameter, POWERSTEP01_PARAM_ALARM_EN_THERMAL_WARNING)) {
        uint8_t alarmEnThermalWarning = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, ALARM_EN_THERMAL_WARNING, &alarmEnThermalWarning);
        printf("%d\n", alarmEnThermalWarning);

    } else if (eq(parameter, POWERSTEP01_PARAM_ALARM_EN_UVLO)) {
        uint8_t alarmEnUvlo = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, ALARM_EN_UVLO, &alarmEnUvlo);
        printf("%d\n", alarmEnUvlo);

    } else if (eq(parameter, POWERSTEP01_PARAM_ALARM_EN_ADC_UVLO)) {
        uint8_t alarmEnAdcUvlo = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, ALARM_EN_ADC_UVLO, &alarmEnAdcUvlo);
        printf("%d\n", alarmEnAdcUvlo);

    } else if (eq(parameter, POWERSTEP01_PARAM_ALARM_EN_STALL_DETECT)) {
        uint8_t alarmEnStallDetect = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, ALARM_EN_STALL_DETECTION, &alarmEnStallDetect);
        printf("%d\n", alarmEnStallDetect);

    } else if (eq(parameter, POWERSTEP01_PARAM_ALARM_EN_SW_TURN_ON)) {
        uint8_t alarmEnSwTurnOn = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, ALARM_EN_SW_TURN_ON, &alarmEnSwTurnOn);
        printf("%d\n", alarmEnSwTurnOn);

    } else if (eq(parameter, POWERSTEP01_PARAM_ALARM_EN_COMMAND_ERROR)) {
        uint8_t alarmEnCommandError = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, ALARM_EN_COMMAND_ERROR, &alarmEnCommandError);
        printf("%d\n", alarmEnCommandError);

    } else if (eq(parameter, POWERSTEP01_PARAM_GATECFG1_TCC)) {
        uint8_t gateCfg1Tcc = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, GATECFG1_TCC, &gateCfg1Tcc);
        printf("%d\n", gateCfg1Tcc);

    } else if (eq(parameter, POWERSTEP01_PARAM_GATECFG1_IGATE)) {
        uint8_t gateCfg1Igate = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, GATECFG1_IGATE, &gateCfg1Igate);
        printf("%d\n", gateCfg1Igate);

    } else if (eq(parameter, POWERSTEP01_PARAM_GATECFG1_TBOOST)) {
        uint8_t gateCfg1Tboost = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, GATECFG1_TBOOST, &gateCfg1Tboost);
        printf("%d\n", gateCfg1Tboost);

    } else if (eq(parameter, POWERSTEP01_PARAM_GATECFG1_WD_EN)) {
        uint8_t gateCfg1WdEn = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, GATECFG1_WD_EN, &gateCfg1WdEn);
        printf("%d\n", gateCfg1WdEn);

    } else if (eq(parameter, POWERSTEP01_PARAM_GATECFG2_TDT)) {
        uint8_t gateCfg2Tdt = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, GATECFG2_TDT, &gateCfg2Tdt);
        printf("%d\n", gateCfg2Tdt);

    } else if (eq(parameter, POWERSTEP01_PARAM_GATECFG2_TBLANK)) {
        uint8_t gateCfg2Tblank = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, GATECFG2_TBLANK, &gateCfg2Tblank);
        printf("%d\n", gateCfg2Tblank);

#ifdef CONFIG_VOLTAGE_MODE
    } else if (eq(parameter, POWERSTEP01_PARAM_CONFIG_OSC_SEL)) {
        uint8_t oscSel = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, CONFIG_OSC_SEL, &oscSel);
        printf("%d\n", oscSel);
    } else if (eq(parameter, POWERSTEP01_PARAM_CONFIG_EXT_CLK)) {
        uint8_t extClk = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, CONFIG_EXT_CLK, &extClk);
        printf("%d\n", extClk);

    } else if (eq(parameter, POWERSTEP01_PARAM_CONFIG_SW_MODE)) {
        uint8_t swMode = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, CONFIG_SW_MODE, &swMode);
        printf("%d\n", swMode);

    } else if (eq(parameter, POWERSTEP01_PARAM_CONFIG_OC_SD)) {
        uint8_t ocSd = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, CONFIG_OC_SD, &ocSd);
        printf("%d\n", ocSd);

    } else if (eq(parameter, POWERSTEP01_PARAM_CONFIG_UVLOVAL)) {
        uint8_t uvloVal = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, CONFIG_UVLOVAL, &uvloVal);
        printf("%d\n", uvloVal);

    } else if (eq(parameter, POWERSTEP01_PARAM_CONFIG_VCCVAL)) {
        uint8_t vccVal = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, CONFIG_VCCVAL, &vccVal);
        printf("%d\n", vccVal);

    } else if (eq(parameter, POWERSTEP01_PARAM_CONFIG_EN_VSCOMP)) {
        uint8_t enVscomp = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, VM_CONFIG_EN_VSCOMP, &enVscomp);
        printf("%d\n", enVscomp);

    } else if (eq(parameter, POWERSTEP01_PARAM_CONFIG_F_PWM_DEC)) {
        uint8_t fPwmDec = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, VM_CONFIG_F_PWM_DEC, &fPwmDec);
        printf("%d\n", fPwmDec);

    } else if (eq(parameter, POWERSTEP01_PARAM_CONFIG_F_PWM_INT)) {
        uint8_t fPwmInt = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, VM_CONFIG_F_PWM_INT, &fPwmInt);
        printf("%d\n", fPwmInt);

    } else if (eq(parameter, POWERSTEP01_PARAM_KVAL_HOLD)) {
        float kvalHold = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, VM_KVAL_HOLD, &kvalHold);
        printf("%f\n", kvalHold);

    } else if (eq(parameter, POWERSTEP01_PARAM_KVAL_RUN)) {
        float kvalRun = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, VM_KVAL_RUN, &kvalRun);
        printf("%f\n", kvalRun);

    } else if (eq(parameter, POWERSTEP01_PARAM_KVAL_ACC)) {
        float kvalAcc = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, VM_KVAL_ACC, &kvalAcc);
        printf("%f\n", kvalAcc);

    } else if (eq(parameter, POWERSTEP01_PARAM_KVAL_DEC)) {
        float kvalDec = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, VM_KVAL_DEC, &kvalDec);
        printf("%f\n", kvalDec);

    } else if (eq(parameter, POWERSTEP01_PARAM_INT_SPEED)) {
        float intSpeed = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, VM_INT_SPEED, &intSpeed);
        printf("%f\n", intSpeed);

    } else if (eq(parameter, POWERSTEP01_PARAM_ST_SLP)) {
        float stSlp = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, VM_ST_SLP, &stSlp);
        printf("%f\n", stSlp);

    } else if (eq(parameter, POWERSTEP01_PARAM_FN_SLP_ACC)) {
        float fnSlpAcc = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, VM_FN_SLP_ACC, &fnSlpAcc);
        printf("%f\n", fnSlpAcc);

    } else if (eq(parameter, POWERSTEP01_PARAM_FN_SLP_DEC)) {
        float fnSlpDec = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, VM_FN_SLP_DEC, &fnSlpDec);
        printf("%f\n", fnSlpDec);

    } else if (eq(parameter, POWERSTEP01_PARAM_K_THERM)) {
        float kTherm = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, VM_K_THERM, &kTherm);
        printf("%f\n", kTherm);

    } else if (eq(parameter, POWERSTEP01_PARAM_STALL_TH)) {
        float stallTh = 0;
        errorCode = MotorStepperParam::getAdvancedParam((MotorNum_t) deviceId, VM_STALL_TH, &stallTh);
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

static void _registerDevice(void)
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

int MotorStepperParam::_registerCLI(void)
{
    _registerDevice();
    return 0;
}
