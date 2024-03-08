/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file StepperParamCLI.h
 * @brief OpenIndus console
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "MotorStepperParam.h"

#if defined(OI_STEPPER) || defined(OI_STEPPER_VE)

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

class StepperParamCLI
{
public:

    static void init(void);

private:

    static void _registerDevice(void);
};

#endif
