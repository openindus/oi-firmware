#include "PS01_Param.h"
#include "PS01_Cmd.h"
#include "PS01_Utils.h"

static const char PS01_TAG[] = "powerSTEP01";

int32_t PS01_ConvertPosition(uint32_t abs_position_reg)
{
	int32_t operation_result;

	if (abs_position_reg & POWERSTEP01_ABS_POS_SIGN_BIT_MASK) 
	{
			/* Negative register value */
			abs_position_reg = ~abs_position_reg;
			abs_position_reg += 1;

			operation_result = (int32_t) (abs_position_reg & POWERSTEP01_ABS_POS_VALUE_MASK);
			operation_result = -operation_result;
	} 
	else 
	{
			operation_result = (int32_t) abs_position_reg;
		}
		return operation_result;
}

float PS01_GetAnalogValue(uint8_t deviceId, uint32_t param)
{
	bool voltageMode = ((POWERSTEP01_CM_VM_CURRENT&PS01_Cmd_GetParam(deviceId,POWERSTEP01_STEP_MODE))==0);
	uint32_t registerValue = PS01_Cmd_GetParam(deviceId, param);
	float value = 0;
	switch (param)
	{
		case POWERSTEP01_ABS_POS:
		case POWERSTEP01_MARK:
		value = (float) PS01_ConvertPosition(registerValue);
		break;
		case POWERSTEP01_ACC:
		case POWERSTEP01_DEC:
		value = PS01_AccDec_RegVal_to_Steps_s2(registerValue);
		break;
		case POWERSTEP01_SPEED:
		value = PS01_Speed_RegVal_to_Steps_s(registerValue);
		break;
		case POWERSTEP01_MAX_SPEED:
		value = PS01_MaxSpd_RegVal_to_Steps_s(registerValue);
		break;
		case POWERSTEP01_MIN_SPEED:
		registerValue &= POWERSTEP01_MIN_SPEED_MASK;
		value = PS01_MinSpd_RegVal_to_Steps_s(registerValue);
		break;      
		case POWERSTEP01_FS_SPD:
		registerValue &= POWERSTEP01_FS_SPD_MASK;
		value = PS01_FSSpd_RegVal_to_Steps_s(registerValue);
		break;
		case POWERSTEP01_INT_SPD:
		value = PS01_IntSpd_RegVal_to_Steps_s(registerValue);
		break;
		case POWERSTEP01_K_THERM:
		value = PS01_KTherm_RegVal_to_Comp(registerValue);
		break;
		case POWERSTEP01_OCD_TH:
		case POWERSTEP01_STALL_TH:
		value = PS01_StallOcd_RegVal_to_Th(registerValue);
		break;
		case POWERSTEP01_KVAL_HOLD:  //POWERSTEP01_TVAL_HOLD
		case POWERSTEP01_KVAL_RUN:   //POWERSTEP01_TVAL_RUN
		case POWERSTEP01_KVAL_ACC:   //POWERSTEP01_TVAL_ACC
		case POWERSTEP01_KVAL_DEC:   //POWERSTEP01_TVAL_DEC
		if (voltageMode!=0)  value = PS01_Kval_RegVal_to_Perc(registerValue);
		else value = PS01_Tval_RegVal_to_RefVoltage(registerValue);     
		break;
		case POWERSTEP01_ST_SLP:
		if (voltageMode==0) 
		{
			break;
		}
		else value = PS01_BEMFslope_RegVal_to_Perc(registerValue);
		break;
		case POWERSTEP01_FN_SLP_ACC: //POWERSTEP01_TON_MIN
		case POWERSTEP01_FN_SLP_DEC: //POWERSTEP01_TOFF_MIN
		if (voltageMode!=0) value = PS01_BEMFslope_RegVal_to_Perc(registerValue);
		else value = PS01_Tmin_RegVal_to_Time(registerValue);
		break;
		default:
		value = (float) registerValue;
	}
	return value;
}

bool PS01_SetAnalogValue(uint8_t deviceId, uint32_t param, float value)
{
	uint32_t registerValue = 0;
	bool result = 1;
	bool voltageMode = ((POWERSTEP01_CM_VM_CURRENT&PS01_Cmd_GetParam(deviceId,POWERSTEP01_STEP_MODE))==0);
	if ((value < 0)&&((param != POWERSTEP01_ABS_POS)&&(param != POWERSTEP01_MARK)))
	{
		result = 0;
	}
	switch (param)
	{
		case POWERSTEP01_EL_POS:
		if ((value > (POWERSTEP01_ELPOS_STEP_MASK|POWERSTEP01_ELPOS_MICROSTEP_MASK))||
			((value!=0)&&(value < (1<<(7-(POWERSTEP01_STEP_MODE_STEP_SEL&PS01_Cmd_GetParam(0,POWERSTEP01_STEP_MODE))))))) result = 0;
		else registerValue = ((uint32_t) value)&(POWERSTEP01_ELPOS_STEP_MASK|POWERSTEP01_ELPOS_MICROSTEP_MASK);
		break;        
		case POWERSTEP01_ABS_POS:
		case POWERSTEP01_MARK:
		if (value < 0)
		{
			value=-value;
			if (((uint32_t)value)<=(POWERSTEP01_MAX_POSITION+1))
			registerValue = (POWERSTEP01_ABS_POS_VALUE_MASK+1-(uint32_t)value)&POWERSTEP01_ABS_POS_VALUE_MASK;
			else result = 0;  
		}
		else
		{
			if (((uint32_t)value)<=POWERSTEP01_MAX_POSITION)
			registerValue = ((uint32_t) value)&POWERSTEP01_ABS_POS_VALUE_MASK;
			else result = 0; 
		}
		break;
		case POWERSTEP01_ACC:
		case POWERSTEP01_DEC:
		if (value > POWERSTEP01_ACC_DEC_MAX_VALUE) result = 0;
		else registerValue = PS01_AccDec_Steps_s2_to_RegVal(value);
		break;
		case POWERSTEP01_MAX_SPEED:
		if (value > POWERSTEP01_MAX_SPEED_MAX_VALUE) result = 0;
		else registerValue = PS01_MaxSpd_Steps_s_to_RegVal(value);
		break;
		case POWERSTEP01_MIN_SPEED:
		if (value > POWERSTEP01_MIN_SPEED_MAX_VALUE) result = 0;
		else registerValue = (POWERSTEP01_LSPD_OPT&PS01_Cmd_GetParam(deviceId, param))|PS01_MinSpd_Steps_s_to_RegVal(value);
		break;      
		case POWERSTEP01_FS_SPD:
		if (value > POWERSTEP01_FS_SPD_MAX_VALUE) result = 0;
		else registerValue = (POWERSTEP01_BOOST_MODE&PS01_Cmd_GetParam(deviceId, param))|PS01_FSSpd_Steps_s_to_RegVal(value);
		break;
		case POWERSTEP01_INT_SPD:
		if (value > POWERSTEP01_INT_SPD_MAX_VALUE) result = 0;
		else registerValue = PS01_IntSpd_Steps_s_to_RegVal(value);
		break;
		case POWERSTEP01_K_THERM:
		if ((value < POWERSTEP01_K_THERM_MIN_VALUE)||
			(value > POWERSTEP01_K_THERM_MAX_VALUE)) result = 0;
		else registerValue = PS01_KTherm_Comp_to_RegVal(value);
		break;
		case POWERSTEP01_OCD_TH:
		case POWERSTEP01_STALL_TH:
		if (value > POWERSTEP01_STALL_OCD_TH_MAX_VALUE) result = 0;
		else registerValue = PS01_StallOcd_Th_to_RegVal(value);
		break;
		case POWERSTEP01_KVAL_HOLD:  //POWERSTEP01_TVAL_HOLD
		case POWERSTEP01_KVAL_RUN:   //POWERSTEP01_TVAL_RUN
		case POWERSTEP01_KVAL_ACC:   //POWERSTEP01_TVAL_ACC
		case POWERSTEP01_KVAL_DEC:   //POWERSTEP01_TVAL_DEC
		if (voltageMode==0)
		{
			if (value > POWERSTEP01_TVAL_MAX_VALUE) result = 0;
			else registerValue = PS01_Tval_RefVoltage_to_RegVal(value);
		}
		else
		{
			if (value > POWERSTEP01_KVAL_MAX_VALUE) result = 0;
			else registerValue = PS01_Kval_Perc_to_RegVal(value);
		}
		break;
		case POWERSTEP01_ST_SLP: // POWERSTEP01_T_FAST
		if (voltageMode==0) 
		{
			result = 0;
			break;
		}
		else
		{
			if (value > POWERSTEP01_SLP_MAX_VALUE) result = 0;
			else registerValue = PS01_BEMFslope_Perc_to_RegVal(value);
		}
		break;
		case POWERSTEP01_FN_SLP_ACC: //POWERSTEP01_TON_MIN
		case POWERSTEP01_FN_SLP_DEC: //POWERSTEP01_TOFF_MIN
		if (voltageMode==0)
		{
			if (value>POWERSTEP01_TOFF_TON_MIN_MAX_VALUE) result = 0;
			else registerValue = PS01_Tmin_Time_to_RegVal(value);
		}
		else
		{
			if (value > POWERSTEP01_SLP_MAX_VALUE) result = 0;
			else registerValue = PS01_BEMFslope_Perc_to_RegVal(value);
		}
		break;
		case POWERSTEP01_ALARM_EN:
		if (value>POWERSTEP01_ALARM_EN_MAX_VALUE) result = 0;
		else registerValue = (uint32_t) value;
		break;
		case POWERSTEP01_GATECFG1:
		case POWERSTEP01_GATECFG2:
		registerValue = (uint32_t) value;
		break;
		default:
		result = 0;
	}
	if (result!=0)
	{
		PS01_Cmd_SetParam(deviceId, param, registerValue);
	}
	// check if the cmd was successful
	uint32_t saved_value = PS01_Cmd_GetParam(deviceId, param);
	if (saved_value != registerValue && result!=0)
	{
		result = 0;
	}

	return result;
}

/**
 * @brief Set default parameters
 * 
 * @param deviceId 
 * @param param 
 */
void PS01_Param_SetAll(uint8_t deviceId, PS01_Param_t *param)
{
	PS01_Cmd_SetParam(deviceId, POWERSTEP01_ABS_POS, 0);
	PS01_Cmd_SetParam(deviceId, POWERSTEP01_EL_POS, 0);
	PS01_Cmd_SetParam(deviceId, POWERSTEP01_MARK, 0);
	PS01_Cmd_SetParam(deviceId, POWERSTEP01_ACC,
		PS01_AccDec_Steps_s2_to_RegVal(param->cm.cp.acceleration));
	PS01_Cmd_SetParam(deviceId, POWERSTEP01_DEC,
		PS01_AccDec_Steps_s2_to_RegVal(param->cm.cp.deceleration));
	PS01_Cmd_SetParam(deviceId, POWERSTEP01_MAX_SPEED,
		PS01_MaxSpd_Steps_s_to_RegVal(param->cm.cp.maxSpeed));
	PS01_Cmd_SetParam(deviceId, POWERSTEP01_MIN_SPEED,
		param->cm.cp.lowSpeedOptimization|
		PS01_MaxSpd_Steps_s_to_RegVal(param->cm.cp.minSpeed));
	PS01_Cmd_SetParam(deviceId, POWERSTEP01_FS_SPD,
		param->cm.cp.boostMode|
		PS01_FSSpd_Steps_s_to_RegVal(param->cm.cp.fullStepSpeed));
	PS01_Cmd_SetParam(deviceId, POWERSTEP01_OCD_TH,
		PS01_StallOcd_Th_to_RegVal(param->cm.cp.ocdThreshold));
	PS01_Cmd_SetParam(deviceId, POWERSTEP01_STEP_MODE,
		(uint8_t)param->cm.cp.syncClockSelection|
		(uint8_t)param->cm.cp.cmVmSelection|
		(uint8_t)(uint8_t)param->cm.cp.stepMode);
	PS01_Cmd_SetParam(deviceId, POWERSTEP01_ALARM_EN,
		param->cm.cp.alarmsSelection);
	PS01_Cmd_SetParam(deviceId, POWERSTEP01_GATECFG1,
		(uint16_t)param->cm.cp.iGate|
		(uint16_t)param->cm.cp.tcc|
		(uint16_t)param->cm.cp.tBoost|
		(uint16_t)param->cm.cp.wdEn);
	PS01_Cmd_SetParam(deviceId, POWERSTEP01_GATECFG2,
		(uint16_t)param->cm.cp.tBlank|
		(uint16_t)param->cm.cp.tdt);  
	if (param->cm.cp.cmVmSelection == POWERSTEP01_CM_VM_VOLTAGE)
	{
		//Voltage mode
		PS01_Cmd_SetParam(deviceId, POWERSTEP01_INT_SPD,
		PS01_IntSpd_Steps_s_to_RegVal(
			param->vm.intersectSpeed));
		PS01_Cmd_SetParam(deviceId, POWERSTEP01_K_THERM,
		PS01_KTherm_Comp_to_RegVal(
			param->vm.thermalCompensationFactor)); 
		PS01_Cmd_SetParam(deviceId, POWERSTEP01_STALL_TH,
		PS01_StallOcd_Th_to_RegVal(
			param->vm.stallThreshold));
		PS01_Cmd_SetParam(deviceId, POWERSTEP01_KVAL_HOLD,
		PS01_Kval_Perc_to_RegVal(
			param->vm.kvalHold));
		PS01_Cmd_SetParam(deviceId, POWERSTEP01_KVAL_RUN,
		PS01_Kval_Perc_to_RegVal(
			param->vm.kvalRun));
		PS01_Cmd_SetParam(deviceId, POWERSTEP01_KVAL_ACC,
		PS01_Kval_Perc_to_RegVal(
			param->vm.kvalAcc));
		PS01_Cmd_SetParam(deviceId, POWERSTEP01_KVAL_DEC,
		PS01_Kval_Perc_to_RegVal(
			param->vm.kvalDec));
		PS01_Cmd_SetParam(deviceId, POWERSTEP01_ST_SLP,
		PS01_BEMFslope_Perc_to_RegVal(
			param->vm.startSlope));
		PS01_Cmd_SetParam(deviceId, POWERSTEP01_FN_SLP_ACC,
		PS01_BEMFslope_Perc_to_RegVal(
			param->vm.accelerationFinalSlope));
		PS01_Cmd_SetParam(deviceId, POWERSTEP01_FN_SLP_DEC,
		PS01_BEMFslope_Perc_to_RegVal(
			param->vm.decelerationFinalSlope));
		PS01_Cmd_SetParam(deviceId, POWERSTEP01_CONFIG,
		(uint16_t)param->vm.oscClkSel| 
		(uint16_t)param->vm.swMode | 
		(uint16_t)param->vm.enVsComp| 
		(uint16_t)param->vm.ocSd| 
		(uint16_t)param->vm.uvloVal| 
		(uint16_t)param->vm.vccVal| 
		(uint16_t)param->vm.fPwmInt| 
		(uint16_t)param->vm.fPwmDec);
	}
	else
	{
		// Current mode
		PS01_Cmd_SetParam(deviceId, POWERSTEP01_TVAL_HOLD,
		PS01_Tval_RefVoltage_to_RegVal(
			param->cm.tvalHold));
		PS01_Cmd_SetParam(deviceId, POWERSTEP01_TVAL_RUN,
		PS01_Tval_RefVoltage_to_RegVal(
			param->cm.tvalRun));
		PS01_Cmd_SetParam(deviceId, POWERSTEP01_TVAL_ACC,
		PS01_Tval_RefVoltage_to_RegVal(
			param->cm.tvalAcc));
		PS01_Cmd_SetParam(deviceId, POWERSTEP01_TVAL_DEC,
		PS01_Tval_RefVoltage_to_RegVal(
			param->cm.tvalDec));
		PS01_Cmd_SetParam(deviceId, POWERSTEP01_T_FAST,
		(uint8_t)param->cm.toffFast|
		(uint8_t)param->cm.fastStep);
		PS01_Cmd_SetParam(deviceId, POWERSTEP01_TON_MIN,
		PS01_Tmin_Time_to_RegVal(
			param->cm.tonMin));
		PS01_Cmd_SetParam(deviceId, POWERSTEP01_TOFF_MIN,
		PS01_Tmin_Time_to_RegVal(
			param->cm.toffMin));       
		PS01_Cmd_SetParam(deviceId, POWERSTEP01_CONFIG,
		(uint16_t)param->cm.oscClkSel| 
		(uint16_t)param->cm.swMode| 
		(uint16_t)param->cm.tqReg| 
		(uint16_t)param->cm.ocSd| 
		(uint16_t)param->cm.uvloVal| 
		(uint16_t)param->cm.vccVal|
		(uint16_t)param->cm.tsw|
		(uint16_t)param->cm.predEn);
	}
}

int32_t PS01_Param_GetAbsPos(uint8_t deviceId)
{
	return PS01_ConvertPosition(PS01_Cmd_GetParam(deviceId, POWERSTEP01_ABS_POS));
}

int32_t PS01_Param_GetMark(uint8_t deviceId)
{
	return PS01_ConvertPosition(PS01_Cmd_GetParam(deviceId, POWERSTEP01_MARK));
}

float PS01_Param_GetSpeed(uint8_t deviceId)
{
	return PS01_GetAnalogValue(deviceId, POWERSTEP01_SPEED);
}

float PS01_Param_GetAcc(uint8_t deviceId)
{
	return PS01_GetAnalogValue(deviceId, POWERSTEP01_ACC);
}

float PS01_Param_GetDec(uint8_t deviceId)
{
	return PS01_GetAnalogValue(deviceId, POWERSTEP01_DEC);
}

float PS01_Param_GetMaxSpeed(uint8_t deviceId)
{
	return PS01_GetAnalogValue(deviceId, POWERSTEP01_MAX_SPEED);
}

min_speed_t PS01_Param_GetMinSpeed(uint8_t deviceId)
{
	uint32_t param = PS01_Cmd_GetParam(deviceId, POWERSTEP01_STEP_MODE);
	register_min_speed_t minspd = *((register_min_speed_t *)&param);
	min_speed_t spd;
	spd.lspd_opt = (bool) minspd.lspd_opt;
	spd.min_speed = PS01_MinSpd_RegVal_to_Steps_s(minspd.min_speed);
	return spd;
}

fs_spd_t PS01_Param_GetFullStepSpeed(uint8_t deviceId)
{

	uint32_t param = PS01_Cmd_GetParam(deviceId, POWERSTEP01_FS_SPD);
	register_fs_spd_t fsspd = *((register_fs_spd_t *)&param);
	fs_spd_t fs;
	fs.boost_mode = (bool) fsspd.boost_mode;
	fs.fs_spd = PS01_FSSpd_RegVal_to_Steps_s(fsspd.fs_spd);
	return fs;
}

register_step_mode_t PS01_Param_GetStepMode(uint8_t deviceId)
{
	uint32_t param = PS01_Cmd_GetParam(deviceId, POWERSTEP01_STEP_MODE);
	return *((register_step_mode_t *)&param);
}

bool PS01_Param_GetBusyStatus(uint8_t deviceId)
{
	return !(PS01_Cmd_GetParam(deviceId, POWERSTEP01_STATUS) & POWERSTEP01_STATUS_BUSY) ? 1 : 0;
}

float PS01_Param_GetOCDThreshold(uint8_t deviceId)
{
	return PS01_GetAnalogValue(deviceId, POWERSTEP01_OCD_TH);
}

register_alarm_enabled_t PS01_Param_GetAlarmEnabled(uint8_t deviceId)
{
	uint32_t param = PS01_Cmd_GetParam(deviceId, POWERSTEP01_ALARM_EN);
	return *((register_alarm_enabled_t *)&param);
}

register_gate_cfg_1_t PS01_Param_GetGateDriverConfig1(uint8_t deviceId)
{
	uint32_t param = PS01_Cmd_GetParam(deviceId, POWERSTEP01_GATECFG1);
	return *((register_gate_cfg_1_t *)&param);
}

register_gate_cfg_2_t PS01_Param_GetGateDriverConfig2(uint8_t deviceId)
{
	uint32_t param = PS01_Cmd_GetParam(deviceId, POWERSTEP01_GATECFG2);
	return *((register_gate_cfg_2_t *)&param);
}

register_status_t PS01_Param_GetStatus(uint8_t deviceId)
{
	uint32_t param = PS01_Cmd_GetParam(deviceId, POWERSTEP01_STATUS);
	return *((register_status_t *)&param);
}

register_config_t PS01_Param_GetConfig(uint8_t deviceId)
{
	uint32_t param = PS01_Cmd_GetParam(deviceId, POWERSTEP01_CONFIG);
	return *((register_config_t *)&param);
}

/* voltage mode*/

float PS01_Param_GetKvalHold(uint8_t deviceId)
{
	return PS01_GetAnalogValue(deviceId, POWERSTEP01_KVAL_HOLD);
}

float PS01_Param_GetKvalRun(uint8_t deviceId)
{
	return PS01_GetAnalogValue(deviceId, POWERSTEP01_KVAL_RUN);
}

float PS01_Param_GetKvalAcc(uint8_t deviceId)
{
	return PS01_GetAnalogValue(deviceId, POWERSTEP01_KVAL_ACC);
}

float PS01_Param_GetKvalDec(uint8_t deviceId)
{
	return PS01_GetAnalogValue(deviceId, POWERSTEP01_KVAL_DEC);
}

float PS01_Param_GetIntersectSpeed(uint8_t deviceId)
{
	return PS01_GetAnalogValue(deviceId, POWERSTEP01_INT_SPD);
}

float PS01_Param_GetStartSlope(uint8_t deviceId)
{
	return PS01_GetAnalogValue(deviceId, POWERSTEP01_ST_SLP);
}

float PS01_Param_GetAccFinalSlope(uint8_t deviceId)
{
	return PS01_GetAnalogValue(deviceId, POWERSTEP01_FN_SLP_ACC);
}

float PS01_Param_GetDecFinalSlope(uint8_t deviceId)
{
	return PS01_GetAnalogValue(deviceId, POWERSTEP01_FN_SLP_DEC);
}

float PS01_Param_GetThermalCompensationFactor(uint8_t deviceId)
{
	return PS01_GetAnalogValue(deviceId, POWERSTEP01_K_THERM);
}

float PS01_Param_GetStallThreshold(uint8_t deviceId)
{
	return PS01_GetAnalogValue(deviceId, POWERSTEP01_STALL_TH);
}

/* current mode */

float PS01_Param_GetTvalHold(uint8_t deviceId)
{
	return PS01_GetAnalogValue(deviceId, POWERSTEP01_TVAL_HOLD);
}

float PS01_Param_GetTvalRun(uint8_t deviceId)
{
	return PS01_GetAnalogValue(deviceId, POWERSTEP01_TVAL_RUN);
}

float PS01_Param_GetTvalAcc(uint8_t deviceId)
{
	return PS01_GetAnalogValue(deviceId, POWERSTEP01_TVAL_ACC);
}

float PS01_Param_GetTvalDec(uint8_t deviceId)
{
	return PS01_GetAnalogValue(deviceId, POWERSTEP01_TVAL_DEC);
}

tfast_t PS01_Param_GetTFast(uint8_t deviceId)
{
	uint32_t value = PS01_Cmd_GetParam(deviceId, POWERSTEP01_T_FAST);
	tfast_t tfast;
	tfast.fast_step = PS01_TFast_RegVal_to_Time((uint8_t)(value & 0xF));
	tfast.toff_fast = PS01_TFast_RegVal_to_Time((uint8_t)((value >> 4) & 0xF));
	return tfast;
}

float PS01_Param_GetTOnMin(uint8_t deviceId)
{
	return PS01_GetAnalogValue(deviceId, POWERSTEP01_TON_MIN);
}

float PS01_Param_GetToffMin(uint8_t deviceId)
{
	return PS01_GetAnalogValue(deviceId, POWERSTEP01_TOFF_MIN);
}

/* ------- */
/* SETTERS */
/* ------- */

int PS01_Param_SetAbsPos(uint8_t deviceId, int32_t pos)
{
	if (!PS01_SetAnalogValue(deviceId, POWERSTEP01_ABS_POS, (float)pos)) {
        ESP_LOGW(PS01_TAG, "%s() Unable to set abs position", __FUNCTION__);
		return 1;
    }	
	return 0;
}

/* /!\ bit 8 and 7 are step and bit 0 to 6 are microstep (page 53)*/
int PS01_Param_SetElectricPos(uint8_t deviceId, uint16_t pos)
{
	if (!PS01_SetAnalogValue(deviceId, POWERSTEP01_EL_POS, (float)pos)) {
        ESP_LOGW(PS01_TAG, "%s() Unable to set elec position", __FUNCTION__);
		return 1;
	}	
	return 0;
}

int PS01_Param_SetMark(uint8_t deviceId, int32_t mark)
{
	PS01_Cmd_SetParam(deviceId, POWERSTEP01_MARK, (uint32_t) mark);
	uint32_t value = PS01_Cmd_GetParam(deviceId, POWERSTEP01_MARK);
	if (value != (uint32_t) mark) {
		ESP_LOGW(PS01_TAG, "%s() Unable to set mark", __FUNCTION__);
		return 1;
	}
	return 0;
}

int PS01_Param_SetAcc(uint8_t deviceId, float acc)
{
	if (!PS01_SetAnalogValue(deviceId, POWERSTEP01_ACC, acc)) {
        ESP_LOGW(PS01_TAG, "%s() Unable to set acceleration", __FUNCTION__);
		return 1;
	}
	return 0;
}

int PS01_Param_SetDec(uint8_t deviceId, float dec)
{
	if (!PS01_SetAnalogValue(deviceId, POWERSTEP01_DEC, dec)) {
        ESP_LOGW(PS01_TAG, "%s() Unable to set deceleration", __FUNCTION__);
		return 1;
	}
	return 0;
}

int PS01_Param_SetMaxSpeed(uint8_t deviceId, float speed)
{
	if (!PS01_SetAnalogValue(deviceId, POWERSTEP01_MAX_SPEED, speed)) {
        ESP_LOGW(PS01_TAG, "%s() Unable to set max speed", __FUNCTION__);
		return 1;
	}
	return 0;
}

int PS01_Param_SetMinSpeed(uint8_t deviceId, min_speed_t speed)
{
	register_min_speed_t reg;
	reg.min_speed = PS01_MinSpd_Steps_s_to_RegVal(speed.min_speed);
	reg.lspd_opt = (uint8_t) speed.lspd_opt;
	reg.reserverd = 0;
	uint32_t value = *((uint32_t *)&reg);
	PS01_Cmd_SetParam(deviceId, POWERSTEP01_MIN_SPEED, value);
	uint32_t read_value = PS01_Cmd_GetParam(deviceId, POWERSTEP01_MIN_SPEED);
	if (value != read_value) {
		ESP_LOGW(PS01_TAG, "%s() Unable to set min speed", __FUNCTION__);
		return 1;
	}
	return 0;
}

int PS01_Param_SetOCDThreshold(uint8_t deviceId, float threshold)
{
	if (!PS01_SetAnalogValue(deviceId, POWERSTEP01_OCD_TH, (float)threshold)) {
        ESP_LOGW(PS01_TAG, "%s() Unable to set ocd threshold", __FUNCTION__);
		return 1;
    }	
	return 0;
}

int PS01_Param_SetFullStepSpeed(uint8_t deviceId, fs_spd_t speed)
{
	register_fs_spd_t reg;
	reg.fs_spd = PS01_FSSpd_Steps_s_to_RegVal(speed.fs_spd);
	reg.boost_mode = (uint8_t) speed.boost_mode;
	reg.reserved = 0;
	uint32_t value = *((uint32_t *)&reg);
	PS01_Cmd_SetParam(deviceId, POWERSTEP01_FS_SPD, value);
	uint32_t read_value = PS01_Cmd_GetParam(deviceId, POWERSTEP01_FS_SPD);
	if (value != read_value) {
		ESP_LOGW(PS01_TAG, "%s() Unable to set full step speed", __FUNCTION__);
		return 1;
	}
	return 0;
}


void PS01_Param_SetStepMode(uint8_t deviceId, PS01_StepMode_t stepMode)
{
	uint8_t stepModeRegister;
	PS01_StepSel_t powerstep01StepMode;

	switch (stepMode)
	{
		case STEP_MODE_FULL:
		powerstep01StepMode = POWERSTEP01_STEP_SEL_1;
		break;
		case STEP_MODE_HALF:
		powerstep01StepMode = POWERSTEP01_STEP_SEL_1_2;
		break;    
		case STEP_MODE_1_4:
		powerstep01StepMode = POWERSTEP01_STEP_SEL_1_4;
		break;        
		case STEP_MODE_1_8:
		powerstep01StepMode = POWERSTEP01_STEP_SEL_1_8;
		break;
		case STEP_MODE_1_16:
		powerstep01StepMode = POWERSTEP01_STEP_SEL_1_16;
		break;        
		case STEP_MODE_1_32:
		powerstep01StepMode = POWERSTEP01_STEP_SEL_1_32;
		break;
		case STEP_MODE_1_64:
		powerstep01StepMode = POWERSTEP01_STEP_SEL_1_64;
		break;
		case STEP_MODE_1_128:
		powerstep01StepMode = POWERSTEP01_STEP_SEL_1_128;
		break;
		default:
		ESP_LOGW(PS01_TAG, "%s() Unable to set step mode", __FUNCTION__);
		return;
	}
	
	/* Set the powerstep01 in HiZ state */
	PS01_Cmd_HardHiZ(deviceId);  
	
	/* Read Step mode register and clear STEP_SEL field */
	stepModeRegister = (uint8_t)(0xF8 & PS01_Cmd_GetParam(deviceId,POWERSTEP01_STEP_MODE)) ;
	
	/* Apply new step mode */
	PS01_Cmd_SetParam(deviceId, POWERSTEP01_STEP_MODE, stepModeRegister | (uint8_t)powerstep01StepMode);

	/* Reset abs pos register */
	PS01_Cmd_ResetPos(deviceId);
}


int PS01_Param_SetStepModeComplete(uint8_t deviceId, register_step_mode_t stepMode)
{
	stepMode.reserved = 0;
	uint32_t value = *((uint32_t *)&stepMode);
	PS01_Cmd_HardHiZ(deviceId);
	
	PS01_Cmd_SetParam(deviceId, POWERSTEP01_STEP_MODE, value);
	uint32_t read_value = PS01_Cmd_GetParam(deviceId, POWERSTEP01_STEP_MODE);
		if (value != read_value) {
		ESP_LOGW(PS01_TAG, "%s() Unable to set step mode", __FUNCTION__);
		return 1;
	}
	return 0;
}

int PS01_Param_SetAlarmEnabled(uint8_t deviceId, register_alarm_enabled_t alarm_enabled)
{
	alarm_enabled.reserved = 0;
	uint32_t value = *((uint32_t *)&alarm_enabled);
	PS01_Cmd_SetParam(deviceId, POWERSTEP01_ALARM_EN, value);
	uint32_t read_value = PS01_Cmd_GetParam(deviceId, POWERSTEP01_ALARM_EN);
	if (value != read_value) {
		ESP_LOGW(PS01_TAG, "%s() Unable to set alarm enabled", __FUNCTION__);
		return 1;
	}
	return 0;
}

int PS01_Param_SetGateDriverConfig1(uint8_t deviceId, register_gate_cfg_1_t config)
{
	config.reserved = 0;
	uint32_t value = *((uint32_t *)&config);
	PS01_Cmd_SetParam(deviceId, POWERSTEP01_GATECFG1, value);
	uint32_t read_value = PS01_Cmd_GetParam(deviceId, POWERSTEP01_GATECFG1);
	if (value != read_value) {
		ESP_LOGW(PS01_TAG, "%s() Unable to set gate driver config 1", __FUNCTION__);
		return 1;
	}
	return 0;
}

int PS01_Param_SetGateDriverConfig2(uint8_t deviceId, register_gate_cfg_2_t config)
{
	config.reserved = 0;
	uint32_t value = *((uint32_t *)&config);
	PS01_Cmd_SetParam(deviceId, POWERSTEP01_GATECFG2, value);
	uint32_t read_value = PS01_Cmd_GetParam(deviceId, POWERSTEP01_GATECFG2);
	if (value != read_value) {
		ESP_LOGW(PS01_TAG, "%s() Unable to set gate driver config 2", __FUNCTION__);
		return 1;
	}
	return 0;
}

int PS01_Param_SetConfig(uint8_t deviceId, register_config_t config)
{
	config.cm.reserved = 0;
	config.cm.reserved_2 = 0;
	uint32_t value = *((uint32_t *)&config);
	PS01_Cmd_SetParam(deviceId, POWERSTEP01_CONFIG, value);
	uint32_t read_value = PS01_Cmd_GetParam(deviceId, POWERSTEP01_CONFIG);
	if (value != read_value) {
		ESP_LOGW(PS01_TAG, "%s() Unable to set config", __FUNCTION__);
		return 1;
	}
	return 0;
}

/* voltage mode */

int PS01_Param_SetKvalHold(uint8_t deviceId, float kval)
{
	if (!PS01_SetAnalogValue(deviceId, POWERSTEP01_KVAL_HOLD, kval)) {
        ESP_LOGW(PS01_TAG, "%s() Unable to set kval hold", __FUNCTION__);
		return 1;
	}
	return 0;
}

int PS01_Param_SetKvalRun(uint8_t deviceId, float kval)
{
	if (!PS01_SetAnalogValue(deviceId, POWERSTEP01_KVAL_RUN, kval)) {
        ESP_LOGW(PS01_TAG, "%s() Unable to set kval run", __FUNCTION__);
		return 1;
	}
	return 0;
}

int PS01_Param_SetKvalAcc(uint8_t deviceId, float kval)
{
	if (!PS01_SetAnalogValue(deviceId, POWERSTEP01_KVAL_ACC, kval)) {
        ESP_LOGW(PS01_TAG, "%s() Unable to set kval acc", __FUNCTION__);
		return 1;
	}
	return 0;
}

int PS01_Param_SetKvalDec(uint8_t deviceId, float kval)
{
	if (!PS01_SetAnalogValue(deviceId, POWERSTEP01_KVAL_DEC, kval)) {
        ESP_LOGW(PS01_TAG, "%s() Unable to set kval dec", __FUNCTION__);
		return 1;
	}
	return 0;
}


int PS01_Param_SetIntersectSpeed(uint8_t deviceId, float speed)
{
	if (!PS01_SetAnalogValue(deviceId, POWERSTEP01_INT_SPD, speed)) {
        ESP_LOGW(PS01_TAG, "%s() Unable to set int speed", __FUNCTION__);
		return 1;
	}
	return 0;
}

int PS01_Param_SetStartSlope(uint8_t deviceId, float start)
{
	if (!PS01_SetAnalogValue(deviceId, POWERSTEP01_ST_SLP, start)) {
        ESP_LOGW(PS01_TAG, "%s() Unable to set start slope", __FUNCTION__);
		return 1;
	}
	return 0;
}

int PS01_Param_SetAccFinalSlope(uint8_t deviceId, float acc) {
	if (!PS01_SetAnalogValue(deviceId, POWERSTEP01_FN_SLP_ACC, acc)) {
        ESP_LOGW(PS01_TAG, "%s() Unable to set slope acc", __FUNCTION__);
		return 1;
	}
	return 0;
}

int PS01_Param_SetDecFinalSlope(uint8_t deviceId, float dec) {
	if (!PS01_SetAnalogValue(deviceId, POWERSTEP01_FN_SLP_DEC, dec)) {
        ESP_LOGW(PS01_TAG, "%s() Unable to set slope dec", __FUNCTION__);
		return 1;
	}
	return 0;
}

int PS01_Param_SetThermalCompensationFactor(uint8_t deviceId, float compFactor) 
{
	if (!PS01_SetAnalogValue(deviceId, POWERSTEP01_K_THERM, compFactor)) {
        ESP_LOGW(PS01_TAG, "%s() Unable to set k therm", __FUNCTION__);
		return 1;
	}
	return 0;
}

int PS01_Param_SetStallThreshold(uint8_t deviceId, float threshold)
{
	if (!PS01_SetAnalogValue(deviceId, POWERSTEP01_STALL_TH, threshold)) {
        ESP_LOGW(PS01_TAG, "%s() Unable to set stall threshold", __FUNCTION__);
		return 1;
	}
	return 0;
}


/* current mode*/

int PS01_Param_SetTvalHold(uint8_t deviceId, float tval)
{
	if (!PS01_SetAnalogValue(deviceId, POWERSTEP01_TVAL_HOLD, tval)) {
        ESP_LOGW(PS01_TAG, "%s() Unable to set tval hold", __FUNCTION__);
		return 1;
	}
	return 0;
}

int PS01_Param_SetTvalRun(uint8_t deviceId, float tval)
{
	if (!PS01_SetAnalogValue(deviceId, POWERSTEP01_TVAL_RUN, tval)) {
        ESP_LOGW(PS01_TAG, "%s() Unable to set tval run", __FUNCTION__);
		return 1;
	}
	return 0;
}

int PS01_Param_SetTvalAcc(uint8_t deviceId, float tval)
{
	if (!PS01_SetAnalogValue(deviceId, POWERSTEP01_TVAL_ACC, tval)) {
        ESP_LOGW(PS01_TAG, "%s() Unable to set tval acc", __FUNCTION__);
		return 1;
	}
	return 0;
}

int PS01_Param_SetTvalDec(uint8_t deviceId, float tval)
{
	if (!PS01_SetAnalogValue(deviceId, POWERSTEP01_TVAL_DEC, tval)) {
        ESP_LOGW(PS01_TAG, "%s() Unable to set tval dec", __FUNCTION__);
		return 1;
	}
	return 0;
}

int PS01_Param_SetTFast(uint8_t deviceId, tfast_t tfast)
{
	uint8_t toff = PS01_TFast_Time_to_RegVal(tfast.toff_fast);
	uint8_t fast = PS01_TFast_Time_to_RegVal(tfast.fast_step);
	uint32_t value = ((toff << 4 ) & 0xF0) | (fast & 0xF);
	PS01_Cmd_SetParam(deviceId, POWERSTEP01_T_FAST, value);
	uint32_t read = PS01_Cmd_GetParam(deviceId, POWERSTEP01_T_FAST);
	if (read != value) {
		ESP_LOGW(PS01_TAG, "%s() Unable to set t fast", __FUNCTION__);
		return 1;
	}
	return 0;
}

int PS01_Param_SetTOnMin(uint8_t deviceId, float mini)
{
	if (!PS01_SetAnalogValue(deviceId, POWERSTEP01_TON_MIN, mini)) {
        ESP_LOGW(PS01_TAG, "%s() Unable to set ton min", __FUNCTION__);
		return 1;
	}
	return 0;
}

int PS01_Param_SetTOffMin(uint8_t deviceId, float mini)
{
	if (!PS01_SetAnalogValue(deviceId, POWERSTEP01_TOFF_MIN, mini)) {
        ESP_LOGW(PS01_TAG, "%s() Unable to set toff min", __FUNCTION__);
		return 1;
	}
	return 0;
}

bool PS01_IsActive(uint8_t deviceId)
{
	uint32_t max_speed = PS01_Cmd_GetParam(deviceId, POWERSTEP01_MAX_SPEED);
	uint32_t min_speed = PS01_Cmd_GetParam(deviceId, POWERSTEP01_MIN_SPEED);
	uint32_t sum = max_speed + min_speed;
	return sum != 0;
}