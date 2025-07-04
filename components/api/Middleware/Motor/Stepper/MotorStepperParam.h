/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file MotorStepperParam.h
 * @brief 
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "Motor.h"
#include "powerSTEP01/PS01.h"

typedef enum {
    ABS_POS,
    EL_POS_MICROSTEP,
    EL_POS_STEP,
    MARK,
    SPEED,
    ACCELERATION,
    DECELERATION,
    MAX_SPEED,
    MIN_SPEED,
    MIN_SPEED_LSPD_OPT,
    ADC_OUT,
    OCD_TH,
    FS_SPD,
    FS_SPD_BOOST_MODE,
    STEP_MODE_STEP_SEL,
    STEP_MODE_CM_VM,
    STEP_MODE_SYNC_SEL,
    STEP_MODE_SYNC_EN,
    ALARM_EN_OVERCURRENT,
    ALARM_EN_THERMAL_SHUTDOWN,
    ALARM_EN_THERMAL_WARNING,
    ALARM_EN_UVLO,
    ALARM_EN_ADC_UVLO,
    ALARM_EN_STALL_DETECTION,
    ALARM_EN_SW_TURN_ON,
    ALARM_EN_COMMAND_ERROR,
    GATECFG1_TCC,
    GATECFG1_IGATE,
    GATECFG1_TBOOST,
    GATECFG1_WD_EN,
    GATECFG2_TDT,
    GATECFG2_TBLANK,
    CONFIG_OSC_SEL,
    CONFIG_EXT_CLK,
    CONFIG_SW_MODE,
    CONFIG_OC_SD,
    CONFIG_UVLOVAL,
    CONFIG_VCCVAL,

#ifdef CONFIG_CURRENT_MODE
    CM_CONFIG_EN_TQREG,
    CM_CONFIG_TSW,
    CM_CONFIG_PRED_EN,
    CM_TVAL_HOLD,
    CM_TVAL_RUN,
    CM_TVAL_ACC,
    CM_TVAL_DEC,
    CM_TFAST_FAST_STEP,
    CM_TFAST_TOFF_FAST,
    CM_TON_MIN,
    CM_TOFF_MIN
#else
    VM_CONFIG_EN_VSCOMP,
    VM_CONFIG_F_PWM_DEC,
    VM_CONFIG_F_PWM_INT,
    VM_KVAL_HOLD,
    VM_KVAL_RUN,
    VM_KVAL_ACC,
    VM_KVAL_DEC,
    VM_INT_SPEED,
    VM_ST_SLP,
    VM_FN_SLP_ACC,
    VM_FN_SLP_DEC,
    VM_K_THERM,
    VM_STALL_TH,
#endif

} AdvancedParameter_t;

typedef struct {
    int32_t absPos;
    int32_t elPosMicroStep;
    int32_t elPosStep;
    int32_t mark;
    float acc;
    float dec;
    float maxSpeed;
    float minSpeed;
    bool minSpeedLspdOpt;
    float ocdTh;
    float fsSpd;
    bool fsSpdBoostMode;

    uint8_t stepModeStepSel;
    uint8_t stepModeCmVm;
    uint8_t stepModeSyncSel;

    bool stepModeSyncEn;
    bool alarmEnOvercurrent;
    bool alarmEnThermalShutdown;
    bool alarmEnThermalWarning;
    bool alarmEnUvlo;
    bool alarmEnAdcUvlo;
    bool alarmEnStallDetection;
    bool alarmEnSwTurnOn;
    bool alarmEnCommandError;

    uint8_t gateCfg1Tcc;
    uint8_t gateCfg1Igate;
    uint8_t gateCfg1Tboost;
    uint8_t gateCfg1WdEn;
    uint8_t gateCfg2Tdt;
    uint8_t gateCfg2Tblank;

    uint8_t configOscSel;
    uint8_t configExtClk;
    uint8_t configSwMode;
    uint8_t configOcSd;
    uint8_t configUvloval;
    uint8_t configVccval;
#ifdef CONFIG_CURRENT_MODE
    uint8_t cmConfigEnTqreg;
    uint8_t cmConfigTsw;
    uint8_t cmConfigPredEn;
    float cmTvalHold;
    float cmTvalRun;
    float cmTvalAcc;
    float cmTvalDec;
    uint8_t cmTfastFastStep;
    uint8_t cmTfastToffFast;
    float cmTonMin;
    float cmToffMin;
#else
    uint8_t vmConfigEnVscomp;
    uint8_t vmConfigFPwmDec;
    uint8_t vmConfigFPwmInt;
    float vmKvalHold;
    float vmKvalRun;
    float vmKvalAcc;
    float vmKvalDec;
    float vmIntSpeed;
    float vmStSlp;
    float vmFnSlpAcc;
    float vmFnSlpDec;
    float vmKTherm;
    float vmStallTh;

#endif
    bool isInit;
} PS01_AdvancedParam_t; 

const PS01_AdvancedParam_t defaultParameters = {
    .absPos = 0,
    .elPosMicroStep = 0,
    .elPosStep = 0,

    .mark = 0,
    .acc = 2008.16,
    .dec = 2008.16,
    .maxSpeed = 991.821,

    .minSpeed = 0,
    .minSpeedLspdOpt = false,

    .ocdTh = 156.25,
    .fsSpd = 991.821,
    .fsSpdBoostMode = false,

    .stepModeStepSel = 4,
    .stepModeCmVm = 0,
    .stepModeSyncSel = 0,
    .stepModeSyncEn = false,

    .alarmEnOvercurrent = true,
    .alarmEnThermalShutdown = true,
    .alarmEnThermalWarning = true,
    .alarmEnUvlo = true,
    .alarmEnAdcUvlo = true,
    .alarmEnStallDetection = true,
    .alarmEnSwTurnOn = false,
    .alarmEnCommandError = true,

    .gateCfg1Tcc = 3,
    .gateCfg1Igate = 6,
    .gateCfg1Tboost = 0,
    .gateCfg1WdEn = 0,

    .gateCfg2Tdt = 0,
    .gateCfg2Tblank = 2,

    .configOscSel = 0,
    .configExtClk = 0,
    .configSwMode = 0,

    .configOcSd = 1,
    .configUvloval = 0,
    .configVccval = 1,

#ifdef CONFIG_CURRENT_MODE
    .cmConfigEnTqreg = 0,
    .cmConfigTsw = 12,
    .cmConfigPredEn = 0,
    .cmTvalHold = 328.12,
    .cmTvalRun = 328.12,
    .cmTvalAcc = 328.12,
    .cmTvalDec = 328.12,
    .cmTfastFastStep = 3,
    .cmTfastToffFast = 5,
    .cmTonMin = 3,
    .cmToffMin = 21,
#else
    .vmConfigEnVscomp = 0,
    .vmConfigFPwmDec = 7,
    .vmConfigFPwmInt = 0,

    .vmKvalHold = 16.02,
    .vmKvalRun = 16.02,
    .vmKvalAcc = 16.02,
    .vmKvalDec = 16.02,
    .vmIntSpeed = 61.512,
    .vmStSlp = 0.03815,
    .vmFnSlpAcc = 0.06256,
    .vmFnSlpDec = 0.06256,
    .vmKTherm = 1,
    .vmStallTh = 468.75,

#endif
    .isInit = false
};

class MotorStepperParam
{
public:

    static int setAdvancedParam(MotorNum_t motor, AdvancedParameter_t advParam, void* value);
    static int getAdvancedParam(MotorNum_t motor, AdvancedParameter_t advParam, void* value);

    static PS01_AdvancedParam_t getAllAdvancedParamPS01(MotorNum_t motor);
    static int setAllAdvancedParamPS01(MotorNum_t motor, PS01_AdvancedParam_t advancedParam);
    static int resetAllAdvancedParamPS01(MotorNum_t motor);
    
    static PS01_AdvancedParam_t getNVSParam(MotorNum_t motor);
    static int setNVSParam(MotorNum_t motor, PS01_AdvancedParam_t param);
    static int initNVSParam(void);

private:

    static int setAdvancedParamWithoutNvs(MotorNum_t motor, AdvancedParameter_t advParam, void* value);

    static int _registerCLI(void);
};
