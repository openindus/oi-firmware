/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Project.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file powerSTEP01_register.h
 * @brief Drivers for powerSTEP01 component
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "PS01_Global.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum {
    POWERSTEP01_ABS_POS     = ((uint8_t)0x01),
    POWERSTEP01_EL_POS      = ((uint8_t)0x02),
    POWERSTEP01_MARK        = ((uint8_t)0x03), 
    POWERSTEP01_SPEED       = ((uint8_t)0x04),
    POWERSTEP01_ACC         = ((uint8_t)0x05),
    POWERSTEP01_DEC         = ((uint8_t)0x06),
    POWERSTEP01_MAX_SPEED   = ((uint8_t)0x07),
    POWERSTEP01_MIN_SPEED   = ((uint8_t)0x08),
    POWERSTEP01_FS_SPD      = ((uint8_t)0x15),
    POWERSTEP01_KVAL_HOLD   = ((uint8_t)0x09),
    POWERSTEP01_KVAL_RUN    = ((uint8_t)0x0A),
    POWERSTEP01_KVAL_ACC    = ((uint8_t)0x0B),
    POWERSTEP01_KVAL_DEC    = ((uint8_t)0x0C),
    POWERSTEP01_INT_SPD     = ((uint8_t)0x0D),
    POWERSTEP01_ST_SLP      = ((uint8_t)0x0E),
    POWERSTEP01_FN_SLP_ACC  = ((uint8_t)0x0F),
    POWERSTEP01_FN_SLP_DEC  = ((uint8_t)0x10),
    POWERSTEP01_K_THERM     = ((uint8_t)0x11),
    POWERSTEP01_ADC_OUT     = ((uint8_t)0x12),
    POWERSTEP01_OCD_TH      = ((uint8_t)0x13),
    POWERSTEP01_STALL_TH    = ((uint8_t)0x14),
    POWERSTEP01_STEP_MODE   = ((uint8_t)0x16),
    POWERSTEP01_ALARM_EN    = ((uint8_t)0x17),
    POWERSTEP01_GATECFG1    = ((uint8_t)0x18),
    POWERSTEP01_GATECFG2    = ((uint8_t)0x19),
    POWERSTEP01_CONFIG      = ((uint8_t)0x1A),
    POWERSTEP01_STATUS      = ((uint8_t)0x1B)
} PS01_Register_t;

#define POWERSTEP01_TVAL_HOLD   (POWERSTEP01_KVAL_HOLD)
#define POWERSTEP01_TVAL_RUN    (POWERSTEP01_KVAL_RUN)
#define POWERSTEP01_TVAL_ACC    (POWERSTEP01_KVAL_ACC)
#define POWERSTEP01_TVAL_DEC    (POWERSTEP01_KVAL_DEC)
#define POWERSTEP01_T_FAST      (POWERSTEP01_ST_SLP)
#define POWERSTEP01_TON_MIN     (POWERSTEP01_FN_SLP_ACC)
#define POWERSTEP01_TOFF_MIN    (POWERSTEP01_FN_SLP_DEC)

#define POWERSTEP01_ACC_DEC_MAX_VALUE       (float)(59590)                  // powerSTEP01 acceleration and deceleration max value
#define POWERSTEP01_MAX_SPEED_MAX_VALUE     (float)(15610)                  // powerSTEP01 max speed max value
#define POWERSTEP01_MIN_SPEED_MAX_VALUE     (float)(976.3)                  // powerSTEP01 min speed max value
#define POWERSTEP01_FS_SPD_MAX_VALUE        (float)(15625)                  // powerSTEP01 full step speed max value
#define POWERSTEP01_INT_SPD_MAX_VALUE       (float)(976.5)                  // powerSTEP01 intersect speed max value
#define POWERSTEP01_K_THERM_MAX_VALUE       (float)(1.46875)                // powerSTEP01 thermal compensation max value
#define POWERSTEP01_K_THERM_MIN_VALUE       (float)(1)                      // powerSTEP01 thermal compensation min value
#define POWERSTEP01_STALL_OCD_TH_MAX_VALUE  (float)(1000)                   // powerSTEP01 thermal compensation max value
#define POWERSTEP01_K_THERM_MAX_VALUE       (float)(1.46875)                // powerSTEP01 thermal compensation max value
#define POWERSTEP01_KVAL_MAX_VALUE          (float)((255.0/256.0) * 100.0)  // powerSTEP01 voltage amplitude regulation max value
#define POWERSTEP01_SLP_MAX_VALUE           (float)(0.4)                    // powerSTEP01 BEMF compensation curve slope max value
#define POWERSTEP01_TVAL_MAX_VALUE          (float)(1000)                   // powerSTEP01 torque regulation DAC reference voltage max value
#define POWERSTEP01_TOFF_TON_MIN_MAX_VALUE  (float)(64)                     // powerSTEP01 minimum off and on time max value
#define POWERSTEP01_ALARM_EN_MAX_VALUE      (float)(0xFF)                   // powerSTEP01 max value

#define POWERSTEP01_LSPD_OPT_SHIFT            (12)      // Shift of the low speed optimization bit in MIN_SPEED register
#define POWERSTEP01_BOOST_MODE_SHIFT          (10)      // Shift of the boost mode bit in FS_SPD register
#define POWERSTEP01_TOFF_FAST_UNIT_US         (2)       // Maximum fast decay time (TOFF_FAST) unit
#define POWERSTEP01_TOFF_FAST_SHIFT           (4)       // Shift of the maximum fast decay time (TOFF_FAST) in T_FAST register
#define POWERSTEP01_FAST_STEP_UNIT_US         (2)       // Maximum fall step time (FAST_STEP) unit
#define POWERSTEP01_FAST_STEP_SHIFT           (0)       // Shift of the maximum fall step time (FAST_STEP) in T_FAST register
#define POWERSTEP01_TCC_UNIT_NS               (125)     // Duration unit of constant current phase during gate turn-on and turn-off (TCC)
#define POWERSTEP01_TCC_SHIFT                 (0)       // Shift of TCC field in GATECFG1 register
#define POWERSTEP01_IGATE_SHIFT               (5)       // Shift of IGATE field in GATECFG1 register
#define POWERSTEP01_TBOOST_SHIFT              (8)       // Shift of TBOOST field in GATECFG1 register
#define POWERSTEP01_TBLANK_UNIT_NS            (125)     // Duration unit of the blanking of the current sensing comparators (TBLANK)
#define POWERSTEP01_TBLANK_SHIFT              (5)       // Shift of TBLANK field in GATECFG2 register
#define POWERSTEP01_TDT_UNIT_NS               (125)     // Deadtime duration unit between gate turn-off and opposite gate turn-on (TDT)
#define POWERSTEP01_TDT_SHIFT                 (0)       // Shift of TDT field in GATECFG2 register
#define POWERSTEP01_CONFIG_PWM_DIV_SHIFT      (13)      // Shift of F_PWM_INT field in CONFIG register for voltage mode
#define POWERSTEP01_CONFIG_PWM_MUL_SHIFT      (10)      // Shift of F_PWM_DEC field in CONFIG register for voltage mode
#define POWERSTEP01_CONFIG_TSW_UNIT_US        (4)       // Target switching period (TSW) unit
#define POWERSTEP01_CONFIG_TSW_SHIFT          (10)      // Shift of TSW field in CONFIG register for current mode
#define POWERSTEP01_STATUS_MOT_STATUS_SHIFT   (5)       // Shift of MOT_STATUS field in STATUS register

/* masks for ABS_POS register of PowerStep01 */
typedef enum {
    POWERSTEP01_ABS_POS_VALUE_MASK        = ((uint32_t)0x003FFFFF),
    POWERSTEP01_ABS_POS_SIGN_BIT_MASK     = ((uint32_t)0x00200000)
} PS01_AbsPosMasks_t;

/* masks for EL_POS register of PowerStep01 */
typedef enum {
    POWERSTEP01_ELPOS_STEP_MASK       = ((uint16_t)0x180),
    POWERSTEP01_ELPOS_MICROSTEP_MASK  = ((uint16_t)0x07F)
} PS01_ElPosMasks_t;

/* masks for MIN_SPEED register of PowerStep01 */
typedef enum {
    POWERSTEP01_LSPD_OPT        = ((uint16_t)((0x1)<<POWERSTEP01_LSPD_OPT_SHIFT)),
    POWERSTEP01_MIN_SPEED_MASK  = ((uint16_t)0x0FFF)
} PS01_MinSpeedMasks_t;

/* Low speed optimization (MIN_SPEED register of PowerStep01) */
typedef enum {
    POWERSTEP01_LSPD_OPT_OFF    = ((uint16_t)0x0000),
    POWERSTEP01_LSPD_OPT_ON     = ((uint16_t)POWERSTEP01_LSPD_OPT)
} PS01_LspdOpt_t;

/* masks for FS_SPD register of PowerStep01 */
typedef enum {
    POWERSTEP01_BOOST_MODE   = ((uint16_t)((0x1)<<POWERSTEP01_BOOST_MODE_SHIFT)),
    POWERSTEP01_FS_SPD_MASK  = ((uint16_t)0x03FF)
} PS01_FsSpdMasks_t;

/* Full step boost (FS_SPD register of PowerStep01) */
typedef enum {
    POWERSTEP01_BOOST_MODE_OFF    = ((uint16_t)0x0000),
    POWERSTEP01_BOOST_MODE_ON     = ((uint16_t)POWERSTEP01_BOOST_MODE)
} PS01_BoostMode_t;

/* masks for T_FAST register of PowerStep01 */
typedef enum {
    POWERSTEP01_FAST_STEP_MASK  = ((uint16_t) ((0xF)<<POWERSTEP01_FAST_STEP_SHIFT)),
    POWERSTEP01_TOFF_FAST_MASK  = ((uint16_t) ((0xF)<<POWERSTEP01_TOFF_FAST_SHIFT))
} PS01_TFastMasks_t;

/* Maximum fall step times (T_FAST register of PowerStep01) */
typedef enum {
    POWERSTEP01_FAST_STEP_2us     = (((uint8_t)0x00)<<POWERSTEP01_FAST_STEP_SHIFT),
    POWERSTEP01_FAST_STEP_4us     = (((uint8_t)0x01)<<POWERSTEP01_FAST_STEP_SHIFT),
    POWERSTEP01_FAST_STEP_6us     = (((uint8_t)0x02)<<POWERSTEP01_FAST_STEP_SHIFT),
    POWERSTEP01_FAST_STEP_8us     = (((uint8_t)0x03)<<POWERSTEP01_FAST_STEP_SHIFT),
    POWERSTEP01_FAST_STEP_10us    = (((uint8_t)0x04)<<POWERSTEP01_FAST_STEP_SHIFT),
    POWERSTEP01_FAST_STEP_12us    = (((uint8_t)0x05)<<POWERSTEP01_FAST_STEP_SHIFT),
    POWERSTEP01_FAST_STEP_14us    = (((uint8_t)0x06)<<POWERSTEP01_FAST_STEP_SHIFT),
    POWERSTEP01_FAST_STEP_16us    = (((uint8_t)0x07)<<POWERSTEP01_FAST_STEP_SHIFT),
    POWERSTEP01_FAST_STEP_18us    = (((uint8_t)0x08)<<POWERSTEP01_FAST_STEP_SHIFT),
    POWERSTEP01_FAST_STEP_20us    = (((uint8_t)0x09)<<POWERSTEP01_FAST_STEP_SHIFT),
    POWERSTEP01_FAST_STEP_22us    = (((uint8_t)0x0A)<<POWERSTEP01_FAST_STEP_SHIFT),
    POWERSTEP01_FAST_STEP_24s     = (((uint8_t)0x0B)<<POWERSTEP01_FAST_STEP_SHIFT),
    POWERSTEP01_FAST_STEP_26us    = (((uint8_t)0x0C)<<POWERSTEP01_FAST_STEP_SHIFT),
    POWERSTEP01_FAST_STEP_28us    = (((uint8_t)0x0D)<<POWERSTEP01_FAST_STEP_SHIFT),
    POWERSTEP01_FAST_STEP_30us    = (((uint8_t)0x0E)<<POWERSTEP01_FAST_STEP_SHIFT),
    POWERSTEP01_FAST_STEP_32us    = (((uint8_t)0x0F)<<POWERSTEP01_FAST_STEP_SHIFT)
} PS01_FastStep_t;

/* Maximum fast decay times (T_FAST register of PowerStep01) */
typedef enum {
    POWERSTEP01_TOFF_FAST_2us     = (((uint8_t)0x00)<<POWERSTEP01_TOFF_FAST_SHIFT),
    POWERSTEP01_TOFF_FAST_4us     = (((uint8_t)0x01)<<POWERSTEP01_TOFF_FAST_SHIFT),
    POWERSTEP01_TOFF_FAST_6us     = (((uint8_t)0x02)<<POWERSTEP01_TOFF_FAST_SHIFT),
    POWERSTEP01_TOFF_FAST_8us     = (((uint8_t)0x03)<<POWERSTEP01_TOFF_FAST_SHIFT),
    POWERSTEP01_TOFF_FAST_10us    = (((uint8_t)0x04)<<POWERSTEP01_TOFF_FAST_SHIFT),
    POWERSTEP01_TOFF_FAST_12us    = (((uint8_t)0x05)<<POWERSTEP01_TOFF_FAST_SHIFT),
    POWERSTEP01_TOFF_FAST_14us    = (((uint8_t)0x06)<<POWERSTEP01_TOFF_FAST_SHIFT),
    POWERSTEP01_TOFF_FAST_16us    = (((uint8_t)0x07)<<POWERSTEP01_TOFF_FAST_SHIFT),
    POWERSTEP01_TOFF_FAST_18us    = (((uint8_t)0x08)<<POWERSTEP01_TOFF_FAST_SHIFT),
    POWERSTEP01_TOFF_FAST_20us    = (((uint8_t)0x09)<<POWERSTEP01_TOFF_FAST_SHIFT),
    POWERSTEP01_TOFF_FAST_22us    = (((uint8_t)0x0A)<<POWERSTEP01_TOFF_FAST_SHIFT),
    POWERSTEP01_TOFF_FAST_24us    = (((uint8_t)0x0B)<<POWERSTEP01_TOFF_FAST_SHIFT),
    POWERSTEP01_TOFF_FAST_26us    = (((uint8_t)0x0C)<<POWERSTEP01_TOFF_FAST_SHIFT),
    POWERSTEP01_TOFF_FAST_28us    = (((uint8_t)0x0D)<<POWERSTEP01_TOFF_FAST_SHIFT),
    POWERSTEP01_TOFF_FAST_30us    = (((uint8_t)0x0E)<<POWERSTEP01_TOFF_FAST_SHIFT),
    POWERSTEP01_TOFF_FAST_32us    = (((uint8_t)0x0F)<<POWERSTEP01_TOFF_FAST_SHIFT)
} PS01_ToffFast_t;

/* Overcurrent threshold options (OCD register of PowerStep01) */
typedef enum {
    POWERSTEP01_OCD_TH_31_25mV    = ((uint8_t)0x00),
    POWERSTEP01_OCD_TH_62_5mV     = ((uint8_t)0x01),
    POWERSTEP01_OCD_TH_93_75mV    = ((uint8_t)0x02),
    POWERSTEP01_OCD_TH_125mV      = ((uint8_t)0x03),
    POWERSTEP01_OCD_TH_156_25mV   = ((uint8_t)0x04),
    POWERSTEP01_OCD_TH_187_50mV   = ((uint8_t)0x05),
    POWERSTEP01_OCD_TH_218_75mV   = ((uint8_t)0x06),
    POWERSTEP01_OCD_TH_250mV      = ((uint8_t)0x07),
    POWERSTEP01_OCD_TH_281_25mV   = ((uint8_t)0x08),
    POWERSTEP01_OCD_TH_312_5mV    = ((uint8_t)0x09),
    POWERSTEP01_OCD_TH_343_75mV   = ((uint8_t)0x0A),
    POWERSTEP01_OCD_TH_375mV      = ((uint8_t)0x0B),
    POWERSTEP01_OCD_TH_406_25mV   = ((uint8_t)0x0C),
    POWERSTEP01_OCD_TH_437_5mV    = ((uint8_t)0x0D),
    POWERSTEP01_OCD_TH_468_75mV   = ((uint8_t)0x0E),
    POWERSTEP01_OCD_TH_500mV      = ((uint8_t)0x0F),
    POWERSTEP01_OCD_TH_531_25mV   = ((uint8_t)0x10),
    POWERSTEP01_OCD_TH_562_5mV    = ((uint8_t)0x11),
    POWERSTEP01_OCD_TH_593_75mV   = ((uint8_t)0x12),
    POWERSTEP01_OCD_TH_625mV      = ((uint8_t)0x13),
    POWERSTEP01_OCD_TH_656_25mV   = ((uint8_t)0x14),
    POWERSTEP01_OCD_TH_687_5mV    = ((uint8_t)0x15),
    POWERSTEP01_OCD_TH_718_75mV   = ((uint8_t)0x16),
    POWERSTEP01_OCD_TH_750mV      = ((uint8_t)0x17),
    POWERSTEP01_OCD_TH_781_25mV   = ((uint8_t)0x18),
    POWERSTEP01_OCD_TH_812_5mV    = ((uint8_t)0x19),
    POWERSTEP01_OCD_TH_843_75mV   = ((uint8_t)0x1A),
    POWERSTEP01_OCD_TH_875mV      = ((uint8_t)0x1B),
    POWERSTEP01_OCD_TH_906_25mV   = ((uint8_t)0x1C),
    POWERSTEP01_OCD_TH_937_75mV   = ((uint8_t)0x1D),
    POWERSTEP01_OCD_TH_968_75mV   = ((uint8_t)0x1E),
    POWERSTEP01_OCD_TH_1V         = ((uint8_t)0x1F)        
} PS01_OcdTh_t;

/* masks for STEP_MODE register of PowerStep01 */
typedef enum {
    POWERSTEP01_STEP_MODE_STEP_SEL    = ((uint8_t)0x07),
    POWERSTEP01_STEP_MODE_CM_VM       = ((uint8_t)0x08),
    POWERSTEP01_STEP_MODE_SYNC_SEL    = ((uint8_t)0x70),
    POWERSTEP01_STEP_MODE_SYNC_EN     = ((uint8_t)0x80)
} PS01_StepModeMasks_t;

/* Voltage or Current mode selection (CM_VM field of STEP_MODE register of PowerStep01) */
typedef enum {
    POWERSTEP01_CM_VM_VOLTAGE    = ((uint8_t)0x00),
    POWERSTEP01_CM_VM_CURRENT    = ((uint8_t)0x08)
} PS01_CmVm_t;

/* Stepping options (field STEP_SEL of STEP_MODE register of PowerStep01) */
typedef enum {
    POWERSTEP01_STEP_SEL_1      = ((uint8_t)0x00),
    POWERSTEP01_STEP_SEL_1_2    = ((uint8_t)0x01),
    POWERSTEP01_STEP_SEL_1_4    = ((uint8_t)0x02),
    POWERSTEP01_STEP_SEL_1_8    = ((uint8_t)0x03),
    POWERSTEP01_STEP_SEL_1_16   = ((uint8_t)0x04),
    POWERSTEP01_STEP_SEL_1_32   = ((uint8_t)0x05),
    POWERSTEP01_STEP_SEL_1_64   = ((uint8_t)0x06),
    POWERSTEP01_STEP_SEL_1_128  = ((uint8_t)0x07)
} PS01_StepSel_t;

/* Powerstep01 Sync Output frequency enabling bitw */
#define POWERSTEP01_SYNC_EN   ((0x1) << 7)

/* SYNC_SEL options (STEP_MODE register of PowerStep01) */
typedef enum {
    POWERSTEP01_SYNC_SEL_DISABLED   = ((uint8_t)0x00),
    POWERSTEP01_SYNC_SEL_1_2        = ((uint8_t)(POWERSTEP01_SYNC_EN|0x00)),
    POWERSTEP01_SYNC_SEL_1          = ((uint8_t)(POWERSTEP01_SYNC_EN|0x10)),
    POWERSTEP01_SYNC_SEL_2          = ((uint8_t)(POWERSTEP01_SYNC_EN|0x20)),
    POWERSTEP01_SYNC_SEL_4          = ((uint8_t)(POWERSTEP01_SYNC_EN|0x30)),
    POWERSTEP01_SYNC_SEL_8          = ((uint8_t)(POWERSTEP01_SYNC_EN|0x40)),
    POWERSTEP01_SYNC_SEL_16         = ((uint8_t)(POWERSTEP01_SYNC_EN|0x50)),
    POWERSTEP01_SYNC_SEL_32         = ((uint8_t)(POWERSTEP01_SYNC_EN|0x60)),
    POWERSTEP01_SYNC_SEL_64         = ((uint8_t)(POWERSTEP01_SYNC_EN|0x70))
} PS01_SyncSel_t;

/* Alarms conditions (ALARM_EN register of PowerStep01) */
typedef enum {
    POWERSTEP01_ALARM_EN_OVERCURRENT        = ((uint8_t)0x01),
    POWERSTEP01_ALARM_EN_THERMAL_SHUTDOWN   = ((uint8_t)0x02),
    POWERSTEP01_ALARM_EN_THERMAL_WARNING    = ((uint8_t)0x04),
    POWERSTEP01_ALARM_EN_UVLO               = ((uint8_t)0x08),
    POWERSTEP01_ALARM_EN_ADC_UVLO           = ((uint8_t)0x10),
    POWERSTEP01_ALARM_EN_STALL_DETECTION    = ((uint8_t)0x20),
    POWERSTEP01_ALARM_EN_SW_TURN_ON         = ((uint8_t)0x40),
    POWERSTEP01_ALARM_EN_WRONG_NPERF_CMD    = ((uint8_t)0x80)
} PS01_AlarmEn_t;

/* masks for GATECFG1 register of PowerStep01 */
typedef enum {
    POWERSTEP01_GATECFG1_TCC_MASK    = ((uint16_t)0x001F),
    POWERSTEP01_GATECFG1_IGATE_MASK  = ((uint16_t)0x00E0),
    POWERSTEP01_GATECFG1_TBOOST_MASK = ((uint16_t)0x0700),
    POWERSTEP01_GATECFG1_WD_EN       = ((uint16_t)0x0800)
} PS01_GateCfg1Masks_t;

/* Control current Time (field TCC of GATECFG1 register of PowerStep01) */
typedef enum {
    POWERSTEP01_TCC_125ns       = (((uint8_t)0x00)<<POWERSTEP01_TCC_SHIFT),
    POWERSTEP01_TCC_250ns       = (((uint8_t)0x01)<<POWERSTEP01_TCC_SHIFT),
    POWERSTEP01_TCC_375ns       = (((uint8_t)0x02)<<POWERSTEP01_TCC_SHIFT),
    POWERSTEP01_TCC_500ns       = (((uint8_t)0x03)<<POWERSTEP01_TCC_SHIFT),
    POWERSTEP01_TCC_625ns       = (((uint8_t)0x04)<<POWERSTEP01_TCC_SHIFT),
    POWERSTEP01_TCC_750ns       = (((uint8_t)0x05)<<POWERSTEP01_TCC_SHIFT),
    POWERSTEP01_TCC_875ns       = (((uint8_t)0x06)<<POWERSTEP01_TCC_SHIFT),
    POWERSTEP01_TCC_1000ns      = (((uint8_t)0x07)<<POWERSTEP01_TCC_SHIFT),
    POWERSTEP01_TCC_1125ns      = (((uint8_t)0x08)<<POWERSTEP01_TCC_SHIFT),
    POWERSTEP01_TCC_1250ns      = (((uint8_t)0x09)<<POWERSTEP01_TCC_SHIFT),
    POWERSTEP01_TCC_1375ns      = (((uint8_t)0x0A)<<POWERSTEP01_TCC_SHIFT),
    POWERSTEP01_TCC_1500ns      = (((uint8_t)0x0B)<<POWERSTEP01_TCC_SHIFT),
    POWERSTEP01_TCC_1625ns      = (((uint8_t)0x0C)<<POWERSTEP01_TCC_SHIFT),
    POWERSTEP01_TCC_1750ns      = (((uint8_t)0x0D)<<POWERSTEP01_TCC_SHIFT),
    POWERSTEP01_TCC_1875ns      = (((uint8_t)0x0E)<<POWERSTEP01_TCC_SHIFT),
    POWERSTEP01_TCC_2000ns      = (((uint8_t)0x0F)<<POWERSTEP01_TCC_SHIFT),
    POWERSTEP01_TCC_2125ns      = (((uint8_t)0x10)<<POWERSTEP01_TCC_SHIFT),
    POWERSTEP01_TCC_2250ns      = (((uint8_t)0x11)<<POWERSTEP01_TCC_SHIFT),
    POWERSTEP01_TCC_2375ns      = (((uint8_t)0x12)<<POWERSTEP01_TCC_SHIFT),
    POWERSTEP01_TCC_2500ns      = (((uint8_t)0x13)<<POWERSTEP01_TCC_SHIFT),
    POWERSTEP01_TCC_2625ns      = (((uint8_t)0x14)<<POWERSTEP01_TCC_SHIFT),
    POWERSTEP01_TCC_2750ns      = (((uint8_t)0x15)<<POWERSTEP01_TCC_SHIFT),
    POWERSTEP01_TCC_2875ns      = (((uint8_t)0x16)<<POWERSTEP01_TCC_SHIFT),
    POWERSTEP01_TCC_3000ns      = (((uint8_t)0x17)<<POWERSTEP01_TCC_SHIFT),
    POWERSTEP01_TCC_3125ns      = (((uint8_t)0x18)<<POWERSTEP01_TCC_SHIFT),
    POWERSTEP01_TCC_3250ns      = (((uint8_t)0x19)<<POWERSTEP01_TCC_SHIFT),
    POWERSTEP01_TCC_3375ns      = (((uint8_t)0x1A)<<POWERSTEP01_TCC_SHIFT),
    POWERSTEP01_TCC_3500ns      = (((uint8_t)0x1B)<<POWERSTEP01_TCC_SHIFT),
    POWERSTEP01_TCC_3625ns      = (((uint8_t)0x1C)<<POWERSTEP01_TCC_SHIFT),
    POWERSTEP01_TCC_3750ns      = (((uint8_t)0x1D)<<POWERSTEP01_TCC_SHIFT),
    POWERSTEP01_TCC_3750ns_bis  = (((uint8_t)0x1E)<<POWERSTEP01_TCC_SHIFT),
    POWERSTEP01_TCC_3750ns_ter  = (((uint8_t)0x1F)<<POWERSTEP01_TCC_SHIFT)
} PS01_Tcc_t;

/* Igate options (GATECFG1 register of PowerStep01) */
typedef enum {
    POWERSTEP01_IGATE_4mA     = (((uint8_t)0x00)<<POWERSTEP01_IGATE_SHIFT),
    POWERSTEP01_IGATE_4mA_Bis = (((uint8_t)0x01)<<POWERSTEP01_IGATE_SHIFT),
    POWERSTEP01_IGATE_8mA     = (((uint8_t)0x02)<<POWERSTEP01_IGATE_SHIFT),
    POWERSTEP01_IGATE_16mA    = (((uint8_t)0x03)<<POWERSTEP01_IGATE_SHIFT),
    POWERSTEP01_IGATE_24mA    = (((uint8_t)0x04)<<POWERSTEP01_IGATE_SHIFT),
    POWERSTEP01_IGATE_32mA    = (((uint8_t)0x05)<<POWERSTEP01_IGATE_SHIFT),
    POWERSTEP01_IGATE_64mA    = (((uint8_t)0x06)<<POWERSTEP01_IGATE_SHIFT),
    POWERSTEP01_IGATE_96mA    = (((uint8_t)0x07)<<POWERSTEP01_IGATE_SHIFT),
} PS01_Igate_t;

/* Turn off boost time (TBOOST field of GATECFG1 register of PowerStep01) */
typedef enum {
    POWERSTEP01_TBOOST_0ns                = (((uint8_t)0x00)<<POWERSTEP01_TBOOST_SHIFT),
    POWERSTEP01_TBOOST_62_5__83_3__125ns  = (((uint8_t)0x01)<<POWERSTEP01_TBOOST_SHIFT),
    POWERSTEP01_TBOOST_125ns              = (((uint8_t)0x02)<<POWERSTEP01_TBOOST_SHIFT),
    POWERSTEP01_TBOOST_250ns              = (((uint8_t)0x03)<<POWERSTEP01_TBOOST_SHIFT),
    POWERSTEP01_TBOOST_375ns              = (((uint8_t)0x04)<<POWERSTEP01_TBOOST_SHIFT),
    POWERSTEP01_TBOOST_500ns              = (((uint8_t)0x05)<<POWERSTEP01_TBOOST_SHIFT),
    POWERSTEP01_TBOOST_750ns              = (((uint8_t)0x06)<<POWERSTEP01_TBOOST_SHIFT),
    POWERSTEP01_TBOOST_1000ns             = (((uint8_t)0x07)<<POWERSTEP01_TBOOST_SHIFT),
} PS01_Tboost_t;

/* External clock watchdog (WD_EN field of GATECFG1 register of PowerStep01) */
typedef enum {
    POWERSTEP01_WD_EN_DISABLE   = ((uint16_t)0x0000),
    POWERSTEP01_WD_EN_ENABLE    = ((uint16_t)((0x1) << 11))
} PS01_WdEn_t;


/* masks for GATECFG2 register of PowerStep01 */
typedef enum {
    POWERSTEP01_GATECFG2_TDT      = ((uint8_t)0x1F),
    POWERSTEP01_GATECFG2_TBLANK   = ((uint8_t)0xE0)
} PS01_GateCfg2Masks_t;

/* Blanking time (TBLANK field of GATECFG2 register of PowerStep01) */
typedef enum {
    POWERSTEP01_TBLANK_125ns    = (((uint8_t)0x00)<<POWERSTEP01_TBLANK_SHIFT),
    POWERSTEP01_TBLANK_250ns    = (((uint8_t)0x01)<<POWERSTEP01_TBLANK_SHIFT),
    POWERSTEP01_TBLANK_375ns    = (((uint8_t)0x02)<<POWERSTEP01_TBLANK_SHIFT),
    POWERSTEP01_TBLANK_500ns    = (((uint8_t)0x03)<<POWERSTEP01_TBLANK_SHIFT),
    POWERSTEP01_TBLANK_625ns    = (((uint8_t)0x04)<<POWERSTEP01_TBLANK_SHIFT),
    POWERSTEP01_TBLANK_750ns    = (((uint8_t)0x05)<<POWERSTEP01_TBLANK_SHIFT),
    POWERSTEP01_TBLANK_875ns    = (((uint8_t)0x06)<<POWERSTEP01_TBLANK_SHIFT),
    POWERSTEP01_TBLANK_1000ns   = (((uint8_t)0x07)<<POWERSTEP01_TBLANK_SHIFT),
} PS01_TBlank_t;

/* Dead time (TDT field of GATECFG2 register of PowerStep01) */
typedef enum {
    POWERSTEP01_TDT_125ns   = (((uint8_t)0x00)<<POWERSTEP01_TDT_SHIFT),
    POWERSTEP01_TDT_250ns   = (((uint8_t)0x01)<<POWERSTEP01_TDT_SHIFT),
    POWERSTEP01_TDT_375ns   = (((uint8_t)0x02)<<POWERSTEP01_TDT_SHIFT),
    POWERSTEP01_TDT_500ns   = (((uint8_t)0x03)<<POWERSTEP01_TDT_SHIFT),
    POWERSTEP01_TDT_625ns   = (((uint8_t)0x04)<<POWERSTEP01_TDT_SHIFT),
    POWERSTEP01_TDT_750ns   = (((uint8_t)0x05)<<POWERSTEP01_TDT_SHIFT),
    POWERSTEP01_TDT_875ns   = (((uint8_t)0x06)<<POWERSTEP01_TDT_SHIFT),
    POWERSTEP01_TDT_1000ns  = (((uint8_t)0x07)<<POWERSTEP01_TDT_SHIFT),
    POWERSTEP01_TDT_1125ns  = (((uint8_t)0x08)<<POWERSTEP01_TDT_SHIFT),
    POWERSTEP01_TDT_1250ns  = (((uint8_t)0x09)<<POWERSTEP01_TDT_SHIFT),
    POWERSTEP01_TDT_1375ns  = (((uint8_t)0x0A)<<POWERSTEP01_TDT_SHIFT),
    POWERSTEP01_TDT_1500ns  = (((uint8_t)0x0B)<<POWERSTEP01_TDT_SHIFT),
    POWERSTEP01_TDT_1625ns  = (((uint8_t)0x0C)<<POWERSTEP01_TDT_SHIFT),
    POWERSTEP01_TDT_1750ns  = (((uint8_t)0x0D)<<POWERSTEP01_TDT_SHIFT),
    POWERSTEP01_TDT_1875ns  = (((uint8_t)0x0E)<<POWERSTEP01_TDT_SHIFT),
    POWERSTEP01_TDT_2000ns  = (((uint8_t)0x0F)<<POWERSTEP01_TDT_SHIFT),
    POWERSTEP01_TDT_2125ns  = (((uint8_t)0x10)<<POWERSTEP01_TDT_SHIFT),
    POWERSTEP01_TDT_2250ns  = (((uint8_t)0x11)<<POWERSTEP01_TDT_SHIFT),
    POWERSTEP01_TDT_2375ns  = (((uint8_t)0x12)<<POWERSTEP01_TDT_SHIFT),
    POWERSTEP01_TDT_2500ns  = (((uint8_t)0x13)<<POWERSTEP01_TDT_SHIFT),
    POWERSTEP01_TDT_2625ns  = (((uint8_t)0x14)<<POWERSTEP01_TDT_SHIFT),
    POWERSTEP01_TDT_2750ns  = (((uint8_t)0x15)<<POWERSTEP01_TDT_SHIFT),
    POWERSTEP01_TDT_2875ns  = (((uint8_t)0x16)<<POWERSTEP01_TDT_SHIFT),
    POWERSTEP01_TDT_3000ns  = (((uint8_t)0x17)<<POWERSTEP01_TDT_SHIFT),
    POWERSTEP01_TDT_3125ns  = (((uint8_t)0x18)<<POWERSTEP01_TDT_SHIFT),
    POWERSTEP01_TDT_3250ns  = (((uint8_t)0x19)<<POWERSTEP01_TDT_SHIFT),
    POWERSTEP01_TDT_3375ns  = (((uint8_t)0x1A)<<POWERSTEP01_TDT_SHIFT),
    POWERSTEP01_TDT_3500ns  = (((uint8_t)0x1B)<<POWERSTEP01_TDT_SHIFT),
    POWERSTEP01_TDT_3625ns  = (((uint8_t)0x1C)<<POWERSTEP01_TDT_SHIFT),
    POWERSTEP01_TDT_3750ns  = (((uint8_t)0x1D)<<POWERSTEP01_TDT_SHIFT),
    POWERSTEP01_TDT_3875ns  = (((uint8_t)0x1E)<<POWERSTEP01_TDT_SHIFT),
    POWERSTEP01_TDT_4000ns  = (((uint8_t)0x1F)<<POWERSTEP01_TDT_SHIFT)
} PS01_Tdt_t;

/* Masks for CONFIG register of Powerstep01 */
typedef enum {
    POWERSTEP01_CONFIG_OSC_SEL      = ((uint16_t)0x0007),
    POWERSTEP01_CONFIG_EXT_CLK      = ((uint16_t)0x0008),
    POWERSTEP01_CONFIG_SW_MODE      = ((uint16_t)0x0010),
    POWERSTEP01_CONFIG_OC_SD        = ((uint16_t)0x0080),
    POWERSTEP01_CONFIG_UVLOVAL      = ((uint16_t)0x0100),
    POWERSTEP01_CONFIG_VCCVAL       = ((uint16_t)0x0200),
    // Masks specific for voltage mode
    POWERSTEP01_CONFIG_EN_VSCOMP    = ((uint16_t)0x0020),
    POWERSTEP01_CONFIG_F_PWM_DEC    = ((uint16_t)0x1C00),
    POWERSTEP01_CONFIG_F_PWM_INT    = ((uint16_t)0xE000),
    // Masks specific for current mode
    POWERSTEP01_CONFIG_TSW          = ((uint16_t)0x7C00),
    POWERSTEP01_CONFIG_PRED_EN      = ((uint16_t)0x8000)  
} PS01_ConfigMasks_t;

/* Masks for CONFIG register of Powerstep01  (specific for current mode) */
#define POWERSTEP01_CONFIG_EN_TQREG (POWERSTEP01_CONFIG_EN_VSCOMP) 

/* Oscillator management (EXT_CLK and OSC_SEL fields of CONFIG register of Powerstep01) */
typedef enum {
    POWERSTEP01_CONFIG_INT_16MHZ               = ((uint16_t)0x0000),
    POWERSTEP01_CONFIG_INT_16MHZ_OSCOUT_2MHZ   = ((uint16_t)0x0008),
    POWERSTEP01_CONFIG_INT_16MHZ_OSCOUT_4MHZ   = ((uint16_t)0x0009),
    POWERSTEP01_CONFIG_INT_16MHZ_OSCOUT_8MHZ   = ((uint16_t)0x000A),
    POWERSTEP01_CONFIG_INT_16MHZ_OSCOUT_16MHZ  = ((uint16_t)0x000B),
    POWERSTEP01_CONFIG_EXT_8MHZ_XTAL_DRIVE     = ((uint16_t)0x0004),
    POWERSTEP01_CONFIG_EXT_16MHZ_XTAL_DRIVE    = ((uint16_t)0x0005),
    POWERSTEP01_CONFIG_EXT_24MHZ_XTAL_DRIVE    = ((uint16_t)0x0006),
    POWERSTEP01_CONFIG_EXT_32MHZ_XTAL_DRIVE    = ((uint16_t)0x0007),
    POWERSTEP01_CONFIG_EXT_8MHZ_OSCOUT_INVERT  = ((uint16_t)0x000C),
    POWERSTEP01_CONFIG_EXT_16MHZ_OSCOUT_INVERT = ((uint16_t)0x000D),
    POWERSTEP01_CONFIG_EXT_24MHZ_OSCOUT_INVERT = ((uint16_t)0x000E),
    POWERSTEP01_CONFIG_EXT_32MHZ_OSCOUT_INVERT = ((uint16_t)0x000F)
} PS01_ConfigOscMgmt_t;

/* Oscillator management (EXT_CLK and OSC_SEL fields of CONFIG register of Powerstep01) */
typedef enum {
    POWERSTEP01_CONFIG_SW_HARD_STOP = ((uint16_t)0x0000),
    POWERSTEP01_CONFIG_SW_USER      = ((uint16_t)0x0010)
} PS01_ConfigSwMode_t;

/* Voltage supply compensation enabling for voltage mode (EN_VSCOMP field of CONFIG register of Powerstep01) */
typedef enum {
    POWERSTEP01_CONFIG_VS_COMP_DISABLE  = ((uint16_t)0x0000),
    POWERSTEP01_CONFIG_VS_COMP_ENABLE   = ((uint16_t)0x0020)
} PS01_ConfigEnVscomp_t;

/* External torque regulation enabling (EN_TQREG field of CONFIG register of Powerstep01) */
typedef enum {
    POWERSTEP01_CONFIG_TQ_REG_TVAL_USED = ((uint16_t)0x0000),
    POWERSTEP01_CONFIG_TQ_REG_ADC_OUT   = ((uint16_t)0x0020)
} PS01_ConfigEnTqReg_t;

/* Overcurrent shutdown (OC_SD field of CONFIG register of Powerstep01) */
typedef enum {
    POWERSTEP01_CONFIG_OC_SD_DISABLE  = ((uint16_t)0x0000),
    POWERSTEP01_CONFIG_OC_SD_ENABLE   = ((uint16_t)0x0080)
} PS01_ConfigOcSd_t;

/* UVLO thresholds (UVLOVAL field of CONFIG register of Powerstep01) */
typedef enum {
    POWERSTEP01_CONFIG_UVLOVAL_LOW      = ((uint16_t)0x0000),
    POWERSTEP01_CONFIG_UVLOVAL_HIGH     = ((uint16_t)0x0100),
} PS01_ConfigUvLoVal_t;

/* Vcc voltage  (VCCVAL field of CONFIG register of Powerstep01) */
typedef enum {
    POWERSTEP01_CONFIG_VCCVAL_7_5V    = ((uint16_t)0x0000),
    POWERSTEP01_CONFIG_VCCVAL_15V     = ((uint16_t)0x0200)
} PS01_ConfigVccVal_t;

/* PWM frequency division factor (F_PWM_INT field of CONFIG register of Powerstep01) */
typedef enum {
    POWERSTEP01_CONFIG_PWM_DIV_1    = (((uint16_t)0x00)<<POWERSTEP01_CONFIG_PWM_DIV_SHIFT),
    POWERSTEP01_CONFIG_PWM_DIV_2    = (((uint16_t)0x01)<<POWERSTEP01_CONFIG_PWM_DIV_SHIFT),
    POWERSTEP01_CONFIG_PWM_DIV_3    = (((uint16_t)0x02)<<POWERSTEP01_CONFIG_PWM_DIV_SHIFT),
    POWERSTEP01_CONFIG_PWM_DIV_4    = (((uint16_t)0x03)<<POWERSTEP01_CONFIG_PWM_DIV_SHIFT),
    POWERSTEP01_CONFIG_PWM_DIV_5    = (((uint16_t)0x04)<<POWERSTEP01_CONFIG_PWM_DIV_SHIFT),
    POWERSTEP01_CONFIG_PWM_DIV_6    = (((uint16_t)0x05)<<POWERSTEP01_CONFIG_PWM_DIV_SHIFT),
    POWERSTEP01_CONFIG_PWM_DIV_7    = (((uint16_t)0x06)<<POWERSTEP01_CONFIG_PWM_DIV_SHIFT)
} PS01_ConfigFPwmInt_t;

/* PWM frequency multiplication factor (F_PWM_DEC field of CONFIG register of Powerstep01) */
typedef enum {
    POWERSTEP01_CONFIG_PWM_MUL_0_625  = (((uint16_t)0x00)<<POWERSTEP01_CONFIG_PWM_MUL_SHIFT),
    POWERSTEP01_CONFIG_PWM_MUL_0_75   = (((uint16_t)0x01)<<POWERSTEP01_CONFIG_PWM_MUL_SHIFT),
    POWERSTEP01_CONFIG_PWM_MUL_0_875  = (((uint16_t)0x02)<<POWERSTEP01_CONFIG_PWM_MUL_SHIFT),
    POWERSTEP01_CONFIG_PWM_MUL_1      = (((uint16_t)0x03)<<POWERSTEP01_CONFIG_PWM_MUL_SHIFT),
    POWERSTEP01_CONFIG_PWM_MUL_1_25   = (((uint16_t)0x04)<<POWERSTEP01_CONFIG_PWM_MUL_SHIFT),
    POWERSTEP01_CONFIG_PWM_MUL_1_5    = (((uint16_t)0x05)<<POWERSTEP01_CONFIG_PWM_MUL_SHIFT),
    POWERSTEP01_CONFIG_PWM_MUL_1_75   = (((uint16_t)0x06)<<POWERSTEP01_CONFIG_PWM_MUL_SHIFT),
    POWERSTEP01_CONFIG_PWM_MUL_2      = (((uint16_t)0x07)<<POWERSTEP01_CONFIG_PWM_MUL_SHIFT)
} PS01_ConfigFPwmDec_t;

/* Switching period  (TSW field of CONFIG register of Powerstep01) */
typedef enum {
    POWERSTEP01_CONFIG_TSW_004us    =(((uint16_t)0x01)<<POWERSTEP01_CONFIG_TSW_SHIFT),
    POWERSTEP01_CONFIG_TSW_008us    =(((uint16_t)0x02)<<POWERSTEP01_CONFIG_TSW_SHIFT),
    POWERSTEP01_CONFIG_TSW_012us    =(((uint16_t)0x03)<<POWERSTEP01_CONFIG_TSW_SHIFT),
    POWERSTEP01_CONFIG_TSW_016us    =(((uint16_t)0x04)<<POWERSTEP01_CONFIG_TSW_SHIFT),
    POWERSTEP01_CONFIG_TSW_020us    =(((uint16_t)0x05)<<POWERSTEP01_CONFIG_TSW_SHIFT),
    POWERSTEP01_CONFIG_TSW_024us    =(((uint16_t)0x06)<<POWERSTEP01_CONFIG_TSW_SHIFT),
    POWERSTEP01_CONFIG_TSW_028us    =(((uint16_t)0x07)<<POWERSTEP01_CONFIG_TSW_SHIFT),
    POWERSTEP01_CONFIG_TSW_032us    =(((uint16_t)0x08)<<POWERSTEP01_CONFIG_TSW_SHIFT),
    POWERSTEP01_CONFIG_TSW_036us    =(((uint16_t)0x09)<<POWERSTEP01_CONFIG_TSW_SHIFT),
    POWERSTEP01_CONFIG_TSW_040us    =(((uint16_t)0x0A)<<POWERSTEP01_CONFIG_TSW_SHIFT),
    POWERSTEP01_CONFIG_TSW_044us    =(((uint16_t)0x0B)<<POWERSTEP01_CONFIG_TSW_SHIFT),
    POWERSTEP01_CONFIG_TSW_048us    =(((uint16_t)0x0C)<<POWERSTEP01_CONFIG_TSW_SHIFT),
    POWERSTEP01_CONFIG_TSW_052us    =(((uint16_t)0x0D)<<POWERSTEP01_CONFIG_TSW_SHIFT),
    POWERSTEP01_CONFIG_TSW_056us    =(((uint16_t)0x0E)<<POWERSTEP01_CONFIG_TSW_SHIFT),
    POWERSTEP01_CONFIG_TSW_060us    =(((uint16_t)0x0F)<<POWERSTEP01_CONFIG_TSW_SHIFT),
    POWERSTEP01_CONFIG_TSW_064us    =(((uint16_t)0x10)<<POWERSTEP01_CONFIG_TSW_SHIFT),
    POWERSTEP01_CONFIG_TSW_068us    =(((uint16_t)0x11)<<POWERSTEP01_CONFIG_TSW_SHIFT),
    POWERSTEP01_CONFIG_TSW_072us    =(((uint16_t)0x12)<<POWERSTEP01_CONFIG_TSW_SHIFT),
    POWERSTEP01_CONFIG_TSW_076us    =(((uint16_t)0x13)<<POWERSTEP01_CONFIG_TSW_SHIFT),
    POWERSTEP01_CONFIG_TSW_080us    =(((uint16_t)0x14)<<POWERSTEP01_CONFIG_TSW_SHIFT),
    POWERSTEP01_CONFIG_TSW_084us    =(((uint16_t)0x15)<<POWERSTEP01_CONFIG_TSW_SHIFT),
    POWERSTEP01_CONFIG_TSW_088us    =(((uint16_t)0x16)<<POWERSTEP01_CONFIG_TSW_SHIFT),
    POWERSTEP01_CONFIG_TSW_092us    =(((uint16_t)0x17)<<POWERSTEP01_CONFIG_TSW_SHIFT),
    POWERSTEP01_CONFIG_TSW_096us    =(((uint16_t)0x18)<<POWERSTEP01_CONFIG_TSW_SHIFT),
    POWERSTEP01_CONFIG_TSW_100us    =(((uint16_t)0x19)<<POWERSTEP01_CONFIG_TSW_SHIFT),
    POWERSTEP01_CONFIG_TSW_104us    =(((uint16_t)0x1A)<<POWERSTEP01_CONFIG_TSW_SHIFT),
    POWERSTEP01_CONFIG_TSW_108us    =(((uint16_t)0x1B)<<POWERSTEP01_CONFIG_TSW_SHIFT),
    POWERSTEP01_CONFIG_TSW_112us    =(((uint16_t)0x1C)<<POWERSTEP01_CONFIG_TSW_SHIFT),
    POWERSTEP01_CONFIG_TSW_116us    =(((uint16_t)0x1D)<<POWERSTEP01_CONFIG_TSW_SHIFT),
    POWERSTEP01_CONFIG_TSW_120us    =(((uint16_t)0x1E)<<POWERSTEP01_CONFIG_TSW_SHIFT),
    POWERSTEP01_CONFIG_TSW_124us    =(((uint16_t)0x1F)<<POWERSTEP01_CONFIG_TSW_SHIFT)        
} PS01_ConfigTsw_t;

/* Voltage supply compensation enabling for current mode(EN_PRED field of CONFIG register of Powerstep01) */
typedef enum {
    POWERSTEP01_CONFIG_PRED_DISABLE =((uint16_t)0x0000),
    POWERSTEP01_CONFIG_PRED_ENABLE  =((uint16_t)0x8000)
} PS01_ConfigPredEn_t;

/* Bit mask for STATUS Register of PowerStep01 */
typedef enum {
    POWERSTEP01_STATUS_HIZ          = (((uint16_t)0x0001)),
    POWERSTEP01_STATUS_BUSY         = (((uint16_t)0x0002)),
    POWERSTEP01_STATUS_SW_F         = (((uint16_t)0x0004)),
    POWERSTEP01_STATUS_SW_EVN       = (((uint16_t)0x0008)),
    POWERSTEP01_STATUS_DIR          = (((uint16_t)0x0010)),
    POWERSTEP01_STATUS_MOT_STATUS   = (((uint16_t)0x0060)),
    POWERSTEP01_STATUS_CMD_ERROR    = (((uint16_t)0x0080)),
    POWERSTEP01_STATUS_STCK_MOD     = (((uint16_t)0x0100)),
    POWERSTEP01_STATUS_UVLO         = (((uint16_t)0x0200)),
    POWERSTEP01_STATUS_UVLO_ADC     = (((uint16_t)0x0400)),
    POWERSTEP01_STATUS_TH_STATUS    = (((uint16_t)0x1800)),
    POWERSTEP01_STATUS_OCD          = (((uint16_t)0x2000)),
    POWERSTEP01_STATUS_STALL_A      = (((uint16_t)0x4000)),
    POWERSTEP01_STATUS_STALL_B      = (((uint16_t)0x8000))
} PS01_StatusMasks_t;

/* Motor state (MOT_STATUS filed of STATUS register of PowerStep01) */
typedef enum {
    POWERSTEP01_STATUS_MOT_STATUS_STOPPED       = (((uint16_t)0x0000)<<POWERSTEP01_STATUS_MOT_STATUS_SHIFT),
    POWERSTEP01_STATUS_MOT_STATUS_ACCELERATION  = (((uint16_t)0x0001)<<POWERSTEP01_STATUS_MOT_STATUS_SHIFT),
    POWERSTEP01_STATUS_MOT_STATUS_DECELERATION  = (((uint16_t)0x0002)<<POWERSTEP01_STATUS_MOT_STATUS_SHIFT),
    POWERSTEP01_STATUS_MOT_STATUS_CONST_SPD     = (((uint16_t)0x0003)<<POWERSTEP01_STATUS_MOT_STATUS_SHIFT)
} PS01_Status_t;

typedef enum {
	STEP_MODE_FULL		= (uint8_t)0x00, 
	STEP_MODE_HALF   	= (uint8_t)0x01,
	STEP_MODE_1_4    	= (uint8_t)0x02,
	STEP_MODE_1_8    	= (uint8_t)0x03,
	STEP_MODE_1_16   	= (uint8_t)0x04,
	STEP_MODE_1_32   	= (uint8_t)0x05,
	STEP_MODE_1_64   	= (uint8_t)0x06,
	STEP_MODE_1_128  	= (uint8_t)0x07,
	STEP_MODE_1_256  	= (uint8_t)0x08,
	STEP_MODE_UNKNOW 	= (uint8_t)0xFE,
	STEP_MODE_WAVE   	= (uint8_t)0xFF  
} PS01_StepMode_t;

/* Powerstep01 command + argument bytes number for command */
#define POWERSTEP01_CMD_ARG_MAX_NB_BYTES              (4)
#define POWERSTEP01_CMD_ARG_NB_BYTES_NOP              (1)
#define POWERSTEP01_CMD_ARG_NB_BYTES_RUN              (4)
#define POWERSTEP01_CMD_ARG_NB_BYTES_STEP_CLOCK       (1)
#define POWERSTEP01_CMD_ARG_NB_BYTES_MOVE             (4)
#define POWERSTEP01_CMD_ARG_NB_BYTES_GO_TO            (4)
#define POWERSTEP01_CMD_ARG_NB_BYTES_GO_TO_DIR        (4)
#define POWERSTEP01_CMD_ARG_NB_BYTES_GO_UNTIL         (4)
#define POWERSTEP01_CMD_ARG_NB_BYTES_RELEASE_SW       (1)
#define POWERSTEP01_CMD_ARG_NB_BYTES_GO_HOME          (1)
#define POWERSTEP01_CMD_ARG_NB_BYTES_GO_MARK          (1)
#define POWERSTEP01_CMD_ARG_NB_BYTES_RESET_POS        (1)
#define POWERSTEP01_CMD_ARG_NB_BYTES_RESET_DEVICE     (1)
#define POWERSTEP01_CMD_ARG_NB_BYTES_SOFT_STOP        (1)
#define POWERSTEP01_CMD_ARG_NB_BYTES_HARD_STOP        (1)
#define POWERSTEP01_CMD_ARG_NB_BYTES_SOFT_HIZ         (1)
#define POWERSTEP01_CMD_ARG_NB_BYTES_HARD_HIZ         (1)
#define POWERSTEP01_CMD_ARG_NB_BYTES_GET_STATUS       (1)

/* Powerstep01 response bytes number */
#define POWERSTEP01_RSP_NB_BYTES_GET_STATUS (2)    
/* Daisy chain command mask */
#define DAISY_CHAIN_COMMAND_MASK (0xFA)
/* powerSTEP01 max absolute position */
#define POWERSTEP01_MAX_POSITION (int32_t)(0x001FFFFF)
/* powerSTEP01 min absolute position*/
#define POWERSTEP01_MIN_POSITION (int32_t)(0xFFE00000)

typedef enum {
    POWERSTEP01_NOP               = ((uint8_t)0x00),
    POWERSTEP01_SET_PARAM         = ((uint8_t)0x00),
    POWERSTEP01_GET_PARAM         = ((uint8_t)0x20),
    POWERSTEP01_RUN               = ((uint8_t)0x50),
    POWERSTEP01_STEP_CLOCK        = ((uint8_t)0x58),
    POWERSTEP01_MOVE              = ((uint8_t)0x40),
    POWERSTEP01_GO_TO             = ((uint8_t)0x60),
    POWERSTEP01_GO_TO_DIR         = ((uint8_t)0x68),
    POWERSTEP01_GO_UNTIL          = ((uint8_t)0x82),
    POWERSTEP01_GO_UNTIL_ACT_CPY  = ((uint8_t)0x8A),
    POWERSTEP01_RELEASE_SW        = ((uint8_t)0x92),
    POWERSTEP01_GO_HOME           = ((uint8_t)0x70),
    POWERSTEP01_GO_MARK           = ((uint8_t)0x78),
    POWERSTEP01_RESET_POS         = ((uint8_t)0xD8),
    POWERSTEP01_RESET_DEVICE      = ((uint8_t)0xC0),
    POWERSTEP01_SOFT_STOP         = ((uint8_t)0xB0),
    POWERSTEP01_HARD_STOP         = ((uint8_t)0xB8),
    POWERSTEP01_SOFT_HIZ          = ((uint8_t)0xA0),
    POWERSTEP01_HARD_HIZ          = ((uint8_t)0xA8),
    POWERSTEP01_GET_STATUS        = ((uint8_t)0xD0),
    POWERSTEP01_RESERVED_CMD1     = ((uint8_t)0xEB),
    POWERSTEP01_RESERVED_CMD2     = ((uint8_t)0xF8)
} PS01_Commands_t;

#ifdef __cplusplus
}
#endif