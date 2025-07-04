/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file MotorStepperParam.cpp
 * @brief 
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "MotorStepperParam.h"
#include "esp_partition.h"
#include "nvs_flash.h"

/* NVS parameters */
const char* MOTOR_STEPPER_NVS_KEY[] = {"PARA1","PARA2"};
const char* MOTOR_STEPPER_NVS_PARTITION = "nvs";
const char* MOTOR_STEPPER_NVS_NAMESPACE = "powerSTEP01";

// static const char MOTOR_STEPPER_TAG[] = "MotorStepperParam";

int MotorStepperParam::setAdvancedParam(MotorNum_t motor, AdvancedParameter_t advParam, void* value)
{
    PS01_AdvancedParam_t param;
    param = MotorStepperParam::getNVSParam(motor);
    switch(advParam){
        case ABS_POS:
        {
            param.absPos = *((int32_t*) value);
            break;
        }
        case MARK:
        {
            param.mark = *((int32_t*) value);
            break;
        }
        case ACCELERATION:
        {
            param.acc = *((float*) value);
            break;
        }
        case DECELERATION:
        {
            param.dec = *((float*) value);
            break;
        }
        case MAX_SPEED:
        {
            param.maxSpeed = *((float*) value);
            break;
        }
        case MIN_SPEED:
        {
            param.minSpeed = *((float*) value);
            break;
        }
        case MIN_SPEED_LSPD_OPT:
        {
            param.minSpeedLspdOpt = *((bool*) value);
            break;
        }
        case OCD_TH:
        {
            param.ocdTh = *((float*) value);
            break;
        }
        case FS_SPD:
        {
            param.fsSpd = *((float*) value);
            break;
        }
        case FS_SPD_BOOST_MODE:
        {
            param.fsSpdBoostMode = *((bool*) value);
            break;
        }
        case STEP_MODE_STEP_SEL:
        {
            param.stepModeStepSel = *((uint8_t*) value);
            break;
        }
        case STEP_MODE_CM_VM:
        {
            param.stepModeCmVm = *((uint8_t*) value);
            break;
        }
        case STEP_MODE_SYNC_SEL:
        {
            param.stepModeSyncSel = *((uint8_t*) value);
            break;
        }
        case STEP_MODE_SYNC_EN:
        {
            param.stepModeSyncEn = *((bool*) value);
            break;
        }   
        case ALARM_EN_OVERCURRENT:
        {
            param.alarmEnOvercurrent = *((bool*) value);
            break;
        }
        case ALARM_EN_THERMAL_SHUTDOWN:
        {
            param.alarmEnThermalShutdown = *((bool*) value);
            break;
        }
        case ALARM_EN_THERMAL_WARNING:
        {
            param.alarmEnThermalWarning = *((bool*) value);
            break;
        }
        case ALARM_EN_UVLO:
        {
            param.alarmEnUvlo = *((bool*) value);
            break;
        }
        case ALARM_EN_ADC_UVLO:
        {
            param.alarmEnAdcUvlo = *((bool*) value);
            break;
        }
        case ALARM_EN_STALL_DETECTION:
        {
            param.alarmEnStallDetection = *((bool*) value);
            break;
        }
        case ALARM_EN_SW_TURN_ON:
        {
            param.alarmEnSwTurnOn = *((bool*) value);
            break;
        }
        case ALARM_EN_COMMAND_ERROR:
        {
            param.alarmEnCommandError = *((bool*) value);
            break;
        }
        case GATECFG1_TCC:
        {
            param.gateCfg1Tcc = *((uint8_t*) value);
            break;
        }
        case GATECFG1_IGATE:
        {
            param.gateCfg1Igate = *((uint8_t*) value);
            break;
        }
        case GATECFG1_TBOOST:
        {
            param.gateCfg1Tboost = *((uint8_t*) value);
            break;
        }
        case GATECFG1_WD_EN:
        {
            param.gateCfg1WdEn = *((bool*) value);
            break;
        }
        case GATECFG2_TDT:
        {
            param.gateCfg2Tdt = *((uint8_t*) value);
            break;
        }
        case GATECFG2_TBLANK:
        {
            param.gateCfg2Tblank = *((uint8_t*) value);
            break;
        }
        case CONFIG_OSC_SEL:
        {
            param.configOscSel = *((uint8_t*) value);
            break;
        }
        case CONFIG_EXT_CLK:
        {
            param.configExtClk = *((uint8_t*) value);
            break;
        }
        case CONFIG_SW_MODE:
        {
            param.configSwMode = *((uint8_t*) value);
            break;
        }
        case CONFIG_OC_SD:
        {
            param.configOcSd = *((bool*) value);
            break;
        }
        case CONFIG_UVLOVAL:
        {
            param.configUvloval = *((uint8_t*) value);
            break;
        }
        case CONFIG_VCCVAL:
        {
            param.configVccval = *((uint8_t*) value);
            break;
        }

#ifdef CONFIG_CURRENT_MODE
        case CM_CONFIG_EN_TQREG:
        {
            param.cmConfigEnTqreg = *((bool*) value);
            break;
        }
        case CM_CONFIG_TSW:
        {
            param.cmConfigTsw = *((uint8_t*) value);
            break;
        }
        case CM_CONFIG_PRED_EN:
        {
            param.cmConfigPredEn = *((bool*) value);
            break;
        }
        case CM_TVAL_HOLD:
        {
            param.cmTvalHold = *((float*) value);
            break;
        }
        case CM_TVAL_RUN:
        {
            param.cmTvalRun = *((float*) value);
            break;
        }
        case CM_TVAL_ACC:
        {
            param.cmTvalAcc = *((float*) value);
            break;
        }
        case CM_TVAL_DEC:
        {
            param.cmTvalDec = *((float*) value);
            break;
        }
        case CM_TFAST_FAST_STEP:
        {
            param.cmTfastFastStep = *((uint8_t*) value);
            break;
        }
        case CM_TFAST_TOFF_FAST:
        {
            param.cmTfastToffFast = *((uint8_t*) value);
            break;
        }
        case CM_TON_MIN:
        {
            param.cmTonMin = *((float*) value);
            break;
        }
        case CM_TOFF_MIN:
        {
            param.cmToffMin = *((float*) value);
            break;
        }
#else
        case VM_CONFIG_EN_VSCOMP:
        {
            param.vmConfigEnVscomp = *((uint8_t*) value);
            break;
        }
        case VM_CONFIG_F_PWM_DEC:
        {
            param.vmConfigFPwmDec = *((uint8_t*) value);
            break;
        }
        case VM_CONFIG_F_PWM_INT:
        {
            param.vmConfigFPwmInt = *((uint8_t*) value);
            break;
        }
        case VM_KVAL_HOLD:
        {
            param.vmKvalHold = *((float*) value);
            break;
        }
        case VM_KVAL_RUN:
        {
            param.vmKvalRun = *((float*) value);
            break;
        }
        case VM_KVAL_ACC:
        {
            param.vmKvalAcc = *((float*) value);
            break;
        }
        case VM_KVAL_DEC:
        {
            param.vmKvalDec = *((float*) value);
            break;
        }
        case VM_INT_SPEED:
        {
            param.vmIntSpeed = *((float*) value);
            break;
        }
        case VM_ST_SLP:
        {
            param.vmStSlp = *((float*) value);
            break;
        }
        case VM_FN_SLP_ACC:
        {
            param.vmFnSlpAcc = *((float*) value);
            break;
        }
        case VM_FN_SLP_DEC:
        {
            param.vmFnSlpDec = *((float*) value);
            break;
        }
        case VM_K_THERM:
        {
            param.vmKTherm = *((float*) value);
            break;
        }
        case VM_STALL_TH:
        {
            param.vmStallTh = *((float*) value);
            break;
        }
        default:
        {
            return 1;
        }
#endif
    }
    // Write the parameter to the device
    int errorCode = MotorStepperParam::setAdvancedParamWithoutNvs(motor, advParam, value);
    
    // If the write was successful, update the NVS
    if (errorCode == 0)
    {
        errorCode |= setNVSParam(motor, param);
    }
    return errorCode;
}

int MotorStepperParam::setAdvancedParamWithoutNvs(MotorNum_t motor, AdvancedParameter_t advParam, void* value)
{
    int errorCode = 0;
    switch (advParam)
    {
    case ABS_POS:
    {
        errorCode = PS01_Param_SetAbsPos(motor, *((int32_t*) value));
        break;
    }
    case MARK:
    {
        errorCode = PS01_Param_SetMark(motor, *((int32_t*) value));
        break;
    }
    case ACCELERATION:
    {
        errorCode = PS01_Param_SetAcc(motor, *((float*) value));
        break;
    }
    case DECELERATION:
    {
        errorCode = PS01_Param_SetDec(motor,  *((float*) value));
        break;
    }
    case MAX_SPEED:
    {
        errorCode = PS01_Param_SetMaxSpeed(motor,  *((float*) value));
        break;
    }
    case MIN_SPEED:
    {
        errorCode = PS01_Param_SetMinSpeed(motor, {
            .min_speed=*((float*) value),
            .lspd_opt=PS01_Param_GetMinSpeed(motor).lspd_opt
            });
        break;
    }
    case MIN_SPEED_LSPD_OPT:
    {
        errorCode = PS01_Param_SetMinSpeed(motor, {
            .min_speed=PS01_Param_GetMinSpeed(motor).min_speed,
            .lspd_opt=*((bool*) value)
            });
        break;
    }
    case OCD_TH:
        errorCode = PS01_Param_SetOCDThreshold(motor, *((float*) value));
        break;
    case FS_SPD:
    {
        errorCode = PS01_Param_SetFullStepSpeed(motor, {
            .fs_spd=*((float *) value),
            .boost_mode=PS01_Param_GetFullStepSpeed(motor).boost_mode
        });
        break;
    }
    case FS_SPD_BOOST_MODE:
    {
        errorCode = PS01_Param_SetFullStepSpeed(motor, {
            .fs_spd=PS01_Param_GetFullStepSpeed(motor).fs_spd,
            .boost_mode=*((bool *) value)
        });
        break;
    }
    case STEP_MODE_STEP_SEL:
    {
        register_step_mode_t stpmd = PS01_Param_GetStepMode(motor);
        uint8_t stepsel = *((uint8_t *) value) & 7;
        errorCode = PS01_Param_SetStepModeComplete(motor,{
            .step_sel = stepsel,
            .cm_vm = stpmd.cm_vm,
            .sync_sel = stpmd.sync_sel,
            .sync_en = stpmd.sync_en,
            .reserved = 0
        });
        break;
    }
    case STEP_MODE_CM_VM:
    {
        register_step_mode_t stpmd1 = PS01_Param_GetStepMode(motor);
        bool cmvm = *((bool *) value) & 1;
        errorCode = PS01_Param_SetStepModeComplete(motor,
        {
            .step_sel = stpmd1.step_sel,
            .cm_vm = cmvm,
            .sync_sel = stpmd1.sync_sel,
            .sync_en = stpmd1.sync_en,
            .reserved = 0

        });
        break;
    }
    case STEP_MODE_SYNC_SEL:
    {
        register_step_mode_t stpmd2 = PS01_Param_GetStepMode(motor);
        uint8_t syncsel = *((uint8_t *) value) & 7;
        errorCode = PS01_Param_SetStepModeComplete(motor,{
            .step_sel = stpmd2.step_sel,
            .cm_vm = stpmd2.cm_vm,
            .sync_sel = syncsel,
            .sync_en = stpmd2.sync_en,
            .reserved = 0
        });
        break;
    }
    case STEP_MODE_SYNC_EN:
    {
        register_step_mode_t stpmd3 = PS01_Param_GetStepMode(motor);
        bool syncen = *((uint8_t *) value) & 1;
        errorCode = PS01_Param_SetStepModeComplete(motor,
        {
            .step_sel = stpmd3.step_sel,
            .cm_vm = stpmd3.cm_vm,
            .sync_sel = stpmd3.sync_sel,
            .sync_en = syncen,
            .reserved = 0
        });
        break;
    }
    case ALARM_EN_OVERCURRENT:
    {
        register_alarm_enabled_t alarm = PS01_Param_GetAlarmEnabled(motor);
        bool overcurrent = *((uint8_t *) value) & 1;
        errorCode = PS01_Param_SetAlarmEnabled(motor, {
            .overcurrent = overcurrent,
            .thermal_shutdown = alarm.thermal_shutdown,
            .thermal_warning = alarm.thermal_warning,
            .uvlo = alarm.uvlo,
            .adc_uvlo = alarm.adc_uvlo,
            .stall_detection = alarm.stall_detection,
            .switch_turn_on = alarm.switch_turn_on,
            .command_error = alarm.command_error,
            .reserved = 0
        });
        break;
    }
    case ALARM_EN_THERMAL_SHUTDOWN:
    {
        register_alarm_enabled_t alarm1 = PS01_Param_GetAlarmEnabled(motor);
        bool thermal_shutdown = *((uint8_t *) value) & 1;
        errorCode = PS01_Param_SetAlarmEnabled(motor, {
            .overcurrent = alarm1.overcurrent,
            .thermal_shutdown = thermal_shutdown,
            .thermal_warning = alarm1.thermal_warning,
            .uvlo = alarm1.uvlo,
            .adc_uvlo = alarm1.adc_uvlo,
            .stall_detection = alarm1.stall_detection,
            .switch_turn_on = alarm1.switch_turn_on,
            .command_error = alarm1.command_error,
            .reserved = 0
        });
        break;
    }
    case ALARM_EN_THERMAL_WARNING:
    {
        register_alarm_enabled_t alarm2 = PS01_Param_GetAlarmEnabled(motor);
        bool thermal_warning = *((uint8_t *) value) & 1;
        errorCode = PS01_Param_SetAlarmEnabled(motor, 
        {
            .overcurrent = alarm2.overcurrent,
            .thermal_shutdown = alarm2.thermal_shutdown,
            .thermal_warning = thermal_warning,
            .uvlo = alarm2.uvlo,
            .adc_uvlo = alarm2.adc_uvlo,
            .stall_detection = alarm2.stall_detection,
            .switch_turn_on = alarm2.switch_turn_on,
            .command_error = alarm2.command_error,
            .reserved = 0
        });
        break;
    }
    case ALARM_EN_UVLO:
    {
        register_alarm_enabled_t alarm3 = PS01_Param_GetAlarmEnabled(motor);
        bool uvlo = *((uint8_t *) value) & 1;
        errorCode = PS01_Param_SetAlarmEnabled(motor, {
            .overcurrent = alarm3.overcurrent,
            .thermal_shutdown = alarm3.thermal_shutdown,
            .thermal_warning = alarm3.thermal_warning,
            .uvlo = uvlo,
            .adc_uvlo = alarm3.adc_uvlo,
            .stall_detection = alarm3.stall_detection,
            .switch_turn_on = alarm3.switch_turn_on,
            .command_error = alarm3.command_error,
            .reserved = 0
        });
        break;
    }
    case ALARM_EN_ADC_UVLO:
    {
        register_alarm_enabled_t alarm4 = PS01_Param_GetAlarmEnabled(motor);
        bool adc_uvlo = *((uint8_t *) value) & 1;
        errorCode = PS01_Param_SetAlarmEnabled(motor, {
            .overcurrent = alarm4.overcurrent,
            .thermal_shutdown = alarm4.thermal_shutdown,
            .thermal_warning = alarm4.thermal_warning,
            .uvlo = alarm4.uvlo,
            .adc_uvlo = adc_uvlo,
            .stall_detection = alarm4.stall_detection,
            .switch_turn_on = alarm4.switch_turn_on,
            .command_error = alarm4.command_error,
            .reserved = 0
        
        });
        break;
    }
    case ALARM_EN_STALL_DETECTION:
    {
        register_alarm_enabled_t alarm5 = PS01_Param_GetAlarmEnabled(motor);
        bool stall_detection = *((uint8_t *) value) & 1;
        errorCode = PS01_Param_SetAlarmEnabled(motor, 
        {
            .overcurrent = alarm5.overcurrent,
            .thermal_shutdown = alarm5.thermal_shutdown,
            .thermal_warning = alarm5.thermal_warning,
            .uvlo = alarm5.uvlo,
            .adc_uvlo = alarm5.adc_uvlo,
            .stall_detection = stall_detection,
            .switch_turn_on = alarm5.switch_turn_on,
            .command_error = alarm5.command_error,
            .reserved = 0
        });
        break;
    }
    case ALARM_EN_SW_TURN_ON:
    {
        register_alarm_enabled_t alarm6 = PS01_Param_GetAlarmEnabled(motor);
        bool switch_turn_on = *((uint8_t *) value) & 1;
        errorCode = PS01_Param_SetAlarmEnabled(motor, {
            .overcurrent = alarm6.overcurrent,
            .thermal_shutdown = alarm6.thermal_shutdown,
            .thermal_warning = alarm6.thermal_warning,
            .uvlo = alarm6.uvlo,
            .adc_uvlo = alarm6.adc_uvlo,
            .stall_detection = alarm6.stall_detection,
            .switch_turn_on = switch_turn_on,
            .command_error = alarm6.command_error,
            .reserved = 0
        });
        break;
    }
    case ALARM_EN_COMMAND_ERROR:
    {
        register_alarm_enabled_t alarm7 = PS01_Param_GetAlarmEnabled(motor);
        bool command_error = *((uint8_t *) value) & 1;
        errorCode = PS01_Param_SetAlarmEnabled(motor, 
        {
            .overcurrent = alarm7.overcurrent,
            .thermal_shutdown = alarm7.thermal_shutdown,
            .thermal_warning = alarm7.thermal_warning,
            .uvlo = alarm7.uvlo,
            .adc_uvlo = alarm7.adc_uvlo,
            .stall_detection = alarm7.stall_detection,
            .switch_turn_on = alarm7.switch_turn_on,
            .command_error = command_error,
            .reserved = 0
        
        });
        break;
    }
    case GATECFG1_TCC:
    {
        register_gate_cfg_1_t cfg = PS01_Param_GetGateDriverConfig1(motor);
        uint8_t tcc = *((uint8_t *) value) & 31;
        errorCode = PS01_Param_SetGateDriverConfig1(motor, 
        {
            .tcc = tcc,
            .igate = cfg.igate,
            .tboost = cfg.tboost,
            .wd_en = cfg.wd_en,
            .reserved = 0
        });
        break;
    }
    case GATECFG1_IGATE:
    {
        register_gate_cfg_1_t cfg1 = PS01_Param_GetGateDriverConfig1(motor);
        uint8_t igate = *((uint8_t *) value) & 7;
        errorCode = PS01_Param_SetGateDriverConfig1(motor, 
        {
            .tcc = cfg1.tcc,
            .igate = igate,
            .tboost = cfg1.tboost,
            .wd_en = cfg1.wd_en,
            .reserved = 0
        });
        break;
    }
    case GATECFG1_TBOOST:
    {
        register_gate_cfg_1_t cfg2 = PS01_Param_GetGateDriverConfig1(motor);
        uint8_t tboost = *((uint8_t *) value) & 7;
        errorCode = PS01_Param_SetGateDriverConfig1(motor, 
        {
            .tcc = cfg2.tcc,
            .igate = cfg2.igate,
            .tboost = tboost,
            .wd_en = cfg2.wd_en,
            .reserved = 0
        
        });
        break;
    }
    case GATECFG1_WD_EN:
    {
        register_gate_cfg_1_t cfg3 = PS01_Param_GetGateDriverConfig1(motor);
        bool wd_en = *((uint8_t *) value) & 1;
        errorCode = PS01_Param_SetGateDriverConfig1(motor, 
        {
            .tcc = cfg3.tcc,
            .igate = cfg3.igate,
            .tboost = cfg3.tboost,
            .wd_en = wd_en,
            .reserved = 0
        
        });
        break;
    }
    case GATECFG2_TDT:
    {
        register_gate_cfg_2_t cfg21 = PS01_Param_GetGateDriverConfig2(motor);
        uint8_t tdt = *((uint8_t *) value) & 31;
        errorCode = PS01_Param_SetGateDriverConfig2(motor, {
            .tdt = tdt,
            .tblank = cfg21.tblank,
            .reserved = 0
        });
        break;
    }
    case GATECFG2_TBLANK:
    {
        register_gate_cfg_2_t cfg22 = PS01_Param_GetGateDriverConfig2(motor);
        uint8_t tblank = *((uint8_t *) value) & 7;
        errorCode = PS01_Param_SetGateDriverConfig2(motor, 
        {
            .tdt = cfg22.tdt,
            .tblank = tblank,
            .reserved = 0
        
        });
        break;
    }
    case CONFIG_OSC_SEL:
    {
        register_config_t config = PS01_Param_GetConfig(motor);
        uint8_t vm_osc_sel = *((uint8_t *) value) & 7;

        register_config_vm_t config_vm = {
            .osc_sel = vm_osc_sel,
            .ext_clk = config.vm.ext_clk,
            .sw_mode = config.vm.sw_mode,
            .en_vscomp = config.vm.en_vscomp,
            .reserved = 0,
            .oc_sd = config.vm.oc_sd,
            .uvloval = config.vm.uvloval,
            .vccval = config.vm.vccval,
            .f_pwm_dec = config.vm.f_pwm_dec,
            .f_pwm_int = config.vm.f_pwm_int,
            .reserved_2 = 0
        };
        config.vm = config_vm;

        errorCode = PS01_Param_SetConfig(motor, config);
        break;
    }
    case CONFIG_EXT_CLK:
    {
        register_config_t config1 = PS01_Param_GetConfig(motor);
        bool vm_ext_clk = *((uint8_t *) value) & 1;

        register_config_vm_t config_vm1 = {
            .osc_sel = config1.vm.osc_sel,
            .ext_clk = vm_ext_clk,
            .sw_mode = config1.vm.sw_mode,
            .en_vscomp = config1.vm.en_vscomp,
            .reserved = 0,
            .oc_sd = config1.vm.oc_sd,
            .uvloval = config1.vm.uvloval,
            .vccval = config1.vm.vccval,
            .f_pwm_dec = config1.vm.f_pwm_dec,
            .f_pwm_int = config1.vm.f_pwm_int,
            .reserved_2 = 0
        };
        config1.vm = config_vm1;

        errorCode = PS01_Param_SetConfig(motor, config1);
        break;
    }
    case CONFIG_SW_MODE:
    {
        register_config_t config2 = PS01_Param_GetConfig(motor);
        bool vm_sw_mode = *((uint8_t *) value) & 1;

        register_config_vm_t config_vm2 = {
            .osc_sel = config2.vm.osc_sel,
            .ext_clk = config2.vm.ext_clk,
            .sw_mode = vm_sw_mode,
            .en_vscomp = config2.vm.en_vscomp,
            .reserved = 0,
            .oc_sd = config2.vm.oc_sd,
            .uvloval = config2.vm.uvloval,
            .vccval = config2.vm.vccval,
            .f_pwm_dec = config2.vm.f_pwm_dec,
            .f_pwm_int = config2.vm.f_pwm_int,
            .reserved_2 = 0
        };
        config2.vm = config_vm2;

        errorCode = PS01_Param_SetConfig(motor, config2);
        break;
    }
    case CONFIG_OC_SD:
    {
        register_config_t config3 = PS01_Param_GetConfig(motor);
        bool vm_oc_sd = *((uint8_t *) value) & 1;

        register_config_vm_t config_vm3 = {
            .osc_sel = config3.vm.osc_sel,
            .ext_clk = config3.vm.ext_clk,
            .sw_mode = config3.vm.sw_mode,
            .en_vscomp = config3.vm.en_vscomp,
            .reserved = 0,
            .oc_sd = vm_oc_sd,
            .uvloval = config3.vm.uvloval,
            .vccval = config3.vm.vccval,
            .f_pwm_dec = config3.vm.f_pwm_dec,
            .f_pwm_int = config3.vm.f_pwm_int,
            .reserved_2 = 0
        };
        config3.vm = config_vm3;

        errorCode = PS01_Param_SetConfig(motor, config3);
        break;
    }
    case CONFIG_UVLOVAL:
    {
        register_config_t config4 = PS01_Param_GetConfig(motor);
        bool vm_uvloval = *((uint8_t *) value) & 1;

        register_config_vm_t config_vm4 = {
            .osc_sel = config4.vm.osc_sel,
            .ext_clk = config4.vm.ext_clk,
            .sw_mode = config4.vm.sw_mode,
            .en_vscomp = config4.vm.en_vscomp,
            .reserved = 0,
            .oc_sd = config4.vm.oc_sd,
            .uvloval = vm_uvloval,
            .vccval = config4.vm.vccval,
            .f_pwm_dec = config4.vm.f_pwm_dec,
            .f_pwm_int = config4.vm.f_pwm_int,
            .reserved_2 = 0
        };
        config4.vm = config_vm4;

        errorCode = PS01_Param_SetConfig(motor, config4);
        break;
    }
    case CONFIG_VCCVAL:
    {
        register_config_t config5 = PS01_Param_GetConfig(motor);
        bool vm_vccval = *((uint8_t *) value) & 1;

        register_config_vm_t config_vm5 = {
            .osc_sel = config5.vm.osc_sel,
            .ext_clk = config5.vm.ext_clk,
            .sw_mode = config5.vm.sw_mode,
            .en_vscomp = config5.vm.en_vscomp,
            .reserved = 0,
            .oc_sd = config5.vm.oc_sd,
            .uvloval = config5.vm.uvloval,
            .vccval = vm_vccval,
            .f_pwm_dec = config5.vm.f_pwm_dec,
            .f_pwm_int = config5.vm.f_pwm_int,
            .reserved_2 = 0
        };
        config5.vm = config_vm5;

        errorCode = PS01_Param_SetConfig(motor, config5);
        break;
    }
#ifdef CONFIG_CURRENT_MODE
    // TODO 
    case CM_CONFIG_EN_TQREG:
    case CM_CONFIG_TSW:
    case CM_CONFIG_PRED_EN:
    case CM_TVAL_HOLD:
    case CM_TVAL_RUN:
    case CM_TVAL_ACC:
    case CM_TVAL_DEC:
    case CM_TFAST_FAST_STEP:
    case CM_TFAST_TOFF_FAST:
    case CM_TON_MIN:
    case CM_TOFF_MIN:
        ESP_LOGE("MotorStepper", "not implemented !\n")
        errorCode = 2;
        break;

#else
    case VM_CONFIG_EN_VSCOMP:
    {
        register_config_t config6 = PS01_Param_GetConfig(motor);
        bool vm_en_vscomp = *((uint8_t *) value) & 1;

        register_config_vm_t config_vm6 = {
            .osc_sel = config6.vm.osc_sel,
            .ext_clk = config6.vm.ext_clk,
            .sw_mode = config6.vm.sw_mode,
            .en_vscomp = vm_en_vscomp,
            .reserved = 0,
            .oc_sd = config6.vm.oc_sd,
            .uvloval = config6.vm.uvloval,
            .vccval = config6.vm.vccval,
            .f_pwm_dec = config6.vm.f_pwm_dec,
            .f_pwm_int = config6.vm.f_pwm_int,
            .reserved_2 = 0
        };
        config6.vm = config_vm6;

        errorCode = PS01_Param_SetConfig(motor, config6);
        break;
    }
    case VM_CONFIG_F_PWM_DEC:
    {
        register_config_t config7 = PS01_Param_GetConfig(motor);
        uint8_t vm_f_pwm_dec = *((uint8_t *) value) & 7;

        register_config_vm_t config_vm_7 = {
            .osc_sel = config7.vm.osc_sel,
            .ext_clk = config7.vm.ext_clk,
            .sw_mode = config7.vm.sw_mode,
            .en_vscomp = config7.vm.en_vscomp,
            .reserved = 0,
            .oc_sd = config7.vm.oc_sd,
            .uvloval = config7.vm.uvloval,
            .vccval = config7.vm.vccval,
            .f_pwm_dec = vm_f_pwm_dec,
            .f_pwm_int = config7.vm.f_pwm_int,
            .reserved_2 = 0
        };
        config7.vm = config_vm_7;

        errorCode = PS01_Param_SetConfig(motor, config7);
        break;
    }
    case VM_CONFIG_F_PWM_INT:
    {
        register_config_t config8 = PS01_Param_GetConfig(motor);
        uint8_t vm_f_pwm_int = *((uint8_t *) value) & 7;

        register_config_vm_t config_vm_8 = {
            .osc_sel = config8.vm.osc_sel,
            .ext_clk = config8.vm.ext_clk,
            .sw_mode = config8.vm.sw_mode,
            .en_vscomp = config8.vm.en_vscomp,
            .reserved = 0,
            .oc_sd = config8.vm.oc_sd,
            .uvloval = config8.vm.uvloval,
            .vccval = config8.vm.vccval,
            .f_pwm_dec = config8.vm.f_pwm_dec,
            .f_pwm_int = vm_f_pwm_int,
            .reserved_2 = 0
        };
        config8.vm = config_vm_8;
        errorCode = PS01_Param_SetConfig(motor, config8);
        break;
    }
    case VM_KVAL_HOLD:
    {
        errorCode = PS01_Param_SetKvalHold(motor, *((float *) value));
        break;
    }
    case VM_KVAL_RUN:
    {
        errorCode = PS01_Param_SetKvalRun(motor, *((float *) value));
        break;
    }
    case VM_KVAL_ACC:
    {
        errorCode = PS01_Param_SetKvalAcc(motor, *((float *) value));
        break;
    }
    case VM_KVAL_DEC:
    {
        errorCode = PS01_Param_SetKvalDec(motor, *((float *) value));
        break;
    }
    case VM_INT_SPEED:
    {
        errorCode = PS01_Param_SetIntersectSpeed(motor, *((float *) value));
        break;
    }
    case VM_ST_SLP:
    {
        errorCode = PS01_Param_SetStartSlope(motor, *((float *) value));
        break;
    }
    case VM_FN_SLP_ACC:
    {
        errorCode = PS01_Param_SetAccFinalSlope(motor, *((float *) value));
        break;
    }
    case VM_FN_SLP_DEC:
    {
        errorCode = PS01_Param_SetDecFinalSlope(motor, *((float *) value));
        break;
    }
    case VM_K_THERM:
    {
        errorCode = PS01_Param_SetThermalCompensationFactor(motor, *((float *) value));
        break;
    }
    case VM_STALL_TH:
    {
        errorCode = PS01_Param_SetStallThreshold(motor, *((float *) value));
        break;
    }
#endif

    case ADC_OUT:
    case EL_POS_MICROSTEP:
    case EL_POS_STEP:
    default:
        errorCode = 1;
        break;
    }
    return errorCode;
}

int MotorStepperParam::getAdvancedParam(MotorNum_t motor, AdvancedParameter_t advParam, void* value)

{
    int errorCode = 0;
    switch(advParam) {
        case ABS_POS:
        {
            int32_t absPos = PS01_Param_GetAbsPos(motor);
            memcpy(value, &absPos, sizeof(absPos));
            break;
        }
        case EL_POS_MICROSTEP:
        case EL_POS_STEP:
        {
            errorCode = 1;
            break;
        }
        case MARK:
        {
            int32_t mark = PS01_Param_GetMark(motor);
            memcpy(value, &mark, sizeof(mark));
            break;
        }
        case SPEED:
        {
            float speed = PS01_Param_GetSpeed(motor);
            memcpy(value, &speed, sizeof(speed));
            break;
        }
        case ACCELERATION:
        {
            float acc = PS01_Param_GetAcc(motor);
            memcpy(value, &acc, sizeof(acc));
            break;
        }
        case DECELERATION:
        {
            float dec = PS01_Param_GetDec(motor);
            memcpy(value, &dec, sizeof(dec));
            break;
        }
        case MAX_SPEED:
        {
            float maxSpeed = PS01_Param_GetMaxSpeed(motor);
            memcpy(value, &maxSpeed, sizeof(maxSpeed));
            break;
        }
        case MIN_SPEED:
        {
            float minSpeed = PS01_Param_GetMinSpeed(motor).min_speed;
            memcpy(value, &minSpeed, sizeof(minSpeed));
            break;
        }
        case MIN_SPEED_LSPD_OPT:
        {
            bool minSpeedLspdOpt = PS01_Param_GetMinSpeed(motor).lspd_opt;
            memcpy(value, &minSpeedLspdOpt, sizeof(minSpeedLspdOpt));
            break;
        }
        case ADC_OUT:
        {
            errorCode = 1;
            break;
        }
        case OCD_TH:
        {
            float ocdTh = PS01_Param_GetOCDThreshold(motor);
            memcpy(value, &ocdTh, sizeof(ocdTh));
            break;
        }
        case FS_SPD:
        {
            float fsSpd = PS01_Param_GetFullStepSpeed(motor).fs_spd;
            memcpy(value, &fsSpd, sizeof(fsSpd));
            break;
        }
        case FS_SPD_BOOST_MODE:
        {
            bool fsSpdBoostMode = PS01_Param_GetFullStepSpeed(motor).boost_mode;
            memcpy(value, &fsSpdBoostMode, sizeof(fsSpdBoostMode));
            break;
        }
        case STEP_MODE_STEP_SEL:
        {
            uint8_t stepModeStepSel = PS01_Param_GetStepMode(motor).step_sel;
            memcpy(value, &stepModeStepSel, sizeof(stepModeStepSel));
            break;
        }
        case STEP_MODE_CM_VM:
        {
            uint8_t stepModeCmVm = PS01_Param_GetStepMode(motor).cm_vm;
            memcpy(value, &stepModeCmVm, sizeof(stepModeCmVm));
            break;
        }
        case STEP_MODE_SYNC_SEL:
        {
            uint8_t stepModeSyncSel = PS01_Param_GetStepMode(motor).sync_sel;
            memcpy(value, &stepModeSyncSel, sizeof(stepModeSyncSel));
            break;
        }
        case STEP_MODE_SYNC_EN:
        {
            uint8_t stepModeSyncEn = PS01_Param_GetStepMode(motor).sync_en;
            memcpy(value, &stepModeSyncEn, sizeof(stepModeSyncEn));
            break;
        }
        case ALARM_EN_OVERCURRENT:
        {
            uint8_t alarmEnOvercurrent = PS01_Param_GetAlarmEnabled(motor).overcurrent;
            memcpy(value, &alarmEnOvercurrent, sizeof(alarmEnOvercurrent));
            break;
        }
        case ALARM_EN_THERMAL_SHUTDOWN:
        {
            uint8_t alarmEnThermalShutdownn = PS01_Param_GetAlarmEnabled(motor).thermal_shutdown;
            memcpy(value, &alarmEnThermalShutdownn, sizeof(alarmEnThermalShutdownn));
            break;
        }
        case ALARM_EN_THERMAL_WARNING:
        {
            uint8_t alarmEnThermalWarning = PS01_Param_GetAlarmEnabled(motor).thermal_warning;
            memcpy(value, &alarmEnThermalWarning, sizeof(alarmEnThermalWarning));
            break;
        }
        case ALARM_EN_UVLO:
        {
            uint8_t alarmEnUvlo = PS01_Param_GetAlarmEnabled(motor).uvlo;
            memcpy(value, &alarmEnUvlo, sizeof(alarmEnUvlo));
            break;
        }
        case ALARM_EN_ADC_UVLO:
        {
            uint8_t alarmEnAdcUvlo = PS01_Param_GetAlarmEnabled(motor).adc_uvlo;
            memcpy(value, &alarmEnAdcUvlo, sizeof(alarmEnAdcUvlo));
            break;
        }
        case ALARM_EN_STALL_DETECTION:
        {
            uint8_t alarmEnStallDetection = PS01_Param_GetAlarmEnabled(motor).stall_detection;
            memcpy(value, &alarmEnStallDetection, sizeof(alarmEnStallDetection));
            break;
        }
        case ALARM_EN_SW_TURN_ON:
        {
            uint8_t alarmEnSwTurnOn = PS01_Param_GetAlarmEnabled(motor).switch_turn_on;
            memcpy(value, &alarmEnSwTurnOn, sizeof(alarmEnSwTurnOn));
            break;
        }
        case ALARM_EN_COMMAND_ERROR:
        {
            uint8_t alarmEnCommandError = PS01_Param_GetAlarmEnabled(motor).command_error;
            memcpy(value, &alarmEnCommandError, sizeof(alarmEnCommandError));
            break;
        }
        case GATECFG1_TCC:
        {
            uint8_t gatecfg1Tcc = PS01_Param_GetGateDriverConfig1(motor).tcc;
            memcpy(value, &gatecfg1Tcc, sizeof(gatecfg1Tcc));
            break;
        }
        case GATECFG1_IGATE:
        {
            uint8_t gatecfg1Igate = PS01_Param_GetGateDriverConfig1(motor).igate;
            memcpy(value, &gatecfg1Igate, sizeof(gatecfg1Igate));
            break;
        }
        case GATECFG1_TBOOST:
        {
            uint8_t gatecfg1Tboost = PS01_Param_GetGateDriverConfig1(motor).tboost;
            memcpy(value, &gatecfg1Tboost, sizeof(gatecfg1Tboost));
            break;
        }
        case GATECFG1_WD_EN:
        {
            uint8_t gatecfg1WdEn = PS01_Param_GetGateDriverConfig1(motor).wd_en;
            memcpy(value, &gatecfg1WdEn, sizeof(gatecfg1WdEn));
            break;
        }
        case GATECFG2_TDT:
        {
            uint8_t gatecfg2Tdt = PS01_Param_GetGateDriverConfig2(motor).tdt;
            memcpy(value, &gatecfg2Tdt, sizeof(gatecfg2Tdt));
            break;
        }
        case GATECFG2_TBLANK:
        {
            uint8_t gatecfg2Tblank = PS01_Param_GetGateDriverConfig2(motor).tblank;
            memcpy(value, &gatecfg2Tblank, sizeof(gatecfg2Tblank));
            break;
        }
#ifdef CONFIG_CURRENT_MODE
        case CM_CONFIG_EN_TQREG:
        case CM_CONFIG_TSW:
        case CM_CONFIG_PRED_EN:
        case CM_TVAL_HOLD:
        case CM_TVAL_RUN:
        case CM_TVAL_ACC:
        case CM_TVAL_DEC:
        case CM_TFAST_FAST_STEP:
        case CM_TFAST_TOFF_FAST:
        case CM_TON_MIN:
        case CM_TOFF_MIN:
        case CONFIG_OSC_SEL:
        case CONFIG_EXT_CLK:
        case CONFIG_SW_MODE:
        case CONFIG_OC_SD:
        case CONFIG_UVLOVAL:
        case CONFIG_VCCVAL:
        {
            errorCode = 1;
            break;
        }
#else
        case CONFIG_OSC_SEL:
        {
            uint8_t configOscSel = PS01_Param_GetConfig(motor).vm.osc_sel;
            memcpy(value, &configOscSel, sizeof(configOscSel));
            break;
        }
        case CONFIG_EXT_CLK:
        {
            uint8_t configExtClk = PS01_Param_GetConfig(motor).vm.ext_clk;
            memcpy(value, &configExtClk, sizeof(configExtClk));
            break;
        }
        case CONFIG_SW_MODE:
        {
            uint8_t configSwMode = PS01_Param_GetConfig(motor).vm.sw_mode;
            memcpy(value, &configSwMode, sizeof(configSwMode));
            break;
        }
        case CONFIG_OC_SD:
        {
            uint8_t configOcSd = PS01_Param_GetConfig(motor).vm.oc_sd;
            memcpy(value, &configOcSd, sizeof(configOcSd));
            break;
        }
        case CONFIG_UVLOVAL:
        {
            uint8_t configUvloval = PS01_Param_GetConfig(motor).vm.uvloval;
            memcpy(value, &configUvloval, sizeof(configUvloval));
            break;
        }
        case CONFIG_VCCVAL:
        {
            uint8_t configVccval = PS01_Param_GetConfig(motor).vm.vccval;
            memcpy(value, &configVccval, sizeof(configVccval));
            break;
        }
        case VM_CONFIG_EN_VSCOMP:
        {
            uint8_t vmConfigEnVscomp = PS01_Param_GetConfig(motor).vm.en_vscomp;
            memcpy(value, &vmConfigEnVscomp, sizeof(vmConfigEnVscomp));
            break;
        }
        case VM_CONFIG_F_PWM_DEC:
        {
            uint8_t vmConfigFPwmDec = PS01_Param_GetConfig(motor).vm.f_pwm_dec;
            memcpy(value, &vmConfigFPwmDec, sizeof(vmConfigFPwmDec));
            break;
        }
        case VM_CONFIG_F_PWM_INT:
        {
            uint8_t vmConfigFPwmInt = PS01_Param_GetConfig(motor).vm.f_pwm_int;
            memcpy(value, &vmConfigFPwmInt, sizeof(vmConfigFPwmInt));
            break;
        }
        case VM_KVAL_HOLD:
        {
            float vmKvalHold = PS01_Param_GetKvalHold(motor);
            memcpy(value, &vmKvalHold, sizeof(vmKvalHold));
            break;
        }
        case VM_KVAL_RUN:
        {
            float vmKvalRun = PS01_Param_GetKvalRun(motor);
            memcpy(value, &vmKvalRun, sizeof(vmKvalRun));
            break;
        }
        case VM_KVAL_ACC:
        {
            float vmKvalAcc = PS01_Param_GetKvalAcc(motor);
            memcpy(value, &vmKvalAcc, sizeof(vmKvalAcc));
            break;
        }
        case VM_KVAL_DEC:
        {
            float vmKvalDec = PS01_Param_GetKvalDec(motor);
            memcpy(value, &vmKvalDec, sizeof(vmKvalDec));
            break;
        }
        case VM_INT_SPEED:
        {
            float vmIntSpeed = PS01_Param_GetIntersectSpeed(motor);
            memcpy(value, &vmIntSpeed, sizeof(vmIntSpeed));
            break;
        }
        case VM_ST_SLP:
        {
            float vmStSlp = PS01_Param_GetStartSlope(motor);
            memcpy(value, &vmStSlp, sizeof(vmStSlp));
            break;
        }
        case VM_FN_SLP_ACC:
        {
            float vmFnSlpAcc = PS01_Param_GetAccFinalSlope(motor);
            memcpy(value, &vmFnSlpAcc, sizeof(vmFnSlpAcc));
            break;
        }
        case VM_FN_SLP_DEC:
        {
            float vmFnSlpDec = PS01_Param_GetDecFinalSlope(motor);
            memcpy(value, &vmFnSlpDec, sizeof(vmFnSlpDec));
            break;
        }
        case VM_K_THERM:
        {
            float vmKTherm = PS01_Param_GetThermalCompensationFactor(motor);
            memcpy(value, &vmKTherm, sizeof(vmKTherm));
            break;
        }
        case VM_STALL_TH:
        {
            float vmStallTh = PS01_Param_GetStallThreshold(motor);
            memcpy(value, &vmStallTh, sizeof(vmStallTh));
            break;
        }
#endif
        default:
        {
            return 1;
        }
    }
    return errorCode;
}

PS01_AdvancedParam_t MotorStepperParam::getAllAdvancedParamPS01(MotorNum_t motor) {
    PS01_AdvancedParam_t advancedParam;
    MotorStepperParam::getAdvancedParam(motor, ABS_POS, &advancedParam.absPos);
    // MotorStepperParam::getAdvancedParam(motor, EL_POS, &advancedParam.elPos);
    MotorStepperParam::getAdvancedParam(motor, MARK, &advancedParam.mark);
    MotorStepperParam::getAdvancedParam(motor, ACCELERATION, &advancedParam.acc);
    MotorStepperParam::getAdvancedParam(motor, DECELERATION, &advancedParam.dec);
    MotorStepperParam::getAdvancedParam(motor, MAX_SPEED, &advancedParam.maxSpeed);
    MotorStepperParam::getAdvancedParam(motor, MIN_SPEED, &advancedParam.minSpeed);
    MotorStepperParam::getAdvancedParam(motor, MIN_SPEED_LSPD_OPT, &advancedParam.minSpeedLspdOpt);
    MotorStepperParam::getAdvancedParam(motor, OCD_TH, &advancedParam.ocdTh);
    MotorStepperParam::getAdvancedParam(motor, FS_SPD, &advancedParam.fsSpd);
    MotorStepperParam::getAdvancedParam(motor, FS_SPD_BOOST_MODE, &advancedParam.fsSpdBoostMode);
    
    MotorStepperParam::getAdvancedParam(motor, STEP_MODE_STEP_SEL, &advancedParam.stepModeStepSel);
    MotorStepperParam::getAdvancedParam(motor, STEP_MODE_CM_VM, &advancedParam.stepModeCmVm);
    MotorStepperParam::getAdvancedParam(motor, STEP_MODE_SYNC_SEL, &advancedParam.stepModeSyncSel);
    MotorStepperParam::getAdvancedParam(motor, STEP_MODE_SYNC_EN, &advancedParam.stepModeSyncEn);

    MotorStepperParam::getAdvancedParam(motor, ALARM_EN_OVERCURRENT, &advancedParam.alarmEnOvercurrent);
    MotorStepperParam::getAdvancedParam(motor, ALARM_EN_THERMAL_SHUTDOWN, &advancedParam.alarmEnThermalShutdown);
    MotorStepperParam::getAdvancedParam(motor, ALARM_EN_THERMAL_WARNING, &advancedParam.alarmEnThermalWarning);
    MotorStepperParam::getAdvancedParam(motor, ALARM_EN_UVLO, &advancedParam.alarmEnUvlo);
    MotorStepperParam::getAdvancedParam(motor, ALARM_EN_ADC_UVLO, &advancedParam.alarmEnAdcUvlo);
    MotorStepperParam::getAdvancedParam(motor, ALARM_EN_STALL_DETECTION, &advancedParam.alarmEnStallDetection);
    MotorStepperParam::getAdvancedParam(motor, ALARM_EN_SW_TURN_ON, &advancedParam.alarmEnSwTurnOn);
    MotorStepperParam::getAdvancedParam(motor, ALARM_EN_COMMAND_ERROR, &advancedParam.alarmEnCommandError);

    MotorStepperParam::getAdvancedParam(motor, GATECFG1_TCC, &advancedParam.gateCfg1Tcc);
    MotorStepperParam::getAdvancedParam(motor, GATECFG1_IGATE, &advancedParam.gateCfg1Igate);
    MotorStepperParam::getAdvancedParam(motor, GATECFG1_TBOOST, &advancedParam.gateCfg1Tboost);
    MotorStepperParam::getAdvancedParam(motor, GATECFG1_WD_EN, &advancedParam.gateCfg1WdEn);
    MotorStepperParam::getAdvancedParam(motor, GATECFG2_TDT, &advancedParam.gateCfg2Tdt);
    MotorStepperParam::getAdvancedParam(motor, GATECFG2_TBLANK, &advancedParam.gateCfg2Tblank);

    MotorStepperParam::getAdvancedParam(motor, CONFIG_OSC_SEL, &advancedParam.configOscSel);
    MotorStepperParam::getAdvancedParam(motor, CONFIG_EXT_CLK, &advancedParam.configExtClk);
    MotorStepperParam::getAdvancedParam(motor, CONFIG_SW_MODE, &advancedParam.configSwMode);
    MotorStepperParam::getAdvancedParam(motor, CONFIG_OC_SD, &advancedParam.configOcSd);
    MotorStepperParam::getAdvancedParam(motor, CONFIG_UVLOVAL, &advancedParam.configUvloval);
    MotorStepperParam::getAdvancedParam(motor, CONFIG_VCCVAL, &advancedParam.configVccval);

#ifdef CONFIG_CURRENT_MODE
    MotorStepperParam::getAdvancedParam(motor, CM_CONFIG_EN_TQREG, &advancedParam.cmConfigEnTqreg);
    MotorStepperParam::getAdvancedParam(motor, CM_CONFIG_TSW, &advancedParam.cmConfigTsw);
    MotorStepperParam::getAdvancedParam(motor, CM_CONFIG_PRED_EN, &advancedParam.cmConfigPredEn);
    MotorStepperParam::getAdvancedParam(motor, CM_TVAL_HOLD, &advancedParam.cmTvalHold);
    MotorStepperParam::getAdvancedParam(motor, CM_TVAL_RUN, &advancedParam.cmTvalRun);
    MotorStepperParam::getAdvancedParam(motor, CM_TVAL_ACC, &advancedParam.cmTvalAcc);
    MotorStepperParam::getAdvancedParam(motor, CM_TVAL_DEC, &advancedParam.cmTvalDec);
    MotorStepperParam::getAdvancedParam(motor, CM_TFAST_FAST_STEP, &advancedParam.cmTfastFastStep);
    MotorStepperParam::getAdvancedParam(motor, CM_TFAST_TOFF_FAST, &advancedParam.cmTfastToffFast);
    MotorStepperParam::getAdvancedParam(motor, CM_TON_MIN, &advancedParam.cmTonMin);
    MotorStepperParam::getAdvancedParam(motor, CM_TOFF_MIN, &advancedParam.cmToffMin);

#else
    MotorStepperParam::getAdvancedParam(motor, VM_CONFIG_EN_VSCOMP, &advancedParam.vmConfigEnVscomp);
    MotorStepperParam::getAdvancedParam(motor, VM_CONFIG_F_PWM_DEC, &advancedParam.vmConfigFPwmDec);
    MotorStepperParam::getAdvancedParam(motor, VM_CONFIG_F_PWM_INT, &advancedParam.vmConfigFPwmInt);
    MotorStepperParam::getAdvancedParam(motor, VM_KVAL_HOLD, &advancedParam.vmKvalHold);
    MotorStepperParam::getAdvancedParam(motor, VM_KVAL_RUN, &advancedParam.vmKvalRun);
    MotorStepperParam::getAdvancedParam(motor, VM_KVAL_ACC, &advancedParam.vmKvalAcc);
    MotorStepperParam::getAdvancedParam(motor, VM_KVAL_DEC, &advancedParam.vmKvalDec);
    MotorStepperParam::getAdvancedParam(motor, VM_INT_SPEED, &advancedParam.vmIntSpeed);
    MotorStepperParam::getAdvancedParam(motor, VM_ST_SLP, &advancedParam.vmStSlp);
    MotorStepperParam::getAdvancedParam(motor, VM_FN_SLP_ACC, &advancedParam.vmFnSlpAcc);
    MotorStepperParam::getAdvancedParam(motor, VM_FN_SLP_DEC, &advancedParam.vmFnSlpDec);
    MotorStepperParam::getAdvancedParam(motor, VM_K_THERM, &advancedParam.vmKTherm);
    MotorStepperParam::getAdvancedParam(motor, VM_STALL_TH, &advancedParam.vmStallTh);

#endif
    return advancedParam;
}

int MotorStepperParam::setAllAdvancedParamPS01(MotorNum_t motor, PS01_AdvancedParam_t advancedParam)
{
    // // print all values in the structure
    // printf("ABS_POS: %d\n", advancedParam.absPos);
    // printf("MARK: %d\n", advancedParam.mark);
    // printf("ACC: %f\n", advancedParam.acc);
    // printf("DEC: %f\n", advancedParam.dec);
    // printf("MAX_SPEED: %f\n", advancedParam.maxSpeed);
    // printf("MIN_SPEED: %f\n", advancedParam.minSpeed);
    // printf("MIN_SPEED_LSPD_OPT: %d\n", advancedParam.minSpeedLspdOpt);
    // printf("FS_SPD: %f\n", advancedParam.fsSpd);
    // printf("KVAL_HOLD: %f\n", advancedParam.vmKvalHold);
    // printf("KVAL_RUN: %f\n", advancedParam.vmKvalRun);

    // set all values in the structure
    int errorCode = 0;
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, ABS_POS, &advancedParam.absPos);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, MARK, &advancedParam.mark);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, ACCELERATION, &advancedParam.acc);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, DECELERATION, &advancedParam.dec);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, MAX_SPEED, &advancedParam.maxSpeed);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, MIN_SPEED, &advancedParam.minSpeed);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, MIN_SPEED_LSPD_OPT, &advancedParam.minSpeedLspdOpt);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, OCD_TH, &advancedParam.ocdTh);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, FS_SPD, &advancedParam.fsSpd);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, FS_SPD_BOOST_MODE, &advancedParam.fsSpdBoostMode);

    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, STEP_MODE_STEP_SEL, &advancedParam.stepModeStepSel);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, STEP_MODE_CM_VM, &advancedParam.stepModeCmVm);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, STEP_MODE_SYNC_SEL, &advancedParam.stepModeSyncSel);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, STEP_MODE_SYNC_EN, &advancedParam.stepModeSyncEn);

    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, ALARM_EN_OVERCURRENT, &advancedParam.alarmEnOvercurrent);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, ALARM_EN_THERMAL_SHUTDOWN, &advancedParam.alarmEnThermalShutdown);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, ALARM_EN_THERMAL_WARNING, &advancedParam.alarmEnThermalWarning);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, ALARM_EN_UVLO, &advancedParam.alarmEnUvlo);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, ALARM_EN_ADC_UVLO, &advancedParam.alarmEnAdcUvlo);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, ALARM_EN_STALL_DETECTION, &advancedParam.alarmEnStallDetection);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, ALARM_EN_SW_TURN_ON, &advancedParam.alarmEnSwTurnOn);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, ALARM_EN_COMMAND_ERROR, &advancedParam.alarmEnCommandError);

    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, GATECFG1_TCC, &advancedParam.gateCfg1Tcc);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, GATECFG1_IGATE, &advancedParam.gateCfg1Igate);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, GATECFG1_TBOOST, &advancedParam.gateCfg1Tboost);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, GATECFG1_WD_EN, &advancedParam.gateCfg1WdEn);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, GATECFG2_TDT, &advancedParam.gateCfg2Tdt);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, GATECFG2_TBLANK, &advancedParam.gateCfg2Tblank);
    
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, CONFIG_OSC_SEL, &advancedParam.configOscSel);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, CONFIG_EXT_CLK, &advancedParam.configExtClk);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, CONFIG_SW_MODE, &advancedParam.configSwMode);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, CONFIG_OC_SD, &advancedParam.configOcSd);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, CONFIG_UVLOVAL, &advancedParam.configUvloval);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, CONFIG_VCCVAL, &advancedParam.configVccval);

#ifdef CONFIG_CURRENT_MODE
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, CM_CONFIG_EN_TQREG, &advancedParam.cmConfigEnTqreg);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, CM_CONFIG_TSW, &advancedParam.cmConfigTsw);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, CM_CONFIG_PRED_EN, &advancedParam.cmConfigPredEn);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, CM_TVAL_HOLD, &advancedParam.cmTvalHold);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, CM_TVAL_RUN, &advancedParam.cmTvalRun);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, CM_TVAL_ACC, &advancedParam.cmTvalAcc);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, CM_TVAL_DEC, &advancedParam.cmTvalDec);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, CM_TFAST_FAST_STEP, &advancedParam.cmTfastFastStep);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, CM_TFAST_TOFF_FAST, &advancedParam.cmTfastToffFast);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, CM_TON_MIN, &advancedParam.cmTonMin);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, CM_TOFF_MIN, &advancedParam.cmToffMin);

#else
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, VM_CONFIG_EN_VSCOMP, &advancedParam.vmConfigEnVscomp);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, VM_CONFIG_F_PWM_DEC, &advancedParam.vmConfigFPwmDec);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, VM_CONFIG_F_PWM_INT, &advancedParam.vmConfigFPwmInt);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, VM_KVAL_HOLD, &advancedParam.vmKvalHold);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, VM_KVAL_RUN, &advancedParam.vmKvalRun);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, VM_KVAL_ACC, &advancedParam.vmKvalAcc);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, VM_KVAL_DEC, &advancedParam.vmKvalDec);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, VM_INT_SPEED, &advancedParam.vmIntSpeed);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, VM_ST_SLP, &advancedParam.vmStSlp);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, VM_FN_SLP_ACC, &advancedParam.vmFnSlpAcc);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, VM_FN_SLP_DEC, &advancedParam.vmFnSlpDec);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, VM_K_THERM, &advancedParam.vmKTherm);
    errorCode += MotorStepperParam::setAdvancedParamWithoutNvs(motor, VM_STALL_TH, &advancedParam.vmStallTh);

#endif
    return errorCode;
}

int MotorStepperParam::resetAllAdvancedParamPS01(MotorNum_t motor) {
    // Reset hardware chip
    PS01_Hal_Reset(motor);
    vTaskDelay(10/portTICK_PERIOD_MS);
    PS01_Hal_ReleaseReset(motor);
    vTaskDelay(10/portTICK_PERIOD_MS);

    // Set default parameters
    int errorCode = MotorStepperParam::setAllAdvancedParamPS01(motor, defaultParameters);
    if (errorCode != 0) {
        printf("Error while setting default parameters\n");
    } else {
        MotorStepperParam::setNVSParam(motor, defaultParameters);
    }
    return errorCode;
}


int MotorStepperParam::initNVSParam(void) 
{        
    int err = 0;

    /* Initialize NVS */
    err |= nvs_flash_init_partition(MOTOR_STEPPER_NVS_PARTITION);

    for (int i=0; i<POWERSTEP01_NUMBER_OF_DEVICES; i++) {
        // Check if motor is active
        if (PS01_IsActive(i)) {
            // Get or create the advanced param from NVS
            PS01_AdvancedParam_t param = MotorStepperParam::getNVSParam((MotorNum_t) i);
            // printf("param step sel %d\n", param.stepModeStepSel);
            err |= MotorStepperParam::setAllAdvancedParamPS01((MotorNum_t) i, param);
        } else {
            ESP_LOGE("MotorStepper", "Motor %d is not active", i);
            err |= -1;
        }
    }

    /* CLI */
    _registerCLI();

    return err;
}


PS01_AdvancedParam_t MotorStepperParam::getNVSParam(MotorNum_t motor) 
{
    PS01_AdvancedParam_t nvsParam;
    size_t nvsParamSize = sizeof(PS01_AdvancedParam_t);
    
    /* Initialize NVS */
    ESP_ERROR_CHECK(nvs_flash_init_partition(MOTOR_STEPPER_NVS_PARTITION));

    /* Open NVS */
    nvs_handle_t handle;
    ESP_ERROR_CHECK_WITHOUT_ABORT(nvs_open_from_partition(MOTOR_STEPPER_NVS_PARTITION, MOTOR_STEPPER_NVS_NAMESPACE, NVS_READWRITE, &handle));
    if (nvs_get_blob(handle, MOTOR_STEPPER_NVS_KEY[(uint8_t) motor], &nvsParam, &nvsParamSize) != ESP_OK 
        || nvsParamSize != sizeof(PS01_AdvancedParam_t)) {
        // If the key doesn't exist in NVS or size is not good, (re)-create it
        nvsParam = defaultParameters;
        int errorCode = MotorStepperParam::setAllAdvancedParamPS01(motor, nvsParam);
        // Check if the motor is active
        if (errorCode != 0) {
            // print error
            ESP_LOGE("MotorStepper", "Motor %d is not active, unable to set NVS", motor);
        } else {
            ESP_ERROR_CHECK_WITHOUT_ABORT(nvs_set_blob(handle, MOTOR_STEPPER_NVS_KEY[(uint8_t) motor], &defaultParameters, sizeof(defaultParameters)));
            ESP_ERROR_CHECK_WITHOUT_ABORT(nvs_commit(handle));
        }
    }
    nvs_close(handle);

    return nvsParam;
}

int MotorStepperParam::setNVSParam(MotorNum_t motor, PS01_AdvancedParam_t nvsParam) 
{
    int err = 0;

    // Check if the motor is active
    if (nvsParam.maxSpeed == 0 && nvsParam.minSpeed == 0 && nvsParam.acc == 0 && nvsParam.dec == 0) {
        // print error
        ESP_LOGE("MotorStepper", "Motor %d is not active, unable to set NVS", motor);
        err |= ESP_ERR_INVALID_ARG;
    } else {
    /* Open NVS */
        nvs_handle_t handle;
        err |= nvs_open_from_partition(MOTOR_STEPPER_NVS_PARTITION, MOTOR_STEPPER_NVS_NAMESPACE, NVS_READWRITE, &handle);
        err |= nvs_set_blob(handle, MOTOR_STEPPER_NVS_KEY[(uint8_t) motor], &nvsParam, sizeof(nvsParam));
        err |= nvs_commit(handle);
        nvs_close(handle);
    }

    return err;
}