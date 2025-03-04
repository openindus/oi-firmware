/**
 * @file StepperParam.h
 * @brief Stepper parameters
 * @author  Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#if defined(CONFIG_VOLTAGE_MODE)
#define STEPPER_PARAM_MOTOR_DEFAULT()                                           \
{                                                                               \
    .vm = {                                                                     \
        .cp = {                                                                 \
            .cmVmSelection = POWERSTEP01_CM_VM_VOLTAGE,                         \
            .acceleration = 2008.16,                                            \
            .deceleration = 2008.16,                                            \
            .maxSpeed = 991.82,                                                 \
            .minSpeed = 0,                                                      \
            .lowSpeedOptimization = POWERSTEP01_LSPD_OPT_OFF,                   \
            .fullStepSpeed = 595.09,                                            \
            .boostMode = POWERSTEP01_BOOST_MODE_OFF,                            \
            .ocdThreshold = POWERSTEP01_OCD_TH_281_25mV,                        \
            .stepMode = STEP_MODE_1_16,                                         \
            .syncClockSelection = POWERSTEP01_SYNC_SEL_DISABLED,                \
            .alarmsSelection = (POWERSTEP01_ALARM_EN_OVERCURRENT |              \
                                POWERSTEP01_ALARM_EN_THERMAL_SHUTDOWN |         \
                                POWERSTEP01_ALARM_EN_THERMAL_WARNING |          \
                                POWERSTEP01_ALARM_EN_UVLO |                     \
                                POWERSTEP01_ALARM_EN_STALL_DETECTION |          \
                                POWERSTEP01_ALARM_EN_SW_TURN_ON |               \
                                POWERSTEP01_ALARM_EN_WRONG_NPERF_CMD),          \
            .iGate = POWERSTEP01_IGATE_64mA,                                    \
            .tBoost = POWERSTEP01_TBOOST_0ns,                                   \
            .tcc = POWERSTEP01_TCC_500ns,                                       \
            .wdEn = POWERSTEP01_WD_EN_DISABLE,                                  \
            .tBlank = POWERSTEP01_TBLANK_375ns,                                 \
            .tdt = POWERSTEP01_TDT_125ns,                                       \
        },                                                                      \
        .kvalHold = 16.02,                                                      \
        .kvalRun = 16.02,                                                       \
        .kvalAcc = 16.02,                                                       \
        .kvalDec = 16.02,                                                       \
        .intersectSpeed = 61.512,                                               \
        .startSlope = 0.03815,                                                  \
        .accelerationFinalSlope = 0.06256,                                      \
        .decelerationFinalSlope = 0.06256,                                      \
        .thermalCompensationFactor = 1,                                         \
        .stallThreshold = 531.25,                                               \
        .oscClkSel = POWERSTEP01_CONFIG_INT_16MHZ_OSCOUT_2MHZ,                  \
        .swMode = POWERSTEP01_CONFIG_SW_HARD_STOP,                              \
        .enVsComp = POWERSTEP01_CONFIG_VS_COMP_DISABLE,                         \
        .ocSd = POWERSTEP01_CONFIG_OC_SD_DISABLE,                               \
        .uvloVal = POWERSTEP01_CONFIG_UVLOVAL_LOW,                              \
        .vccVal = POWERSTEP01_CONFIG_VCCVAL_15V,                                \
        .fPwmInt = POWERSTEP01_CONFIG_PWM_DIV_1,                                \
        .fPwmDec = POWERSTEP01_CONFIG_PWM_MUL_2,                                \
    }                                                                           \
}
#elif defined(CONFIG_CURRENT_MODE)
#define STEPPER_PARAM_MOTOR_DEFAULT()                                           \
{                                                                               \
    .cm = {                                                                     \
        .cp = {                                                                 \
            .cmVmSelection = POWERSTEP01_CM_VM_CURRENT,                         \
            .acceleration = 2008.16,                                            \
            .deceleration = 2008.16,                                            \
            .maxSpeed = 991.82,                                                 \
            .minSpeed = 0,                                                      \
            .lowSpeedOptimization = POWERSTEP01_LSPD_OPT_OFF,                   \
            .fullStepSpeed = 595.09,                                            \
            .boostMode = POWERSTEP01_BOOST_MODE_OFF,                            \
            .ocdThreshold = POWERSTEP01_OCD_TH_281_25mV,                        \
            .stepMode = STEP_MODE_1_16,                                         \
            .syncClockSelection = POWERSTEP01_SYNC_SEL_DISABLED,                \
            .alarmsSelection = (POWERSTEP01_ALARM_EN_OVERCURRENT |              \
                                POWERSTEP01_ALARM_EN_THERMAL_SHUTDOWN |         \
                                POWERSTEP01_ALARM_EN_THERMAL_WARNING |          \
                                POWERSTEP01_ALARM_EN_UVLO |                     \
                                POWERSTEP01_ALARM_EN_STALL_DETECTION |          \
                                POWERSTEP01_ALARM_EN_SW_TURN_ON |               \
                                POWERSTEP01_ALARM_EN_WRONG_NPERF_CMD),          \
            .iGate = POWERSTEP01_IGATE_64mA,                                    \
            .tBoost = POWERSTEP01_TBOOST_0ns,                                   \
            .tcc = POWERSTEP01_TCC_500ns,                                       \
            .wdEn = POWERSTEP01_WD_EN_DISABLE,                                  \
            .tBlank = POWERSTEP01_TBLANK_375ns,                                 \
            .tdt = POWERSTEP01_TDT_125ns,                                       \
        },                                                                      \
        .tvalHold = 328.12,                                                     \
        .tvalRun = 328.12,                                                      \
        .tvalAcc = 328.12,                                                      \
        .tvalDec = 328.12,                                                      \
        .toffFast = POWERSTEP01_TOFF_FAST_8us,                                  \
        .fastStep = POWERSTEP01_FAST_STEP_12us,                                 \
        .tonMin = 3.0,                                                          \
        .toffMin = 21.0,                                                        \
        .oscClkSel = POWERSTEP01_CONFIG_INT_16MHZ_OSCOUT_2MHZ,                  \
        .swMode = POWERSTEP01_CONFIG_SW_HARD_STOP,                              \
        .tqReg = POWERSTEP01_CONFIG_TQ_REG_TVAL_USED,                           \
        .enVsComp = POWERSTEP01_CONFIG_VS_COMP_DISABLE,                         \
        .ocSd = POWERSTEP01_CONFIG_OC_SD_DISABLE,                               \
        .uvloVal = POWERSTEP01_CONFIG_UVLOVAL_LOW,                              \
        .vccVal = POWERSTEP01_CONFIG_VCCVAL_15V,                                \
        .tsw = POWERSTEP01_CONFIG_TSW_048us,                                    \
        .predEn = POWERSTEP01_CONFIG_PRED_DISABLE,                              \
    }                                                                           \
}
#endif
