/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Project.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file powerSTEP01_param.h
 * @brief Drivers for powerSTEP01 component
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "PS01_Global.h"
#include "PS01_Register.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct {
    PS01_CmVm_t cmVmSelection;               // Current or voltage mode selection
    float acceleration;                             // Acceleration 
    float deceleration;                             // Deceleration
    float maxSpeed;                                 // Maximum speed
    float minSpeed;                                 // Minimum speed
    PS01_LspdOpt_t lowSpeedOptimization;     // Low speed optimization bit
    float fullStepSpeed;                            // Full step speed
    PS01_BoostMode_t boostMode;              // Boost mode bit
    float ocdThreshold;                             // Over current detection threshold
    PS01_StepMode_t stepMode;                // Step mode
    PS01_SyncSel_t syncClockSelection;       // Sync clock selection
    uint8_t alarmsSelection;                        // Alarm selection
    PS01_Igate_t iGate;                      // Sink or source current used by gate driving circuitry
    PS01_Tboost_t tBoost;                    // Duration of the overboost phase during gate turn-off
    PS01_Tcc_t tcc;                          // Duration of constant current phase during gate turn-on and turn-off
    PS01_WdEn_t wdEn;                        // Clock source monitoring enable bit
    PS01_TBlank_t tBlank;                    // Duration of the blanking of the current sensing comparators
    PS01_Tdt_t tdt;                          // Deadtime duration between gate turn-off and opposite gate turn-on
} PS01_Param_Common_t;

typedef struct {
    PS01_Param_Common_t cp;              // Parameters common to current and voltage modes  
    float kvalHold;                             // Voltage amplitude regulation when the motor is stopped  
    float kvalRun;                              // Voltage amplitude regulation when the motor is running at constant speed  
    float kvalAcc;                              // Voltage amplitude regulation during motor acceleration  
    float kvalDec;                              // Voltage amplitude regulation during motor deceleration  
    float intersectSpeed;                       // Speed value at which the BEMF compensation curve changes slope  
    float startSlope;                           // BEMF compensation curve slope when speed is lower than intersect speed  
    float accelerationFinalSlope;               // BEMF compensation curve slope when speed is greater than intersect speed during acceleration  
    float decelerationFinalSlope;               // BEMF compensation curve slope when speed is greater than intersect speed during deceleration  
    float thermalCompensationFactor;            // Winding resistance thermal drift compensation coefficient  
    float stallThreshold;                       // Stall detection threshold  
    PS01_ConfigOscMgmt_t oscClkSel;      // System clock source management  
    PS01_ConfigSwMode_t swMode;          // External switch to act as HardStop interrupt or not  
    PS01_ConfigEnVscomp_t enVsComp;      // Motor supply voltage compensation enable bit  
    PS01_ConfigOcSd_t ocSd;              // Overcurrent event causes or not the bridges to turn-off  
    PS01_ConfigUvLoVal_t uvloVal;        // UVLO protection thresholds  
    PS01_ConfigVccVal_t vccVal;          // Internal VCC regulator output voltage  
    PS01_ConfigFPwmInt_t fPwmInt;        // Integer division factor of PWM frequency generation  
    PS01_ConfigFPwmDec_t fPwmDec;        // Multiplication factor of PWM frequency generation
} PS01_Param_VM_t;

typedef struct {
    PS01_Param_Common_t cp;              // Parameters common to current and voltage modes
    float tvalHold;                             // Torque regulation DAC reference voltage when motor is stopped
    float tvalRun;                              // Torque regulation DAC reference voltage when motor is runnig at constant speed
    float tvalAcc;                              // Torque regulation DAC reference voltage during motor acceleration
    float tvalDec;                              // Torque regulation DAC reference voltage during motor deceleration
    PS01_ToffFast_t toffFast;            // Maximum fast decay time
    PS01_FastStep_t fastStep;            // Maximum fall step time
    float tonMin;                               // Minimum on-time
    float toffMin;                              // Minimum off-time
    PS01_ConfigOscMgmt_t oscClkSel;      // System clock source management
    PS01_ConfigSwMode_t swMode;          // External switch to act as HardStop interrupt or not
    PS01_ConfigEnTqReg_t tqReg;          // Peak current is adjusted through the ADCIN input or not
    PS01_ConfigEnVscomp_t enVsComp;      // Motor supply voltage compensation enable bit
    PS01_ConfigOcSd_t ocSd;              // Overcurrent event causes or not the bridges to turn-off
    PS01_ConfigUvLoVal_t uvloVal;        // UVLO protection thresholds
    PS01_ConfigVccVal_t vccVal;          // Internal VCC regulator output voltage
    PS01_ConfigTsw_t tsw;                // target switching period
    PS01_ConfigPredEn_t predEn;          // predictive current control method enable bit
} PS01_Param_CM_t;

/* Initialization parameters struct */
typedef struct { 
    union {
        PS01_Param_CM_t cm; // Current mode
        PS01_Param_VM_t vm; // Voltage mode
    };
} PS01_Param_t;

typedef struct {
    uint32_t min_speed:12;
    uint32_t lspd_opt:1;
    uint32_t reserverd:19;
} register_min_speed_t;

typedef struct {
    float min_speed;
    bool lspd_opt;
} min_speed_t;

typedef struct {
    uint32_t fs_spd:10;
    uint32_t boost_mode:1;
    uint32_t reserved:21;
} register_fs_spd_t;

typedef struct {
    float fs_spd;
    bool boost_mode;
} fs_spd_t;

typedef struct {
    uint32_t step_sel:3;
    uint32_t cm_vm:1;
    uint32_t sync_sel:3;
    uint32_t sync_en:1;
    uint32_t reserved:24;
} register_step_mode_t;

typedef struct {
    uint32_t overcurrent:1;
    uint32_t thermal_shutdown:1;
    uint32_t thermal_warning:1;
    uint32_t uvlo:1;
    uint32_t adc_uvlo:1;
    uint32_t stall_detection:1;
    uint32_t switch_turn_on:1;
    uint32_t command_error:1;
    uint32_t reserved:24;
} register_alarm_enabled_t;

typedef struct {
    uint32_t tcc:5;
    uint32_t igate:3;
    uint32_t tboost:3;
    uint32_t wd_en:1;
    uint32_t reserved:20;
} register_gate_cfg_1_t;

typedef struct {
    uint32_t tdt:5;
    uint32_t tblank:3;
    uint32_t reserved:24;
} register_gate_cfg_2_t;

typedef struct {
    uint32_t osc_sel:3;
    uint32_t ext_clk:1;
    uint32_t sw_mode:1;
    uint32_t en_vscomp:1;
    uint32_t reserved:1;
    uint32_t oc_sd:1;
    uint32_t uvloval:1;
    uint32_t vccval:1;
    uint32_t f_pwm_dec:3;
    uint32_t f_pwm_int:3;
    uint32_t reserved_2:16;    
} register_config_vm_t;

typedef struct {
    uint32_t osc_sel:3;
    uint32_t ext_clk:1;
    uint32_t sw_mode:1;
    uint32_t en_tqreg:1;
    uint32_t reserved:1;
    uint32_t oc_sd:1;
    uint32_t uvloval:1;
    uint32_t vccval:1;
    uint32_t tsw:5;
    uint32_t pred_en:1;
    uint32_t reserved_2:16;    
} register_config_cm_t;

typedef struct {
    union {
        register_config_vm_t vm; // voltage mode
        register_config_cm_t cm; // current mode
    };
} register_config_t;

typedef struct {
    uint32_t hiz:1;
    uint32_t busy:1;
    uint32_t sw_f:1;
    uint32_t sw_evn:1;
    uint32_t dir:1;
    uint32_t mot_status:2;
    uint32_t cmd_error:1;
    uint32_t stck_mod:1;
    uint32_t uvlo:1;
    uint32_t uvlo_adc:1;
    uint32_t th_status:2;
    uint32_t ocd:1;
    uint32_t stall_b:1;
    uint32_t stall_a:1;
    uint32_t reserved:16;
} register_status_t;

typedef struct {
    float toff_fast;
    float fast_step;
} tfast_t;


void PS01_Param_SetAll(uint8_t deviceId, PS01_Param_t *param);

/*------------*/
/* get params */
/*------------*/

/* general */
int32_t PS01_Param_GetAbsPos(uint8_t deviceId);
// float PS01_Param_GetElectricPos(uint8_t deviceId); // Not implemented
int32_t PS01_Param_GetMark(uint8_t deviceId);
float PS01_Param_GetSpeed(uint8_t deviceId);
float PS01_Param_GetAcc(uint8_t deviceId);
float PS01_Param_GetDec(uint8_t deviceId);
float PS01_Param_GetMaxSpeed(uint8_t deviceId);
min_speed_t PS01_Param_GetMinSpeed(uint8_t deviceId);
// float PS01_Param_GetAdcOut(uint8_t deviceId); // Not implemented
float PS01_Param_GetOCDThreshold(uint8_t deviceId);
fs_spd_t PS01_Param_GetFullStepSpeed(uint8_t deviceId);
register_step_mode_t PS01_Param_GetStepMode(uint8_t deviceId);
bool PS01_Param_GetBusyStatus(uint8_t deviceId);
register_alarm_enabled_t PS01_Param_GetAlarmEnabled(uint8_t deviceId);
register_gate_cfg_1_t PS01_Param_GetGateDriverConfig1(uint8_t deviceId);
register_gate_cfg_2_t PS01_Param_GetGateDriverConfig2(uint8_t deviceId);
register_status_t PS01_Param_GetStatus(uint8_t deviceId);
register_config_t PS01_Param_GetConfig(uint8_t deviceId);

/* voltage mode*/
float PS01_Param_GetKvalHold(uint8_t deviceId);
float PS01_Param_GetKvalRun(uint8_t deviceId);
float PS01_Param_GetKvalAcc(uint8_t deviceId);
float PS01_Param_GetKvalDec(uint8_t deviceId);
float PS01_Param_GetIntersectSpeed(uint8_t deviceId);
float PS01_Param_GetStartSlope(uint8_t deviceId);
float PS01_Param_GetAccFinalSlope(uint8_t deviceId);
float PS01_Param_GetDecFinalSlope(uint8_t deviceId);
float PS01_Param_GetThermalCompensationFactor(uint8_t deviceId);
float PS01_Param_GetStallThreshold(uint8_t deviceId);

/* current mode*/
float PS01_Param_GetTvalHold(uint8_t deviceId);
float PS01_Param_GetTvalRun(uint8_t deviceId);
float PS01_Param_GetTvalAcc(uint8_t deviceId);
float PS01_Param_GetTvalDec(uint8_t deviceId);
tfast_t PS01_Param_GetTFast(uint8_t deviceId);
float PS01_Param_GetTOnMin(uint8_t deviceId);
float PS01_Param_GetToffMin(uint8_t deviceId);

/*------------*/
/* set params */
/*------------*/

/* general */
int PS01_Param_SetAbsPos(uint8_t deviceId, int32_t pos);
int PS01_Param_SetElectricPos(uint8_t deviceId, uint16_t pos); 
int PS01_Param_SetMark(uint8_t deviceId, int32_t mark);
int PS01_Param_SetAcc(uint8_t deviceId, float acc);
int PS01_Param_SetDec(uint8_t deviceId, float acc);
int PS01_Param_SetMaxSpeed(uint8_t deviceId, float speed);
int PS01_Param_SetMinSpeed(uint8_t deviceId, min_speed_t speed);
int PS01_Param_SetOCDThreshold(uint8_t deviceId, float threshold);
int PS01_Param_SetFullStepSpeed(uint8_t deviceId, fs_spd_t fs_spd);
void PS01_Param_SetStepMode(uint8_t deviceId, PS01_StepMode_t stepMode);
int PS01_Param_SetStepModeComplete(uint8_t deviceId, register_step_mode_t stepMode);
int PS01_Param_SetAlarmEnabled(uint8_t deviceId, register_alarm_enabled_t alarm_enabled);
int PS01_Param_SetGateDriverConfig1(uint8_t deviceId, register_gate_cfg_1_t config);
int PS01_Param_SetGateDriverConfig2(uint8_t deviceId, register_gate_cfg_2_t config);
int PS01_Param_SetConfig(uint8_t deviceId, register_config_t config);

/* voltage mode*/
int PS01_Param_SetKvalHold(uint8_t deviceId, float kval);
int PS01_Param_SetKvalRun(uint8_t deviceId, float kval);
int PS01_Param_SetKvalAcc(uint8_t deviceId, float kval);
int PS01_Param_SetKvalDec(uint8_t deviceId, float kval);
int PS01_Param_SetIntersectSpeed(uint8_t deviceId, float speed);
int PS01_Param_SetStartSlope(uint8_t deviceId, float start);
int PS01_Param_SetAccFinalSlope(uint8_t deviceId, float acc);
int PS01_Param_SetDecFinalSlope(uint8_t deviceId, float dec);
int PS01_Param_SetThermalCompensationFactor(uint8_t deviceId, float compFactor);
int PS01_Param_SetStallThreshold(uint8_t deviceId, float threshold);

/*current mode*/
int PS01_Param_SetTvalHold(uint8_t deviceId, float tval);
int PS01_Param_SetTvalRun(uint8_t deviceId, float tval);
int PS01_Param_SetTvalAcc(uint8_t deviceId, float tval);
int PS01_Param_SetTvalDec(uint8_t deviceId, float tval);
int PS01_Param_SetTFast(uint8_t deviceId, tfast_t tfast);
int PS01_Param_SetTOnMin(uint8_t deviceId, float mini);
int PS01_Param_SetTOffMin(uint8_t deviceId, float mini);

bool PS01_IsActive(uint8_t deviceId);

#ifdef __cplusplus
}
#endif